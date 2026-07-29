// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#ifndef GUARD_NUIME_ENGINE_HPP
#define GUARD_NUIME_ENGINE_HPP

#include <Nuime/BuildFiles/Nuime/NuimeBuildFile.hpp>
#include <Ishiko/Errors.hpp>
#include <boost/filesystem/path.hpp>

namespace Nuime
{
    // High-level entry point driving nuime. The engine holds the loaded model and decides how to
    // project it onto a given output format. Today it works with a single build file; in future it
    // may span a whole organization (e.g. all of Ishiko).
    class Engine
    {
    public:
        void load(const boost::filesystem::path& path, Ishiko::Error& error);

        void exportToCMake(const boost::filesystem::path& output_path, Ishiko::Error& error);

    private:
        NuimeBuildFile m_build_file;
    };
}

#endif
