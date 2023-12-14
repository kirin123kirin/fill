/* argparser.hpp | MIT License | https://github.com/kirin123kirin/cutil/raw/main/LICENSE */
#ifndef _ARGPARSE_HPP_
#define _ARGPARSE_HPP_


#include <string> /* for char_traits */
#include <vector>
#include <iostream>
#include <iomanip> // std::setw(int)
#include <typeindex> // std::type_index

#define _TXT_(str, type) std::get<const type *>(std::make_tuple(str, L##str))

enum class ITEM { REQUIRED, OPTION, NONE, NULL_t };

char escape_array[128] = {0,    1,   2,    3,   4,    5,   6,    7,    8,   9,   10,  11,  12,   13,  14,   15,
                            16,   17,  18,   19,  20,   21,  22,   23,   24,  25,  26,  27,  28,   29,  30,   31,
                            32,   33,  '\"', 35,  36,   37,  38,   '\'', 40,  41,  42,  43,  44,   45,  46,   47,
                            '\0', 49,  50,   51,  52,   53,  54,   55,   56,  57,  58,  59,  60,   61,  62,   '\?',
                            64,   65,  66,   67,  68,   69,  70,   71,   72,  73,  74,  75,  76,   77,  78,   79,
                            80,   81,  82,   83,  84,   85,  86,   87,   88,  89,  90,  91,  '\\', 93,  94,   95,
                            96,   97,  '\b', 99,  100,  101, '\f', 103,  104, 105, 106, 107, 108,  109, '\n', 111,
                            112,  113, '\r', 115, '\t', 117, '\v', 119,  120, 121, 122, 123, 124,  125, 126,  127};

template <typename CharT, typename Traits = std::char_traits<CharT>>
CharT* unescape(CharT* buf) {
    std::size_t len = Traits::length(buf);
    if(Traits::find(buf, len, '\\') == nullptr)
        return buf;
    CharT* p = buf;
    for(CharT *end = buf + len; p < end; ++p) {
        if(*p == '\\' && *(char*)++p > -1)
            *p = escape_array[*p];
    }
    *p = 0;
    return buf;
}

template <typename T> void _printany_(T s) { if(s) std::cout << s << std::flush; }
template <> void _printany_(const char* s) { if(s && *s) std::cout << s << std::flush; }
template <> void _printany_(const wchar_t* s) { if(s && *s) std::wcout << s << std::flush; }
template <> void _printany_(const wchar_t s) { if(s) std::wcout << s << std::flush; }
// template <> void _printany_(wchar_t* s) { if(s && *s) std::wcout << s << std::flush; }
// template <> void _printany_(wchar_t s) { if(s) std::wcout << s << std::flush; }
template <> void _printany_(const std::string& s) { if(s.empty()) std::cout << s << std::flush; }
template <> void _printany_(const std::wstring& s) { if(s.empty()) std::wcout << s << std::flush; }

template <typename T> void printany(T s) { _printany_(s); }

template <typename Char0, typename Char1>
void printany(const Char0 s0, const Char1 s1) {
    _printany_(s0);
    _printany_(s1);
}

template <typename Char0, typename Char1, typename Char2>
void printany(const Char0 s0, const Char1 s1, const Char2 s2) {
    printany(s0, s1);
    _printany_(s2);
}

template <typename Char0, typename Char1, typename Char2, typename Char3>
void printany(const Char0 s0, const Char1 s1, const Char2 s2, const Char3 s3) {
    printany(s0, s1);
    printany(s2, s3);
}
template <typename Char0, typename Char1, typename Char2, typename Char3, typename Char4>
void printany(const Char0 s0, const Char1 s1, const Char2 s2, const Char3 s3, const Char4 s4) {
    printany(s0, s1, s2);
    printany(s3, s4);
}

template <typename T> void _printerr_(T s) { if(s) std::cerr << s << std::flush; }
template <> void _printerr_(const char* s) {if(s && *s) std::cerr << s << std::flush;}
template <> void _printerr_(char* s) {if(s && *s) std::cerr << s << std::flush;}
template <> void _printerr_(const wchar_t* s) {if(s && *s) std::wcerr << s << std::flush;}
template <> void _printerr_(wchar_t* s) {if(s && *s) std::wcerr << s << std::flush;}
template <> void _printerr_(wchar_t s) {if(s) std::wcerr << s << std::flush;}
template <> void _printerr_(const std::string& s) {if(!s.empty()) std::cerr << s << std::flush;}
template <> void _printerr_(const std::wstring& s) {if(!s.empty()) std::wcerr << s << std::flush;}

template <typename T>
void printerr(T s) {
    std::cout << "" << std::flush;
    _printerr_(s);
}
template <typename Char0, typename Char1>
void printerr(const Char0 s0, const Char1 s1) {
    _printerr_(s0);
    _printerr_(s1);
}
template <typename Char0, typename Char1, typename Char2>
void printerr(const Char0 s0, const Char1 s1, const Char2 s2) {
    printerr(s0, s1);
    _printerr_(s2);
}

