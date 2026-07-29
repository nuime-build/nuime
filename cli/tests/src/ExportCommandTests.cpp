// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "ExportCommandTests.hpp"

using namespace Ishiko;

ExportCommandTests::ExportCommandTests(const TestNumber& number, const TestContext& context)
    : TestSequence(number, "export command tests", context)
{
    ExportTest1(*this);
    ExportTest2(*this);
}

void ExportCommandTests::ExportTest1(TestSequence& test_sequence)
{
    const char* output_name = "ExportCommandTests_ExportTest1.txt";
    boost::filesystem::path application_path = test_sequence.context().getApplicationPath();
    boost::filesystem::path build_file_path = test_sequence.context().getDataPath("minimal_static_library.nuime");
    boost::filesystem::path output_path = test_sequence.context().getOutputPath(output_name);

    std::string command_line = (application_path.string() + " export " + build_file_path.string() + " " +
        output_path.string());

    test_sequence.append<ConsoleApplicationTest>("export command test 1", command_line,
        [output_name](int exit_code, ConsoleApplicationTest& test)
        {
            ISHIKO_TEST_FAIL_IF_NEQ(exit_code, 0);
            ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(output_name);
            ISHIKO_TEST_PASS();
        }
    );
}

void ExportCommandTests::ExportTest2(TestSequence& test_sequence)
{
    boost::filesystem::path application_path = test_sequence.context().getApplicationPath();
    boost::filesystem::path build_file_path = test_sequence.context().getDataPath("DoesNotExist.nuime");
    boost::filesystem::path output_path = test_sequence.context().getOutputPath("ExportCommandTests_ExportTest2.txt");

    std::string command_line = (application_path.string() + " export " + build_file_path.string() + " " +
        output_path.string());

    test_sequence.append<ConsoleApplicationTest>("export command test 2", command_line,
        [](int exit_code, ConsoleApplicationTest& test)
        {
            ISHIKO_TEST_FAIL_IF_NEQ(exit_code, -1);
            ISHIKO_TEST_PASS();
        }
    );
}
