#include "ArgParser.h"

using namespace ArgumentParser;

bool ArgParser::Parse(const std::vector<std::string>& argv) {
    CheckMultiPositionalArgument();
    for (size_t i = 1; i < argv.size(); ++i) {
        std::string_view line = argv[i];
        if ((line.length() == 1 && line[0] == '-') || (line[0] == '-' && line[1] == '-' && line.length() == 2)) {
            return false;
        }
        if (line.length() >= kShortArgSize) {
            if (is_help_given_ && (line.substr(kStartOfShortArg) == short_arg_help_ || line.substr(kStartOfLongArg) == long_arg_help_)) {
                is_help_detected_ = true;
                continue;
            }
            std::string short_arg;
            std::string long_arg;
            std::string value;
            if (line.length() == kShortArgSize && line[0] == '-') {
                if (CheckInclusiveOfArgument(line[1])) {
                    AddToPositional(argv[i]);
                    continue; 
                }
                short_arg = line.substr(1);
                long_arg = map_of_short_arguments[short_arg];
                if (map_of_bool_arguments.contains(long_arg)) {
                    AddValueForArgument(long_arg);
                    continue;
                }
                value = argv[++i];
            } else if (line[0] == '-' && line[1] != '-') {
                size_t equal_pos = line.find('=');
                if (equal_pos == std::string::npos) {
                    if (CheckArgsInMapOfBoolArgs(argv[i].substr(kStartOfShortArg))) {
                        for (const char& short_arg_c : line.substr(kStartOfShortArg)) {
                            long_arg = map_of_short_arguments[std::string(1, short_arg_c)];
                            AddValueForArgument(long_arg);
                        }
                    } else {
                        AddToPositional(argv[i]);
                    }
                    continue;
                }
                if (equal_pos == kStartOfShortArg || equal_pos == (line.length() - 1)) {
                    AddToPositional(argv[i]);
                    continue;
                }
                short_arg = line.substr(kStartOfShortArg, equal_pos - kStartOfShortArg);
                value = line.substr(equal_pos + 1);
                if (CheckInclusiveOfArgument(short_arg[0])) {
                    AddToPositional(argv[i]);
                    continue;
                }
                long_arg = map_of_short_arguments[short_arg];
            } else if (line[0] == '-' && line[1] == '-') {
                size_t equal_pos = line.find('=');
                long_arg = line.substr(kStartOfLongArg);
                if (equal_pos == std::string::npos) {
                    if (map_of_bool_arguments.contains(long_arg)) {
                        AddValueForArgument(long_arg);
                        continue;
                    }
                    if (CheckInclusiveOfArgument(long_arg)) {
                        return false;
                    }
                    value = argv[++i];
                } else if (equal_pos == (line.length() - 1)) {
                    return false;
                } else if (equal_pos == kStartOfLongArg) {
                    AddToPositional(argv[i]);
                    continue;
                } else {
                    long_arg = line.substr(kStartOfLongArg, equal_pos - kStartOfLongArg);
                    value = line.substr(equal_pos + 1);
                    if (CheckInclusiveOfArgument(long_arg)) {
                        return false;
                    }
                }
            } else {
                AddToPositional(argv[i]);
                continue;
            }
            AddValueForArgument(long_arg, value);
        } else {
            AddToPositional(argv[i]);
        }
    }
    return ArgParser::CheckAllArguments() || is_help_detected_;
}

bool ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> parse_lines(argc);
    for (size_t i = 0; i < argc; ++i) {
        parse_lines[i] = argv[i];
    }
    return ArgParser::Parse(parse_lines);
}

Argument<std::string>& ArgParser::AddStringArgument(const std::string& long_arg, const std::string& information_of_arg) {
    CheckInitArgumentsForDublicates(long_arg);
    map_of_string_arguments.insert({long_arg, Argument<std::string>("", long_arg, information_of_arg)});
    return map_of_string_arguments[long_arg];
}

Argument<std::string>& ArgParser::AddStringArgument(char short_arg, const std::string& long_arg, const std::string& information_of_arg) {
    CheckInitArgumentsForDublicates(short_arg, long_arg);
    std::string string_short_arg(1, short_arg);
    map_of_short_arguments[string_short_arg] = long_arg; 
    map_of_string_arguments.insert({long_arg, Argument<std::string>(string_short_arg, long_arg, information_of_arg)});
    return map_of_string_arguments[long_arg];
}

