#pragma once
#include "Argument.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>

namespace ArgumentParser {

    class ArgParser {
    public:
        ArgParser() = default;
        explicit ArgParser(const std::string& parser_name) : parser_name_(parser_name){};

        ~ArgParser(){};

    // <String Arguments>
        // Adding <string> argument
        Argument<std::string>& AddStringArgument(const std::string& long_arg, const std::string& information_of_arg="");
        Argument<std::string>& AddStringArgument(char short_arg, const std::string& long_arg, const std::string& information_of_arg="");
        // Getting <string> argument
        std::string GetStringValue(const std::string& long_arg);
        std::string GetStringValue(char short_arg);
        std::string GetStringValue(const std::string& long_arg, int index);
        std::string GetStringValue(char short_arg, int index);

    // <Int Arguments>
        // Adding <int> argument
        Argument<int>& AddIntArgument(const std::string& long_arg, const std::string& information_of_arg="");
        Argument<int>& AddIntArgument(char short_arg, const std::string& long_arg, const std::string& information_of_arg="");
        // Getting <int> argument
        int GetIntValue(const std::string& long_arg);
        int GetIntValue(char short_arg);
        int GetIntValue(const std::string& long_arg, int index);
        int GetIntValue(char short_arg, int index);

    // <Bool Arguments>
        // Adding <bool> argument
        Argument<bool>& AddFlag(const std::string& long_arg, const std::string& information_of_arg="");
        Argument<bool>& AddFlag(char short_arg, const std::string& long_arg, const std::string& information_of_arg="");
        // Getting <bool> argument
        bool GetFlag(const std::string& long_arg, int index);
        bool GetFlag(char short_arg, int index);
        bool GetFlag(char short_arg);
        bool GetFlag(const std::string& long_arg);
    
    // Help
        // Add Help
        void AddHelp(char short_arg, const std::string& long_arg, const std::string& information_of_arg);
        bool Help() const;
        std::string HelpDescription();

    // Parse arguments
        bool Parse(int argc, char** argv);
        bool Parse(const std::vector<std::string>& argv);

    private:
        const std::string parser_name_ = "Parser";
        const int kShortArgSize = 2;
        const int kStartOfLongArg = 2;
        const int kStartOfShortArg = 1;
        std::unordered_map<std::string, Argument<std::string>> map_of_string_arguments;
        std::unordered_map<std::string, Argument<int>> map_of_int_arguments;
        std::unordered_map<std::string, Argument<bool>> map_of_bool_arguments;
        std::unordered_map<std::string, std::string> map_of_short_arguments;
        bool is_help_detected_ = false;
        bool is_help_given_ = false;
        std::string long_arg_help_;
        std::string short_arg_help_;
        std::string information_of_help_arg_ = "Some Description about program";

        bool CheckInclusiveOfArgument(const std::string& long_arg) const;
        bool CheckInclusiveOfArgument(char short_arg) const;
        void CheckMultiPositionalArgument() const;
        void AddValueForArgument(const std::string& long_arg, const std::string& value="");
        void AddToPositional(const std::string& value);
        bool IsInteger(const std::string& value) const;
        bool CheckAllArguments();
        bool CheckArgsInMapOfBoolArgs(const std::string& line);
        void CheckForMultiPositional(bool& multi_positional_detected, auto& pair) const;
        void CheckInitArgumentsForDublicates(const std::string& long_arg);
        void CheckInitArgumentsForDublicates(char short_arg, const std::string& long_arg);
        void ParseShortArgs();
    };
}
