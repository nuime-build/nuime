// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "CommandLineSpecification.hpp"
#include <Nuime.hpp>
#include <Nuime/BuildFiles/linkoptions.hpp>
#include <Ishiko/Color.hpp>
#include <Ishiko/Errors.hpp>
#include <Ishiko/Terminal.hpp>
#include <sstream>

using namespace Nuime;

int main(int argc, char* argv[])
{
    try
    {
        CommandLineSpecification command_line_spec;
        Ishiko::Configuration configuration = command_line_spec.createDefaultConfiguration();
        Ishiko::CommandLineParser::parse(command_line_spec, argc, argv, configuration);

        Ishiko::Error error;
        error.extensions().install<Ishiko::InfoErrorExtension>();

        const Ishiko::Configuration& command_configuration = configuration.value("command").asConfiguration();
        const std::string& command_name = command_configuration.value("name").asString();
        if (command_name == "build")
        {
            const std::string& build_file = command_configuration.value("build-file").asString();

            // TODO
        }
        else if (command_name == "export")
        {
            const std::string& build_file = command_configuration.value("build-file").asString();

            Engine engine;
            engine.load(build_file, error);
            if (!error)
            {
                engine.exportToCMake("CMakeLists.txt", error);
            }
        }

        if (error)
        {
            std::stringstream message;
            message << "ERROR: " << error << std::endl;
            Ishiko::TerminalOutput(stderr).write(message.str(), Ishiko::ColorName::red);
        }

        return error.code().value();
    }
    catch (const std::exception& e)
    {
        std::stringstream message;
        message << "EXCEPTION: " << e.what() << std::endl;
        Ishiko::TerminalOutput(stderr).write(message.str(), Ishiko::ColorName::red);

        // TODO
        return -1;
    }
    catch (...)
    {
        // TODO
        return -1;
    }
}
