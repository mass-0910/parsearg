#pragma once

#include <map>
#include <string>
#include <vector>

enum parsearg_error_t {
    PARSE_OK = 0,
    PARSE_ERROR_LACK_OF_ESSENTIAL_ARGUMENTS,
    PARSE_ERROR_LACK_OF_OPTION_ARGUMENTS,
    PARSE_ERROR_INVALID_OPTIONS,
};

class parsearg {
public:
    /**
     * @brief Set argument
     *
     * @param argument_name Argument name
     * @param description Argument description
     * @param is_optional Set to true if this argument is not required.
     */
    void argument(const std::string& argument_name, const std::string& description, bool is_optional = false);

    /**
     * @brief Set option
     *
     * @param option_name Option name
     * @param description Option description
     * @param has_argument Set to true if this option has an argument of its own
     * @param short_option_name Option name abbreviated to one letter
     */
    void option(const std::string& option_name, const std::string& description, bool has_argument, char short_option_name = 0);

    /**
     * @brief Print usage message
     *
     * @param argument_descriptrions Messages after program name
     */
    void print_usage(const std::string& argument_descriptrions);

    /**
     * @brief Parse arguments to options
     *
     * @param argc The argc passed on main
     * @param argv The argv passed on main
     * @return The status of parsing arguments
     */
    parsearg_error_t parse(int argc, char* argv[]);

    /**
     * @brief Return true if an option which specified exists in arguments
     *
     * @param argument_name Specified argument name
     * @retval true Specified argument is in arguments
     * @retval false Specified argument is not in arguments
     */
    bool contains_argument(const std::string& argument_name);

    /**
     * @brief Return true if an option which specified exists in arguments
     *
     * @param option_name Specified option name
     * @retval true Specified option is in arguments
     * @retval false Specified option is not in arguments
     */
    bool contains_option(const std::string& option_name);

    /**
     * @brief Get argument value of argument
     *
     * @param name The name of arguments
     * @param is_option Set true if argument argument is option
     * @return The value of specified arguments
     */
    std::string parsed_value(const std::string& name, bool is_option);

    /**
     * @brief Get program name
     *
     * @return program name
     */
    std::string get_program_name();

private:
    using parsed_arg_t = std::map<std::string, std::string>;

    struct argument_record_t {
        std::string name;
        std::string description;
        bool is_optional;
    };

    struct option_record_t {
        std::string name;
        std::string description;
        char short_name;
        bool has_argument;
    };

    parsearg_error_t parse_long_option(int& i, const std::vector<std::string>& arg_list);
    parsearg_error_t parse_char_option(int& i, const std::vector<std::string>& arg_list);
    option_record_t find_by_name(const std::vector<option_record_t>& list, const std::string& name) const;

    parsed_arg_t parsed_args;
    std::map<char, option_record_t> short_option;
    char* argv_0;
    std::vector<argument_record_t> argument_list;
    std::vector<option_record_t> option_list;
    unsigned int argument_len_max = 0;
    unsigned int option_len_max = 0;
    int required_argument_num = 0;
    bool optional_argument_has_passed = false;
};
