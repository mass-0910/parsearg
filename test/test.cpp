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