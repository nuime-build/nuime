// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "Engine.hpp"
#include <Nuime/BuildFiles/CMake/CMakeListsWriter.hpp>
#include <Nuime/BuildFiles/Nuime/NuimeWellKnownLabels.hpp>
#include <boost/filesystem.hpp>

using namespace Nuime;

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
        bool is_executable = target.hasLabel(WellKnownLabels::k_executable);
        bool is_static_library = target.hasLabel(WellKnownLabels::k_static_library);
        if (!is_executable && !is_static_library)
        {
            continue;
        }

        // Each input group becomes its own CMake variable, named after the group's label. The
        // add_library/add_executable command then references those variables.
        std::vector<std::string> variable_references;
        for (const NuimeInputGroup& input_group : recipe.inputGroups())
        {
            std::string variable_name;
            if (input_group.hasLabel(WellKnownLabels::k_cpp_source))
            {
                variable_name = "SOURCE_FILES";
            }
            else if (input_group.hasLabel(WellKnownLabels::k_cpp_header))
            {
                variable_name = "HEADER_FILES";
            }
            else
            {
                // Only recognised input kinds map to a variable for now.
                continue;
            }

            // Each file is resolved against its group's base (relative to the build file's directory)
            // and then re-expressed relative to the generated CMakeLists.txt directory.
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

            writer.writeSetCommand(variable_name, files);
            variable_references.push_back("${" + variable_name + "}");
        }

        // The artifact name is the recipe's first output, falling back to the target name.
        std::string name = target.name();
        for (const NuimeOutputGroup& output_group : recipe.outputGroups())
        {
            if (!output_group.outputs().empty())
            {
                name = output_group.outputs()[0].asString();
                break;
            }
        }

        if (is_executable)
        {
            writer.writeAddExecutableCommand(name, variable_references);
        }
        else
        {
            writer.writeAddLibraryCommand(name, variable_references);
        }
    }

    writer.close();
}
