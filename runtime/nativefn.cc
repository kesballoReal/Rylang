#include "nativefn.hh"

#include "environment/environment.hh"
#include "eval/statements.hh"

#include <iostream>
#include <cmath>
#include <algorithm>

std::string process_escapes(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char next = s[i + 1];
            switch (next) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case '\\': result += '\\'; break;
                default: result += next; break;
            }
            i++;
        } else {
            result += s[i];
        }
    }
    return result;
}


void register_default_native_functions() {
    auto& registry = NativeRegistry::instance();

    registry.register_function("puts", [](const std::vector<RVPtr>& args, Environment*, std::size_t line) -> RVPtr {
        if (args.empty() || args[0]->kind != VAL_STRING) {
            runtime_err("puts: first argument must be a format string", line);
        }

        std::string fmt = std::dynamic_pointer_cast<StringValue>(args[0])->value;
        fmt = process_escapes(fmt);
        std::string output;
        size_t arg_idx = 1;

        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                char spec = fmt[i + 1];

                if (spec == '%') {
                    output += '%';
                } else {
                    if (arg_idx >= args.size()) {
                        runtime_err("puts: not enough arguments for format string", line);
                    }

                    RVPtr v = args[arg_idx++];

                    switch (spec) {
                        case 'd': {
                            auto iv = std::dynamic_pointer_cast<IntValue>(cast(v, VAL_INT, line));
                            output += std::to_string(iv->value);
                            break;
                        }
                        case 'f': {
                            RVPtr fv_val = cast(v, VAL_FLOAT, line);
                            if (fv_val->kind == VAL_FLOAT) {
                                auto fv = std::dynamic_pointer_cast<FloatValue>(fv_val);
                                output += std::to_string(fv->value);
                            } else {
                                auto iv = std::dynamic_pointer_cast<IntValue>(fv_val);
                                output += std::to_string(static_cast<double>(iv->value));
                            }
                            break;
                        }
                        case 's': {
                            auto sv = std::dynamic_pointer_cast<StringValue>(cast(v, VAL_STRING, line));
                            output += sv->value;
                            break;
                        }
                        case 'b': {
                            auto bv = std::dynamic_pointer_cast<BoolValue>(cast(v, VAL_BOOL, line));
                            output += (bv->value ? "true" : "false");
                            break;
                        }
                        case 'c': {
                            auto cv = std::dynamic_pointer_cast<CharValue>(cast(v, VAL_CHAR, line));
                            output += cv->value;
                            break;
                        }

                        default:
                            runtime_err("puts: unknown format specifier '%" + std::string(1, spec) + "'", line);
                    }
                }
                i++; // skip specifier
            } else {
                output += fmt[i];
            }
        }

        // Only print after all formatting succeeds
        std::cout << output << std::endl;
        return std::make_shared<NullValue>();
    });

    registry.register_function("gets", [](const std::vector<RVPtr>& args, Environment* env, std::size_t line) -> RVPtr {
        std::string prompt;
        if (!args.empty()) {
            if (args[0]->kind != VAL_STRING) {
                runtime_err("gets: argument must be a string", line);
            }
            prompt = std::dynamic_pointer_cast<StringValue>(args[0])->value;
        }

        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);

        return std::make_shared<StringValue>(input);
    });
}