std::string ArgParser::GetStringValue(const std::string& long_arg) {
    if (!map_of_string_arguments.contains(long_arg)) {
        throw std::runtime_error("Undefined <string> argument --" + long_arg);
    }
    return map_of_string_arguments[long_arg].GetValue();
}

std::string ArgParser::GetStringValue(char short_arg) {
    std::string string_short_arg(1, short_arg);
    if (!map_of_short_arguments.contains(string_short_arg)) {
        throw std::runtime_error("Undefined <string> argument -" + string_short_arg);
    }
    return map_of_string_arguments[map_of_short_arguments[string_short_arg]].GetValue();
}

std::string ArgParser::GetStringValue(const std::string& long_arg, int index) {
    if (!map_of_string_arguments.contains(long_arg)) {
        throw std::runtime_error("Undefined <string> argument --" + long_arg);
    }
    return map_of_string_arguments[long_arg].GetValue(index);
}

std::string ArgParser::GetStringValue(char short_arg, int index) {
    std::string string_short_arg(1, short_arg);
    if (!map_of_short_arguments.contains(string_short_arg)) {
        throw std::runtime_error("Undefined <string> argument -" + string_short_arg);
    }
    return map_of_string_arguments[map_of_short_arguments[string_short_arg]].GetValue(index);
}

Argument<int>& ArgParser::AddIntArgument(const std::string& long_arg, const std::string& information_of_arg) {
    CheckInitArgumentsForDublicates(long_arg);
    map_of_int_arguments.insert({long_arg, Argument<int>("", long_arg, information_of_arg)});
    return map_of_int_arguments[long_arg];
}

Argument<int>& ArgParser::AddIntArgument(char short_arg, const std::string& long_arg, const std::string& information_of_arg) {
    CheckInitArgumentsForDublicates(short_arg, long_arg);
    std::string string_short_arg(1, short_arg);
    map_of_short_arguments[string_short_arg] = long_arg; 
    map_of_int_arguments.insert({long_arg, Argument<int>(string_short_arg, long_arg, information_of_arg)});
    return map_of_int_arguments[long_arg];
}

int ArgParser::GetIntValue(const std::string& long_arg) {
    if (!map_of_int_arguments.contains(long_arg)) {
        throw std::runtime_error("Undefined <int> argument --" + long_arg);
    }
    return map_of_int_arguments[long_arg].GetValue();
}

int ArgParser::GetIntValue(char short_arg) {
    std::string string_short_arg(1, short_arg);
    if (!map_of_short_arguments.contains(string_short_arg)) {
        throw std::runtime_error("Undefined <int> argument -" + string_short_arg);
    }
    return map_of_int_arguments[map_of_short_arguments[string_short_arg]].GetValue();
}
int ArgParser::GetIntValue(const std::string& long_arg, int index) {
    if (!map_of_int_arguments.contains(long_arg)) {
        throw std::runtime_error("Undefined <int> argument --" + long_arg);
    }
    return map_of_int_arguments[long_arg].GetValue(index);
}

int ArgParser::GetIntValue(char short_arg, int index) {
    std::string string_short_arg(1, short_arg);
    if (!map_of_short_arguments.contains(string_short_arg)) {
        throw std::runtime_error("Undefined <int> argument -" + string_short_arg);
    }
    return map_of_int_arguments[map_of_short_arguments[string_short_arg]].GetValue(index);
}

Argument<bool>& ArgParser::AddFlag(const std::string& long_arg, const std::string& information_of_arg) {
    CheckInitArgumentsForDublicates(long_arg);
    map_of_bool_arguments.insert({long_arg, Argument<bool>("", long_arg, information_of_arg)});
    return map_of_bool_arguments[long_arg];
}

Argument<bool>& ArgParser::AddFlag(char short_arg, const std::string& long_arg, const std::string& information_of_arg) {
    CheckInitArgumentsForDublicates(short_arg, long_arg);
    std::string string_short_arg(1, short_arg);
    map_of_short_arguments[string_short_arg] = long_arg;
    map_of_bool_arguments.insert({long_arg, Argument<bool>(string_short_arg, long_arg, information_of_arg)});
    return map_of_bool_arguments[long_arg];
}

