// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "Engine.hpp"
#include <Nuime/BuildFiles/CMake/CMakeListsWriter.hpp>
#include <Nuime/BuildFiles/Nuime/NuimeWellKnownStrings.hpp>
#include <boost/filesystem.hpp>

using namespace Nuime;

namespace
{

// Maps a nuime configuration axis value to the CMake configuration name it corresponds to.
std::string ToCMakeConfiguration(const std::string& configuration)
{
    if (configuration == "debug")
    {
        return "Debug";
    }
    if (configuration == "release")
    {
        return "Release";
    }
    return configuration;
}

// Maps a nuime architecture axis value to the Visual Studio platform name CMake exposes through
// CMAKE_VS_PLATFORM_NAME (which reflects the -A option), e.g. x86 -> Win32.
std::string ToVSPlatformName(const std::string& architecture)
{
    if (architecture == "x86")
    {
        return "Win32";
    }
    if (architecture == "x64")
    {
        return "x64";
    }
    if (architecture == "arm64")
    {
        return "ARM64";
    }
    return architecture;
}

// Compiles a configuration tag to the piece of an OUTPUT_NAME it contributes: a build-time choice in
// multi-config generators, so one $<CONFIG:...> expression per non-empty value.
std::string CompileConfigurationTag(const NuimeStructuredFilename::Tag& tag)
{
    std::string result;
    for (const auto& value : tag.values())
    {
        if (!value.second.empty())
        {
            result += "$<$<CONFIG:" + ToCMakeConfiguration(value.first) + ">:" + value.second + ">";
        }
    }
    return result;
}

// Writes a set() command for each recognised source/header input group and records a reference to the
// variable it defines, and gathers the user include directories declared on those groups. Paths are
// resolved against each group's base (relative to the build file's directory) and then re-expressed
// relative to the generated CMakeLists.txt directory.
void WriteInputGroups(CMakeListsWriter& writer, const std::vector<NuimeInputGroup>& input_groups,
    const boost::filesystem::path& build_file_dir, const boost::filesystem::path& output_dir,
    std::vector<std::string>& variable_references, std::vector<std::string>& private_include_directories)
{
    for (const NuimeInputGroup& input_group : input_groups)
    {
        std::string variable_name;
        if (input_group.hasLabel(WellKnownStrings::k_cpp_source))
        {
            variable_name = "SOURCE_FILES";
        }
        else if (input_group.hasLabel(WellKnownStrings::k_cpp_header))
        {
            variable_name = "HEADER_FILES";
        }
        else
        {
            // Only recognised input kinds map to a variable for now.
            continue;
        }

        boost::filesystem::path base(input_group.base());
        std::vector<std::string> files;
        for (const NuimeInput& input : input_group.inputs())
        {
            boost::filesystem::path source = base / input.asString();
            if (source.is_relative())
            {
                source = build_file_dir / source;
            }
            source = source.lexically_normal();
            files.push_back(source.lexically_relative(output_dir).generic_string());
        }

        writer.writeBlankLine();
        writer.writeSetCommand(variable_name, files);
        variable_references.push_back("${" + variable_name + "}");

        for (const NuimeProperty& property : input_group.properties().properties())
        {
            if (property.name() == WellKnownStrings::k_cpp_user_include_directories)
            {
                boost::filesystem::path directory(property.value());
                if (directory.is_relative())
                {
                    directory = build_file_dir / directory;
                }
                directory = directory.lexically_normal();
                private_include_directories.push_back(directory.lexically_relative(output_dir).generic_string());
            }
        }
    }
}

}

void Engine::load(const boost::filesystem::path& path, Ishiko::Error& error)
{
    m_build_file.load(path, error);
}