template <typename Char0, typename Char1, typename Char2, typename Char3>
void printerr(const Char0 s0, const Char1 s1, const Char2 s2, const Char3 s3) {
    printerr(s0, s1);
    printerr(s2, s3);
}

template <typename Char0, typename Char1, typename Char2, typename Char3, typename Char4>
void printerr(const Char0 s0, const Char1 s1, const Char2 s2, const Char3 s3, const Char4 s4) {
    printerr(s0, s1);
    printerr(s2, s3);
    _printerr_(s4);
}


template <typename CharT, typename Traits = std::char_traits<CharT>>
struct argstruct {
    const CharT* shortarg;
    const CharT* longarg;
    void* value;
    const CharT* helpstr;
    ITEM item;
    std::type_index type;
    int parsed;
    int length_of_match;

    template <typename T>
    argstruct(const CharT* _shortarg, const CharT* _longarg, T* _value, const CharT* _helpstr, ITEM _item = ITEM::NONE) : shortarg(_shortarg), longarg(_longarg), value(_value), helpstr(_helpstr), item(_item), type(typeid(std::remove_const_t<T>)), parsed(0), length_of_match(0) {}
    argstruct() : shortarg(NULL), longarg(NULL), value(NULL), helpstr(NULL), type(typeid(nullptr)), item(ITEM::NULL_t), parsed(0), length_of_match(0) {}

    const char* type_string() {
        if(type == typeid(bool))
            return "[BOOL]   ";
        if(type == typeid(int32_t)||type == typeid(int64_t))
            return "[INT]    ";
        if(type == typeid(uint32_t) || type == typeid(uint64_t))
            return "[UINT]   ";
        if(type == typeid(float))
            return "[FLOAT]  ";
        if(type == typeid(double))
            return "[DOUBLE] ";
        return     "[STRING] ";
    }

    bool operator==(const CharT* arg) { 
        if(arg && *arg && arg[0] == '-') {
            if (arg[1] == shortarg[1] && arg[2] == 0){
                length_of_match = 2;
                return true;
            }
            const CharT* lng = longarg;
            while(*arg && *lng && *arg++ == *lng++) {}
            if(*lng == 0) {
                length_of_match = lng - longarg;
                return true;
            }
        }
        return false;
    }

    void set(void) {
        if (parsed)
            std::cerr << "Warning: Already Parsed `" << shortarg << "`." << std::endl << std::flush;

        if(type == typeid(bool))
            *(bool*)value = *(bool*)value ? 0 : 1;
        ++parsed;
    }
    void set(CharT* src) {
        if (src == NULL || !src[0])
            return;
        if (parsed)
            std::cerr << "Warning: Already Parsed `" << shortarg << "`." << std::endl << std::flush;
        bool NotZero = !(src[0] == '0' && src[1] == 0);
        if(type == typeid(int32_t)) {
            if((*(int32_t*)value = std::stol(src)) == 0 && NotZero)
                force_abort();
        } else if (type == typeid(int64_t)) {
            if((*(int64_t*)value = std::stoll(src)) == 0 && NotZero)
                force_abort();
        } else if(type == typeid(uint32_t)) {
            if((*(uint32_t*)value = std::stoul(src)) == 0 && NotZero)
                force_abort();
        } else if (type == typeid(uint64_t)) {
            if((*(uint64_t*)value = std::stoull(src)) == 0 && NotZero)
                force_abort();
        } else if (type == typeid(float)) {
            if((*(float*)value = std::stof(src)) == 0 && NotZero)
                force_abort();
        } else if (type == typeid(double)) {
            if((*(double*)value = std::stod(src)) == 0 && NotZero)
                force_abort();
        } else if (type == typeid(CharT*)) {
            *(CharT**)value = src;
        } else if (type == typeid(CharT)) {
            *(CharT*)value = *src;
        } else {
            printerr("Error: Unknown This Type.\n\n");
            exit(EXIT_FAILURE);
        }
        ++parsed;
    }

    private:
    void force_abort() {
        printerr("Error: Ileagal Argument value of `", shortarg, "` expected TypeValue of", type.name(), "\n\n");
        exit(EXIT_FAILURE);
    }
};


template <typename CharT, typename Traits = std::char_traits<CharT>>
struct argparser {
    using type = CharT;
    int argc;
    CharT** argv;
    std::vector<argstruct<CharT>> args;
    const CharT* desc;
    const CharT* prog;
    std::vector<CharT*> positional_argv;

    argparser(int _argc, CharT** _argv, const CharT* _desc = NULL) : argc(_argc), argv(_argv), desc(_desc), prog(_argv[0]), positional_argv() {}

