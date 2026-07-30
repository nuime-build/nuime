// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "EngineTests.hpp"
#include <Nuime/Engine.hpp>
#include <boost/filesystem.hpp>

using namespace Nuime;

EngineTests::EngineTests(const Ishiko::TestNumber& number, const Ishiko::TestContext& context)
    : TestSequence(number, "Engine tests", context)
{
    append<Ishiko::HeapAllocationErrorsTest>("load test 1", LoadTest1);
    append<Ishiko::HeapAllocationErrorsTest>("exportToCMake test 1", ExportToCMakeTest1);
    append<Ishiko::HeapAllocationErrorsTest>("exportToCMake test 2", ExportToCMakeTest2);
    append<Ishiko::HeapAllocationErrorsTest>("exportToCMake test 3", ExportToCMakeTest3);
    append<Ishiko::HeapAllocationErrorsTest>("exportToCMake test 4", ExportToCMakeTest4);
}

void EngineTests::LoadTest1(Ishiko::Test& test)
{
    boost::filesystem::path input_path = test.context().getDataPath("minimal.nuime");

    Engine engine;

    Ishiko::Error error;
    engine.load(input_path, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_PASS();
}

void EngineTests::ExportToCMakeTest1(Ishiko::Test& test)
{
    const char* output_name = "EngineTests_ExportToCMakeTest1.txt";
    boost::filesystem::path input_path = test.context().getDataPath("minimal.nuime");
    boost::filesystem::path output_path = test.context().getOutputPath(output_name);

    Engine engine;

    Ishiko::Error error;
    engine.load(input_path, error);
    ISHIKO_TEST_ABORT_IF(error);

    engine.exportToCMake(output_path, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(output_name);
    ISHIKO_TEST_PASS();
}

void EngineTests::ExportToCMakeTest2(Ishiko::Test& test)
{
    const char* output_name = "EngineTests_ExportToCMakeTest2.txt";
    boost::filesystem::path input_path = test.context().getOutputPath("minimal_static_library.nuime");
    boost::filesystem::copy_file(test.context().getDataPath("minimal_static_library.nuime"), input_path,
        boost::filesystem::copy_options::overwrite_existing);
    boost::filesystem::path output_path = test.context().getOutputPath(output_name);

    Engine engine;

    Ishiko::Error error;
    engine.load(input_path, error);
    ISHIKO_TEST_ABORT_IF(error);

    engine.exportToCMake(output_path, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(output_name);
    ISHIKO_TEST_PASS();
}

void EngineTests::ExportToCMakeTest3(Ishiko::Test& test)
{
    const char* output_name = "EngineTests_ExportToCMakeTest3.txt";
    boost::filesystem::path input_path = test.context().getOutputPath("minimal_executable.nuime");
    boost::filesystem::copy_file(test.context().getDataPath("minimal_executable.nuime"), input_path,
        boost::filesystem::copy_options::overwrite_existing);
    boost::filesystem::path output_path = test.context().getOutputPath(output_name);

    Engine engine;

    Ishiko::Error error;
    engine.load(input_path, error);
    ISHIKO_TEST_ABORT_IF(error);

    engine.exportToCMake(output_path, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(output_name);
    ISHIKO_TEST_PASS();
}

void EngineTests::ExportToCMakeTest4(Ishiko::Test& test)
{
    const char* output_name = "EngineTests_ExportToCMakeTest4.txt";
    boost::filesystem::path input_path = test.context().getOutputPath("minimal_base_library.nuime");
    boost::filesystem::copy_file(test.context().getDataPath("minimal_base_library.nuime"), input_path,
        boost::filesystem::copy_options::overwrite_existing);
    boost::filesystem::path output_path = test.context().getOutputPath(output_name);

    Engine engine;

    Ishiko::Error error;
    engine.load(input_path, error);
    ISHIKO_TEST_ABORT_IF(error);

    engine.exportToCMake(output_path, error);

    ISHIKO_TEST_FAIL_IF(error);
    ISHIKO_TEST_FAIL_IF_OUTPUT_AND_REFERENCE_FILES_NEQ(output_name);
    ISHIKO_TEST_PASS();
}
