// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#ifndef GUARD_NUIME_TESTS_ENGINETESTS_HPP
#define GUARD_NUIME_TESTS_ENGINETESTS_HPP

#include <Ishiko/TestFramework.hpp>

class EngineTests : public Ishiko::TestSequence
{
public:
    EngineTests(const Ishiko::TestNumber& number, const Ishiko::TestContext& context);

private:
    static void LoadTest1(Ishiko::Test& test);
    static void ExportToCMakeTest1(Ishiko::Test& test);
    static void ExportToCMakeTest2(Ishiko::Test& test);
    static void ExportToCMakeTest3(Ishiko::Test& test);
    static void ExportToCMakeTest4(Ishiko::Test& test);
    static void ExportToCMakeTest5(Ishiko::Test& test);
    static void ExportToCMakeTest6(Ishiko::Test& test);
};

#endif