void Engine::exportToCMake(const boost::filesystem::path& output_path, Ishiko::Error& error)
{
    CMakeListsWriter writer(output_path, error);
    if (error)
    {
        return;
    }

    writer.writeCMakeMinimumRequiredCommand("3.16");
    writer.writeProjectCommand(m_build_file.name());

    // A group's base and inputs give a path relative to the build file's directory (unless the base
    // is absolute). CMake needs each source relative to the generated CMakeLists.txt instead.
    boost::filesystem::path build_file_dir =
        boost::filesystem::absolute(m_build_file.path()).parent_path().lexically_normal();
    boost::filesystem::path output_dir =
        boost::filesystem::absolute(output_path).parent_path().lexically_normal();

    for (const NuimeRecipe& recipe : m_build_file.recipes())
    {
        const NuimeTarget& target = recipe.target();

        // Check for the executable label first, then static library. For now we only handle these two
        // and silently ignore everything else.
        bool is_executable = target.hasLabel(WellKnownStrings::k_executable);
        bool is_static_library = target.hasLabel(WellKnownStrings::k_static_library);
        if (!is_executable && !is_static_library)
        {
            continue;
        }

        // Each recognised input group becomes a CMake variable (referenced by add_library/add_executable),
        // and the user include directories declared on those groups become the target's PRIVATE include
        // directories.
        std::vector<std::string> variable_references;
        std::vector<std::string> private_include_directories;
        WriteInputGroups(writer, recipe.inputGroups(), build_file_dir, output_dir, variable_references,
            private_include_directories);

        // The artifact name is the recipe's first output, falling back to the target name. The base of
        // that same output group, if any, is the directory the built artifact should be placed in.
        std::string name = target.name();
        std::string output_directory;
        const NuimeOutput* artifact = nullptr;
        for (const NuimeOutputGroup& output_group : recipe.outputGroups())
        {
            if (!output_group.outputs().empty())
            {
                const NuimeOutput& output = output_group.outputs()[0];
                name = output.asString();
                output_directory = output_group.base();
                artifact = &output;
                break;
            }
        }

        writer.writeBlankLine();
        if (is_executable)
        {
            writer.writeAddExecutableCommand(name, variable_references);
        }
        else
        {
            writer.writeAddLibraryCommand(name, variable_references);
        }

        // A static library's output directory maps to ARCHIVE_OUTPUT_DIRECTORY. CMake resolves a
        // relative value of that property against the build tree, so anchor it to the source tree
        // (CMAKE_CURRENT_SOURCE_DIR, i.e. the generated CMakeLists.txt directory) instead.
        if (is_static_library && !output_directory.empty())
        {
            boost::filesystem::path directory(output_directory);
            if (directory.is_relative())
            {
                directory = build_file_dir / directory;
            }
            directory = directory.lexically_normal();
            std::string relative = directory.lexically_relative(output_dir).generic_string();
            writer.writeBlankLine();
            writer.writeSetTargetPropertiesCommand(name, "ARCHIVE_OUTPUT_DIRECTORY",
                "${CMAKE_CURRENT_SOURCE_DIR}/" + relative);
        }

        // An explicit filename layout maps to CMake's PREFIX and OUTPUT_NAME. Each tag is compiled to
        // the construct its axis resolves through (e.g. configuration -> a $<CONFIG:...> expression).
        if (artifact && artifact->hasFilename())
        {
            const NuimeStructuredFilename& filename = artifact->filename();
            if (!filename.prefix().empty())
            {
                writer.writeBlankLine();
                writer.writeSetTargetPropertiesCommand(name, "PREFIX", filename.prefix());
            }

            // OUTPUT_NAME is the stem plus each tag's contribution. The configuration axis compiles to a
            // generator expression; the architecture axis is resolved at configure time, so it is emitted
            // as a variable (set from CMAKE_VS_PLATFORM_NAME) that OUTPUT_NAME then references.
            std::string output_name = name;
            for (const NuimeStructuredFilename::Tag& tag : filename.tags())
            {
                if (tag.axis() == WellKnownStrings::k_configuration)
                {
                    output_name += CompileConfigurationTag(tag);
                }
                else if (tag.axis() == WellKnownStrings::k_architecture)
                {
                    std::vector<std::pair<std::string, std::string>> cases;
                    for (const auto& value : tag.values())
                    {
                        cases.push_back(std::make_pair(ToVSPlatformName(value.first), value.second));
                    }
                    writer.writeBlankLine();
                    writer.writeStringSwitchCommand("NUIME_ARCH_TAG", "CMAKE_VS_PLATFORM_NAME", cases);
                    output_name += "${NUIME_ARCH_TAG}";
                }
            }
            writer.writeBlankLine();
            writer.writeSetTargetPropertiesCommand(name, "OUTPUT_NAME", output_name);
        }

        if (!private_include_directories.empty())
        {
            writer.writeBlankLine();
            writer.writeTargetIncludeDirectoriesCommand(name, "PRIVATE", private_include_directories);
        }
    }

    writer.close();
}
