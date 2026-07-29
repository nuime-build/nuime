// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "CommandLineSpecification.hpp"

using namespace Nuime;

CommandLineSpecification::CommandLineSpecification()
{
    addPositionalOption(1, "command", { Ishiko::CommandLineSpecification::OptionType::single_value });

    Ishiko::CommandLineSpecification::CommandDetails& build_command_details = addCommand("command", "build");
    build_command_details.addPositionalOption(2, "build-file",
        {Ishiko::CommandLineSpecification::OptionType::single_value});

    Ishiko::CommandLineSpecification::CommandDetails& export_command_details = addCommand("command", "export");
    export_command_details.addPositionalOption(2, "build-file",
        {Ishiko::CommandLineSpecification::OptionType::single_value});
    export_command_details.addPositionalOption(3, "output-file",
        {Ishiko::CommandLineSpecification::OptionType::single_value});
}