    template <typename T>
    void add(const CharT* _shortarg, const CharT* _longarg, T&& _value, const CharT* _helpstr, ITEM _item = ITEM::OPTION) {
        if(!_shortarg && !_longarg)
            abort("Passed NULL pointers to both argstrings\n\n");
        if(_shortarg && *_shortarg == 0)
            abort("Passed empty string to shortarg\n\n");
        if(_longarg && *_longarg == 0)
            abort("Passed empty string to longarg\n\n");
        if(_shortarg && (Traits::length(_shortarg) != 2 || _shortarg[0] != '-' || _shortarg[1] == '-'))
            abort("Shortarg must be a single dash followed by a single character\n\n");
        if(_longarg && (Traits::length(_longarg) <= 2 || _longarg[0] != '-' || _longarg[1] != '-'))
            abort("Longarg must be two dashes followed by any number of additional characters\n\n");
        if(_shortarg && _shortarg[0] == '-' && _shortarg[1] == 'h' && _shortarg[2] == 0)
            abort("-h is reserved short arg\n\n");
        if(_longarg && _longarg[0] == '-' && _longarg[1] == '-' && _longarg[2] == 'h' &&
           _longarg[3] == 'e' && _longarg[4] == 'l' && _longarg[5] == 'p' && _longarg[6] == 0)
                abort("--help is reserved long arg\n\n");
        args.emplace_back(_shortarg, _longarg, _value, _helpstr, _item);
    }

    void print_help() {
        std::size_t max_len = 10;  // mean length of "-h, --help";;
        std::size_t rightpad = 4;
        const char* leftpadstr = "    ";

        const CharT* c = prog + (Traits::length(prog) - 1);
        for(; prog != c; --c) {
            if(*c == '\\' || *c == '/')
                break;
        }

        printany("Usage: ", c + 1, " [-h,--help] ");


        for(auto& as : args) {
            bool req = as.item == ITEM::REQUIRED;
            printany(req ? " " : " [", as.shortarg);
            if(as.longarg[0])
                printany(',');
            printany(as.longarg, req ? " " : "] ");
            std::size_t len = Traits::length(as.longarg);
            if(len > max_len)
                max_len = len;
        }
        printany("\n\n");

        if(desc)
            printany("\n\nDescription: \n", leftpadstr, desc, "\n\n");

        printany("Arguments:\n", "    -h, --help\n\n");
        for(auto& as : args) {
            printany("    ", as.shortarg);
            if(as.longarg[0])
                printany(", ", as.longarg);
            std::cout << std::left << std::setw(max_len + rightpad - Traits::length(as.longarg)) << "";
            printany(as.type_string(), as.helpstr, "\n");
        }
    }

    void parse() {
        bool _ = true;

        argstruct help_arg(_TXT_("-h", CharT), _TXT_("--help", CharT), &_, _TXT_("this help", CharT));

        for(CharT** v = argv + 1, **end = argv + argc; v != end; ++v) {
            CharT* a = *v;
            if(*a != '-') {
                positional_argv.emplace_back(a);
                continue;
            }
            if(help_arg == a) {
                print_help();
                exit(EXIT_FAILURE);
            }

            int done = 0;
            auto r = Traits::find(a, Traits::length(a), '=');
            std::size_t idx = r ? r - a : 0;
            for(auto& s : args) {

                /* parse of long option */
                if(a[1] == '-') {
                    if(idx)
                        a[idx] = 0;
                    if(s == a) {
                        ++done;
                        if(s.type == typeid(bool))
                            s.set();
                        else if(idx)
                            s.set(&(a[idx + 1]));
                        else if(*(v + 1)[0] == '-')
                            abort("Error: Nothing Argument of `", *v, "`.\n\n");
                        else
                            s.set(*++v);
                    }
                    continue;
                }

                /* parse of short option */
                CharT* c = a;
                while(*++c) {
                    CharT short_flag[3] = {'-', *c, 0};
                    if(s == short_flag) {
                        ++done;
                        if(s.type == typeid(bool)) {
                            s.set();
                            continue;
                        }
                        unescape(++c);
                        if(!*c || (*c == '=' && !*++c))
                            abort("Error: Nothing Argument of `", *v, "`.\n\n");
                        s.set(c);
                        break;
                    }
                }
            }
            if(done == 0)
                abort("Error: `", *v, "` is Unknown Option.\n\n");
        }

        /* Check parsed Require options */
        for(auto& s : args) {
            if(s.item == ITEM::REQUIRED && s.parsed == 0)
                abort("Error: `", s.shortarg, "`is Require Argument.\n\n");
        }
    }

   private:
    template <typename Char0>
    void abort(const Char0 msg) {
        printerr(msg);
        print_help();
        exit(EXIT_FAILURE);
    }
    template <typename Char0, typename Char1>
    void abort(const Char0 s0, const Char1 s1) {
        printerr(s0);
        abort(s1);
    }
    template <typename Char0, typename Char1, typename Char2>
    void abort(const Char0 s0, const Char1 s1, const Char2 s2) {
        printerr(s0, s1);
        abort(s2);
    }
};

template <typename CharT>
argparser<CharT> ArgParser(int org_argc, CharT** org_argv) {
    return {org_argc, org_argv};
}

template <typename CharT>
argparser<CharT> ArgParser(const CharT* _desc, int org_argc, CharT** org_argv) {
    return {org_argc, org_argv, _desc};
}

#endif /* _ARGPARSE_HPP_ */
