#include <vector>
#include <cstring>
#include <filesystem>

#include <gtest/gtest.h>

#include <parsearg.hpp>

// Test of argument parsing
TEST(parsearg_test, argument_test) {
    parsearg::parser pa;
    int argc = 3;
    char *argv[] = {"program_name", "arg1", "arg2"};
    pa.argument("foo", "foo is mock");
    pa.argument("bar", "bar is mock");
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_OK);
    EXPECT_TRUE(pa.contains_argument("foo"));
    EXPECT_TRUE(pa.contains_argument("bar"));
    EXPECT_FALSE(pa.contains_argument("baz"));
    EXPECT_EQ(pa.parsed_value("foo", false), "arg1");
    EXPECT_EQ(pa.parsed_value("bar", false), "arg2");
}

// Test of option parsing
TEST(parsearg_test, option_test) {
    parsearg::parser pa;
    int argc = 2;
    char *argv[] = {"program_name", "--opt2"};
    pa.option("opt1", "opt1 is mock", false);
    pa.option("opt2", "opt2 is mock", false);
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_OK);
    EXPECT_FALSE(pa.contains_option("opt1"));
    EXPECT_TRUE(pa.contains_option("opt2"));
}

// Test of parsing options with arguments of its own
TEST(parsearg_test, option_with_argument_test) {
    parsearg::parser pa;
    int argc = 5;
    char *argv[] = {"program_name", "--opt1", "arg1", "--opt2", "arg2"};
    pa.option("opt1", "opt1 is mock", true);
    pa.option("opt2", "opt2 is mock", true);
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_OK);
    EXPECT_TRUE(pa.contains_option("opt1"));
    EXPECT_TRUE(pa.contains_option("opt2"));
    EXPECT_EQ(pa.parsed_value("opt1", true), "arg1");
    EXPECT_EQ(pa.parsed_value("opt2", true), "arg2");
}

// Test for parsing option abbreviated to one character
TEST(parsearg_test, option_shortname_test) {
    parsearg::parser pa;
    int argc = 7;
    char *argv[] = {"program_name", "-A", "-B", "arg1", "-CD", "arg2", "-Earg3"};
    pa.option("optA", "optA is mock", false, 'A');
    pa.option("optB", "optB is mock", true, 'B');
    pa.option("optC", "optC is mock", false, 'C');
    pa.option("optD", "optD is mock", true, 'D');
    pa.option("optE", "optE is mock", true, 'E');
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_OK);
    EXPECT_TRUE(pa.contains_option("optA"));
    EXPECT_TRUE(pa.contains_option("optB"));
    EXPECT_TRUE(pa.contains_option("optC"));
    EXPECT_TRUE(pa.contains_option("optD"));
    EXPECT_TRUE(pa.contains_option("optE"));
    EXPECT_EQ(pa.parsed_value("optB", true), "arg1");
    EXPECT_EQ(pa.parsed_value("optD", true), "arg2");
    EXPECT_EQ(pa.parsed_value("optE", true), "arg3");
}

TEST(parsearg_test, argument_num_over) {
    parsearg::parser pa;
    int argc = 3;
    char *argv[] = {"program_name", "hoge", "fuga"};
    pa.argument("hoge", "hoge", false);
    pa.option("optA", "optA is mock", false, 'A');
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_ERROR_TOO_MANY_ARGUMENTS);
}

// Test for lack of essential argument
TEST(parsearg_test, lack_of_essential_argument) {
    parsearg::parser pa;
    int argc = 2;
    char *argv[] = {"program_name", "value1"};
    pa.argument("argA", "argA is mock", false);
    pa.argument("argB", "argB is mock", false);
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_ERROR_LACK_OF_ESSENTIAL_ARGUMENTS);
}

// Test for exit when assigned required argument after optional argument
TEST(parsearg_test, exit_when_assign_required_after_optional) {
    parsearg::parser pa;
    pa.argument("argA", "argA is mock", true);
    ASSERT_DEATH(pa.argument("argB", "argB is mock", false), "You can't add required arguments after optional arguments.");
}

// Test for lack of long option argument
TEST(parsearg_test, lack_of_long_option_argument) {
    parsearg::parser pa;
    int argc = 2;
    char *argv[] = {"program_name", "--optA"};
    pa.option("optA", "optA is mock", true);
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_ERROR_LACK_OF_OPTION_ARGUMENTS);
}

// Test for lack of short option argument
TEST(parsearg_test, lack_of_short_option_argument) {
    parsearg::parser pa;
    int argc = 2;
    char *argv[] = {"program_name", "-A"};
    pa.option("optA", "optA is mock", true, 'A');
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_ERROR_LACK_OF_OPTION_ARGUMENTS);
}

// Test for invalid long option
TEST(parsearg_test, invalid_long_option) {
    parsearg::parser pa;
    int argc = 2;
    char *argv[] = {"program_name", "--optB"};
    pa.option("optA", "optA is mock", true);
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_ERROR_INVALID_OPTIONS);
}

// Test for invalid short option
TEST(parsearg_test, invalid_short_option) {
    parsearg::parser pa;
    int argc = 2;
    char *argv[] = {"program_name", "-B"};
    pa.option("optA", "optA is mock", true, 'A');
    auto err = pa.parse(argc, argv);
    EXPECT_EQ(err, parsearg::PARSE_ERROR_INVALID_OPTIONS);
}

// Test for usage output
TEST(parsearg_test, parsearg_usage) {
    parsearg::parser pa;
    int argc = 7;
    char *argv[] = {"program_name", "build", "--input", "hoge.txt", "--output", "fuga.bin", "--quiet"};
    testing::internal::CaptureStdout();
    pa.argument("subcommand", "subcommand name", true);
    pa.option("input", "input file", true, 'i');
    pa.option("output", "output file", true, 'o');
    pa.option("quiet", "quiet mode", false);
    auto err = pa.parse(argc, argv);
    ASSERT_EQ(err, parsearg::PARSE_OK);

    pa.print_usage("<subcommand> --input <input file> --output <output file> [--quiet]");
    auto expect_usage_str =
        "usage: program_name <subcommand> --input <input file> --output <output file> [--quiet]\n"
        "\n"
        "Arguments:\n"
        "    subcommand    subcommand name\n"
        "\n"
        "Options:\n"
        "    --input  | -i   input file\n"
        "    --output | -o   output file\n"
        "    --quiet         quiet mode\n";
    ASSERT_STREQ(expect_usage_str, testing::internal::GetCapturedStdout().c_str());
}