bool ArgParser::GetFlag(const std::string& long_arg) {
    if (!map_of_bool_arguments.contains(long_arg)) {
        throw std::runtime_error("Undefined <bool> flag --" + long_arg);
    }
    return map_of_bool_arguments[long_arg].GetValue();
}

bool ArgParser::GetFlag(char short_arg) {
    std::string string_short_arg(1, short_arg);
    if (!map_of_short_arguments.contains(string_short_arg)) {
        throw std::runtime_error("Undefined <bool> argument -" + string_short_arg);
    }
    return map_of_bool_arguments[map_of_short_arguments[string_short_arg]].GetValue();
}

bool ArgParser::GetFlag(const std::string& long_arg, int index) {
    if (!map_of_bool_arguments.contains(long_arg)) {
        throw std::runtime_error("Undefined <bool> flag --" + long_arg);
    }
    return map_of_bool_arguments[long_arg].GetValue(index);
}

bool ArgParser::GetFlag(char short_arg, int index) {
    std::string string_short_arg(1, short_arg);
    if (!map_of_short_arguments.contains(string_short_arg)) {
        throw std::runtime_error("Undefined <bool> argument -" + string_short_arg);
    }
    return map_of_bool_arguments[map_of_short_arguments[string_short_arg]].GetValue(index);
}

void ArgParser::AddHelp(char short_arg, const std::string& long_arg, const std::string& information_of_arg) {
    if (!CheckInclusiveOfArgument(long_arg) || !CheckInclusiveOfArgument(short_arg)) {
        throw std::runtime_error("The key for argument has already added");
    }
    is_help_given_ = true;
    long_arg_help_ = long_arg;
    short_arg_help_ = std::string(1, short_arg);
    information_of_help_arg_ = information_of_arg;
}

bool ArgParser::Help() const {
    return is_help_detected_;
}

std::string ArgParser::HelpDescription() {
    std::string desription;
    desription += parser_name_ + "\n";
    desription += information_of_help_arg_ + "\n\n";
    for (const auto& pair : map_of_string_arguments) {
        if (!pair.second.GetShortArg().empty()) {
            desription += "-" + pair.second.GetShortArg() + ", ";
        } else {
            desription += "    ";
        }
        desription += "--" + pair.first + "=<string>" + ",  " + pair.second.GetInformation();
        if (pair.second.IsDefaultSet() && !pair.second.IsMultiValue()) {
            desription += " [default = ";
            std::string default_value = pair.second.GetDefaultValue();
            desription += default_value + "]";
        } else if (pair.second.IsMultiValue()) {
            desription += " [repeated, min args = ";
            desription += std::to_string(pair.second.GetMinArgsQuantity()) + "]";
        }
        desription += "\n";
    }
    for (const auto& pair : map_of_bool_arguments) {
        if (!pair.second.GetShortArg().empty()) {
            desription += "-" + pair.second.GetShortArg() + ", ";
        } else {
            desription += "    ";
        }
        desription += "--" + pair.first + ",  " + pair.second.GetInformation();
        if (pair.second.IsDefaultSet()) {
            desription += " [default = ";
            bool default_value = pair.second.GetDefaultValue();
            if (default_value) {
                desription += "true]";
            } else {
                desription += "false]";
            }
        }
        desription += "\n";
    }
    for (const auto& pair : map_of_int_arguments) {
        if (!pair.second.GetShortArg().empty()) {
            desription += "-" + pair.second.GetShortArg() + ", ";
        } else {
            desription += "    ";
        }
        desription += "--" + pair.first + "=<int>" + ",  " + pair.second.GetInformation();
        if (pair.second.IsDefaultSet() && !pair.second.IsMultiValue()) {
            desription += " [default = ";
            int default_value = pair.second.GetDefaultValue();
            desription += std::to_string(default_value) + "]";
        } else if (pair.second.IsMultiValue()) {
            desription += " [repeated, min args = ";
            desription += std::to_string(pair.second.GetMinArgsQuantity()) + "]";
        }
        desription += "\n";
    }
    desription += "-" + short_arg_help_ + ", " + "--" + long_arg_help_ + " " + information_of_help_arg_ + "\n";
    return desription;
}

