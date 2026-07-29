// SPDX-FileCopyrightText: 2026 Xavier Leclercq
// SPDX-License-Identifier: MIT

#ifndef GUARD_NUIME_CLI_TESTS_EXPORTCOMMANDTESTS_HPP
#define GUARD_NUIME_CLI_TESTS_EXPORTCOMMANDTESTS_HPP

#include <Ishiko/TestFramework.hpp>

class ExportCommandTests : public Ishiko::TestSequence
{
public:
    ExportCommandTests(const Ishiko::TestNumber& number, const Ishiko::TestContext& context);

private:
    static void ExportTest1(Ishiko::TestSequence& test_sequence);
    static void ExportTest2(Ishiko::TestSequence& test_sequence);
};

#endif
