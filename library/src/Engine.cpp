// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "Engine.hpp"
#include <Nuime/BuildFiles/CMake/CMakeListsWriter.hpp>

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
        // For now we only handle the static-library case of our simple example: the library name is
        // the recipe's output and its inputs are the sources.
        std::string library_name = recipe.outputs().empty() ? recipe.target().name() : recipe.outputs()[0].asString();

        std::vector<std::string> source_files;
        for (const NuimeInput& input : recipe.inputs())
        {
            source_files.push_back(input.asString());
        }

        writer.writeAddLibraryCommand(library_name, source_files);
    }

    writer.close();
}