void ArgParser::AddToPositional(const std::string& value) {
    if (IsInteger(value)) {
        for (auto& pair : map_of_int_arguments) {
            if (pair.second.GetPositional() && !pair.second.GetValueSet() && !pair.second.IsMultiValue()) {
                pair.second.SetValue(std::stoi(value));
                return;
            }
        }
        for (auto& pair : map_of_int_arguments) {
            if (pair.second.GetPositional() && pair.second.IsMultiValue()) {
                pair.second.SetValue(std::stoi(value));
                return;
            }
        }
    }
    for (auto& pair : map_of_string_arguments) {
        if (pair.second.GetPositional() && !pair.second.GetValueSet() && !pair.second.IsMultiValue()) {
            pair.second.SetValue(value);
            return;
        }
    }
    for (auto& pair : map_of_string_arguments) {
        if (pair.second.GetPositional() && pair.second.IsMultiValue()) {
            pair.second.SetValue(value);
            return;
        }
    }
    throw std::runtime_error("Incorrect value were given: " + value);
}

void ArgParser::AddValueForArgument(const std::string& long_arg, const std::string& value) {
    if (map_of_string_arguments.contains(long_arg)) {
        map_of_string_arguments[long_arg].SetValue(value);
    } else if (map_of_int_arguments.contains(long_arg)) {
        if (IsInteger(value)) {
            map_of_int_arguments[long_arg].SetValue(std::stoi(value));
        } else {
            throw std::runtime_error("Given value is not <int>");
        }
    } else if (map_of_bool_arguments.contains(long_arg)) {
        map_of_bool_arguments[long_arg].SetValue(true);
    }
}

bool ArgParser::CheckAllArguments() {
    for (auto& pair : map_of_string_arguments) {
        if (!pair.second.IsValueSet() && !pair.second.IsDefaultSet()) {
            return false;
        }
    }
    for (auto& pair : map_of_int_arguments) {
        if (!pair.second.IsValueSet() && !pair.second.IsDefaultSet()) {
            return false;
        }
    }
    return true;
}

bool ArgParser::CheckInclusiveOfArgument(const std::string& long_arg) const {
    if (map_of_string_arguments.contains(long_arg) || map_of_int_arguments.contains(long_arg) ||
     map_of_bool_arguments.contains(long_arg) || is_help_given_ && long_arg_help_ == long_arg) {
        return false;
    }
    return true;
}

bool ArgParser::CheckInclusiveOfArgument(char short_arg) const {
    std::string string_short_arg(1, short_arg);
    if (map_of_short_arguments.contains(string_short_arg)) {
        return false;
    }
    return true;
}

void ArgParser::CheckMultiPositionalArgument() const {
    bool multi_positional_detected = false;
    for (const auto& pair : map_of_string_arguments) {
        CheckForMultiPositional(multi_positional_detected, pair.second);
    }
    multi_positional_detected = false;
    for (const auto& pair : map_of_int_arguments) {
        CheckForMultiPositional(multi_positional_detected, pair.second);
    }
}

bool ArgParser::IsInteger(const std::string& value) const {
    try {
        size_t pos;
        std::stoi(value, &pos);
        return (pos == value.size());
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}

bool ArgParser::CheckArgsInMapOfBoolArgs(const std::string& line) {
    for (const char& short_arg : line) {
        if (map_of_short_arguments.contains(std::string(1, short_arg))) {
            std::string long_arg = map_of_short_arguments[std::string(1, short_arg)];
            if (map_of_bool_arguments.contains(long_arg)) {
                return true;
            }
        } else {
            return false;
        }
    }
    return false;
}

void ArgParser::CheckForMultiPositional(bool& multi_positional_detected, auto& pair) const {
    if (multi_positional_detected && pair.GetPositional() && pair.IsMultiValue()) {
        throw std::runtime_error("Parser detected 2 multi value positional arguments");
    } else if (pair.GetPositional() && pair.IsMultiValue()) {
        multi_positional_detected = true;
    }
}

void ArgParser::CheckInitArgumentsForDublicates(const std::string& long_arg) {
    if (!CheckInclusiveOfArgument(long_arg)) {
        throw std::runtime_error("The key for argument has already added: " + long_arg);
    }
}

void ArgParser::CheckInitArgumentsForDublicates(char short_arg, const std::string& long_arg) {
    if (!CheckInclusiveOfArgument(short_arg) || !CheckInclusiveOfArgument(long_arg)) {
        throw std::runtime_error("The key for argument has already added: " + long_arg);
    }
}
