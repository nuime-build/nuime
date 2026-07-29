// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "Engine.hpp"
#include <Nuime/BuildFiles/CMake/CMakeListsWriter.hpp>
#include <Nuime/BuildFiles/Nuime/NuimeWellKnownLabels.hpp>

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

        // The artifact name is the recipe's output and its inputs are the sources.
        std::string name = recipe.outputs().empty() ? target.name() : recipe.outputs()[0].asString();

        std::vector<std::string> source_files;
        for (const NuimeInput& input : recipe.inputs())
        {
            source_files.push_back(input.asString());
        }

        if (is_executable)
        {
            writer.writeAddExecutableCommand(name, source_files);
        }
        else
        {
            writer.writeAddLibraryCommand(name, source_files);
        }
    }

    writer.close();
}
