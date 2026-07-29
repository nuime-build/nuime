// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#include "EngineTests.hpp"
#include <Nuime/Engine.hpp>

using namespace Nuime;

EngineTests::EngineTests(const Ishiko::TestNumber& number, const Ishiko::TestContext& context)
    : TestSequence(number, "Engine tests", context)
{
    append<Ishiko::HeapAllocationErrorsTest>("load test 1", LoadTest1);
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
