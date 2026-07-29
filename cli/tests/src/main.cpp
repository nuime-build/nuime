// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "ExportCommandTests.hpp"
#include <Ishiko/BasePlatform.hpp>
#include <Ishiko/TestFramework.hpp>
#include <exception>

using namespace Ishiko;

int main(int argc, char* argv[])
{
    try
    {
        TestHarness::CommandLineSpecification command_line_spec;
        Configuration configuration = command_line_spec.createDefaultConfiguration();
        configuration.set("context.data", "../../data");
        configuration.set("context.reference", "../../reference");
        configuration.set("context.output", "../../output");
#if ISHIKO_OS == ISHIKO_OS_LINUX
        configuration.set("context.application-path", "../../../../bin/nuime");
#elif ISHIKO_OS == ISHIKO_OS_WINDOWS
        configuration.set("context.application-path", "../../../../bin/x64/nuime.exe");
#else
#error Unsupported or unrecognized OS
#endif
        CommandLineParser::parse(command_line_spec, argc, argv, configuration);

        TestHarness the_test_harness("Nuime CLI Tests", configuration);

        TestSequence& the_tests = the_test_harness.tests();
        the_tests.append<ExportCommandTests>();

        return the_test_harness.run();
    }
    catch (const std::exception& e)
    {
        return TestApplicationReturnCode::exception;
    }
    catch (...)
    {
        return TestApplicationReturnCode::exception;
    }
}
