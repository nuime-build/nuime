// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "Engine.hpp"

using namespace Nuime;

void Engine::load(const boost::filesystem::path& path, Ishiko::Error& error)
{
    m_build_file.load(path, error);
}

void Engine::exportToCMake(const boost::filesystem::path& output_path, Ishiko::Error& error)
{
    // TODO: project the loaded model onto a CMakeLists.txt. The engine decides how to map the nuime
    // model onto CMake; for now this will handle our single static-library example.
}
