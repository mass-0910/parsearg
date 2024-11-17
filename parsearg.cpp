#include <algorithm>
#include <filesystem>
#include <iostream>

#include "parsearg.hpp"

using namespace parsearg;

std::string parser::get_program_name() {
    return program_name;
}

void parser::argument(const std::string& argument_name, const std::string& description, bool is_optional) {
    const bool last_is_optional = argument_list.empty() ? false : argument_list.back().is_optional;
    if (last_is_optional && !is_optional) {
        std::cerr << "You can't add required arguments after optional arguments." << std::endl;
        exit(1);
    }
    argument_list.push_back(argument_record_t{argument_name, description, is_optional});
}

void parser::option(const std::string& option_name, const std::string& description, bool has_argument, char short_option_name) {
    option_list.push_back(option_record_t{option_name, description, short_option_name, has_argument});
    if (short_option_name) {
        short_option[short_option_name] = option_list.back();
    }
}

void parser::print_usage(const std::string& argument_descriptions) {
    const auto get_length_max = [](const auto& list) {
        return std::max_element(list.begin(), list.end(), [](const auto& a, const auto& b) {
                return a.name.length() < b.name.length();
            })->name.length();
    };
    const auto print_argument_usage = [&get_length_max](std::vector<argument_record_t> argument_list) {
        const auto max_length = get_length_max(argument_list);
        std::cout << "Arguments:" << std::endl;
        for (auto arg : argument_list) {
            std::cout << "    " << std::setw(max_length + 4) << std::left << arg.name;
            std::cout << arg.description << std::endl;
        }
    };
    const auto print_option_usage = [&get_length_max](std::vector<option_record_t> option_list) {
        const auto max_length = get_length_max(option_list);
        std::cout << "Options:" << std::endl;
        std::string opt_print;
        for (auto opt : option_list) {
            if (opt.short_name) {
                std::cout << "    " << std::setw(max_length + 2) << std::left << std::string("--") + opt.name;
                std::cout << std::setw(8) << std::left << std::string(" | -") + opt.short_name;
            } else {
                std::cout << "    " << std::setw(max_length + 10) << std::left << std::string("--") + opt.name;
            }
            std::cout << opt.description << std::endl;
        }
    };

    std::cout << "usage: " << get_program_name() << " " << argument_descriptions << std::endl << std::endl;
    // Print argument usage
    if (argument_list.size() > 0) {
        print_argument_usage(argument_list);
        std::cout << std::endl;
    }
    // Print option usage
    if (option_list.size() > 0) {
        print_option_usage(option_list);
    }
}

parsearg_error_t parser::parse(int argc, char* argv[]) {
    const std::vector<std::string> arg_list(argv, argv + argc);
    const auto required_argument_num = std::count_if(argument_list.begin(), argument_list.end(), [](const argument_record_t& argrecord) { return !argrecord.is_optional; });
    int arg_num = 0;
    parsearg_error_t err;
    for (int i = 1; i < argc; i++) {
        const std::string arg = arg_list[i];
        if (arg.empty()) continue;
        if (arg.length() <= 1 || arg[0] != '-') {  // If the argument is not an option
            if (arg_num >= argument_list.size()) {
                std::cerr << "Too many arguments\nonly " << std::to_string(required_argument_num) << " arguments required" << std::endl;
                return PARSE_ERROR_TOO_MANY_ARGUMENTS;
            }
            parsed_args[argument_list[arg_num++].name] = arg;
            continue;
        }

        if (arg[1] == '-') {  // If the argument is long-name option
            err = parse_long_option(i, arg_list);
        } else {  // If the argument is abbreviated-name option
            err = parse_char_option(i, arg_list);
        }

        if (err != PARSE_OK) {
            return err;
        }
    }
    // Error if it lacks of required arguments
    if (arg_num < required_argument_num) {
        std::cerr << std::to_string(required_argument_num) << " arguments required" << std::endl;
        return PARSE_ERROR_LACK_OF_ESSENTIAL_ARGUMENTS;
    }
    // Register program name
    auto argv_0_str = std::string(argv[0]);
    if (std::filesystem::exists(argv_0_str)) {
        program_name = std::filesystem::path(argv_0_str).filename().string();
    } else {
        program_name = argv_0_str;
    }
    return PARSE_OK;
}

bool parser::contains_argument(const std::string& argument_name) {
    return parsed_args.find(argument_name) != parsed_args.end();
}

bool parser::contains_option(const std::string& option_name) {
    return parsed_options.find(option_name) != parsed_options.end();
}

std::string parser::parsed_value(const std::string& name, bool is_option) {
    if (is_option) {
        return parsed_options.at(name);
    } else {
        return parsed_args.at(name);
    }
}

parsearg_error_t parser::parse_long_option(int& i, const std::vector<std::string>& arg_list) {
    const std::string option_name = arg_list[i].substr(2);
    const auto option_record_p = std::find_if(option_list.begin(), option_list.end(), [&option_name](const option_record_t& e) { return e.name == option_name; });

    if (option_record_p == option_list.end()) {
        // Error if the option is not in the option_list
        std::cerr << "option " << arg_list[i] << " is not a valid option" << std::endl;
        return PARSE_ERROR_INVALID_OPTIONS;
    }

    if (!option_record_p->has_argument) {
        parsed_options[option_name] = "";
        return PARSE_OK;
    }

    // Parse next element as option argument
    if (i + 1 >= arg_list.size()) {
        // Error if there are no arguments after the option which requires an argument of its own
        std::cerr << arg_list[i] << " option requires an argument" << std::endl;
        return PARSE_ERROR_LACK_OF_OPTION_ARGUMENTS;
    }
    parsed_options[option_name] = arg_list[i++ + 1];

    return PARSE_OK;
}

parsearg_error_t parser::parse_char_option(int& i, const std::vector<std::string>& arg_list) {
    for (int j = 1; j < arg_list[i].length(); j++) {
        const char arg_char = arg_list[i][j];
        const auto found = short_option.find(arg_char);
        if (found == short_option.end()) {
            // Error if the option is not in the option_list
            std::cerr << std::string("option -") << arg_char << " is not a valid option." << std::endl;
            return PARSE_ERROR_INVALID_OPTIONS;
        }
        const auto& found_option = found->second;
        if (!found_option.has_argument) {
            parsed_options[found_option.name] = "";
            continue;
        }
        if (j + 1 < arg_list[i].length()) {
            parsed_options[found_option.name] = arg_list[i].substr(j + 1);
            return PARSE_OK;
        } else if (i + 1 < arg_list.size()) {
            parsed_options[found_option.name] = arg_list[i++ + 1];
            return PARSE_OK;
        } else {
            // Error if there are no arguments after the option which requires an argument of its own
            std::cerr << std::string("-") << arg_char << " option requires an argument" << std::endl;
            return PARSE_ERROR_LACK_OF_OPTION_ARGUMENTS;
        }
    }
    return PARSE_OK;
}