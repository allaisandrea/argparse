#include <cstdlib>
#include <errno.h>
#include <limits.h>
#include <memory>
#include <string>
#include <vector>

namespace argparse {

template <typename T>
const char* GetTypeName();

template <>
const char* GetTypeName<unsigned long long>() {
    return "unsigned long long";
}

template <>
const char* GetTypeName<unsigned long>() {
    return "unsigned long";
}

template <>
const char* GetTypeName<unsigned int>() {
    return "unsigned int";
}

template <>
const char* GetTypeName<long long>() {
    return "long long";
}

template <>
const char* GetTypeName<long>() {
    return "long";
}

template <>
const char* GetTypeName<int>() {
    return "int";
}

template <>
const char* GetTypeName<float>() {
    return "float";
}

template <>
const char* GetTypeName<double>() {
    return "double";
}

template <>
const char* GetTypeName<std::string>() {
    return "std::string";
}

template <typename T>
std::string OutOfRangeMessage(const std::string& token) {
    return "Token \"" + token + "\" is out of range for type \"" + GetTypeName<T>() + "\"";
}

template <typename T>
std::string CannotParseMssage(const std::string& token) {
    return "Token \"" + token + "\" cannot be parsed as type \"" + GetTypeName<T>() + "\"";
}

template <typename T>
T strto(const char* str, char** endptr);

template <>
long strto<long>(const char* str, char** strend) {
    return std::strtol(str, strend, 0);
}

template <>
unsigned long strto<unsigned long>(const char* str, char** strend) {
    return std::strtoul(str, strend, 0);
}

template <>
unsigned int strto<unsigned int>(const char* str, char** strend) {
    const unsigned long long x = std::strtoull(str, strend, 0);
    if (x > UINT_MAX) {
        errno = ERANGE;
    }
    return x;
}

template <>
long long strto<long long>(const char* str, char** strend) {
    return std::strtoll(str, strend, 0);
}

template <>
unsigned long long strto<unsigned long long>(const char* str, char** strend) {
    return std::strtoull(str, strend, 0);
}

template <>
int strto<int>(const char* str, char** strend) {
    const long long x = std::strtoll(str, strend, 0);
    if (x > INT_MAX || x < INT_MIN) {
        errno = ERANGE;
    }
    return x;
}

template <>
double strto<double>(const char* str, char** strend) {
    return std::strtod(str, strend);
}

template <>
float strto<float>(const char* str, char** strend) {
    return std::strtof(str, strend);
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
ParseToken(const std::string& token, T* x) {
    char* parsedEnd;
    errno = 0;
    *x = strto<T>(token.c_str(), &parsedEnd);
    if (errno == ERANGE) {
        errno = 0;
        return OutOfRangeMessage<T>(token);
    }
    if (std::distance<const char*>(token.data(), parsedEnd) != int64_t(token.size())) {
        return CannotParseMssage<T>(token);
    }
    return "";
}

inline std::string ParseToken(const std::string& token, std::string* x) {
    *x = token;
    return "";
}

template <typename Arguments>
class GenericArgument {
  public:
    virtual std::string
    parse(const std::vector<std::string>& tokens,
          std::vector<std::string>* tokenHasBeenParsedBy) = 0;
    virtual ~GenericArgument() = default;
};

template <typename Arguments, typename T>
class TypedOption : public GenericArgument<Arguments> {
  public:
    TypedOption(const std::string& name, const std::string& description, T Arguments::*offset) {}
    TypedOption(
            const std::string& name,
            const std::string& description,
            T Arguments::*offset,
            const T& defaultValue) {}
    std::string
    parse(const std::vector<std::string>& tokens, std::vector<std::string>* tokenHasBeenParsedBy) {
        return "";
    }
};

template <typename Arguments, typename T>
class TypedPositionalArgument : public GenericArgument<Arguments> {
  public:
    TypedPositionalArgument(const std::string& description, T Arguments::*offset) {}
    TypedPositionalArgument(
            const std::string& description,
            T Arguments::*offset,
            const T& defaultValue) {}
    std::string
    parse(const std::vector<std::string>& tokens, std::vector<std::string>* tokenHasBeenParsedBy) {
        return "";
    }
};

template <typename Arguments, typename T>
std::shared_ptr<TypedOption<Arguments, T>>
Option(const std::string& name, const std::string& description, T Arguments::*offset) {
    return std::make_shared<TypedOption<Arguments, T>>(name, description, offset);
}

template <typename Arguments, typename T>
std::shared_ptr<TypedOption<Arguments, T>>
Option(const std::string& name,
       const std::string& description,
       T Arguments::*offset,
       const T& defaultValue) {
    return std::make_shared<TypedOption<Arguments, T>>(name, description, offset, defaultValue);
}

template <typename Arguments, typename T>
std::shared_ptr<TypedPositionalArgument<Arguments, T>>
Positional(const std::string& description, T Arguments::*offset) {
    return std::make_shared<TypedPositionalArgument<Arguments, T>>(description, offset);
}

template <typename Arguments, typename T>
std::shared_ptr<TypedPositionalArgument<Arguments, T>>
Positional(const std::string& description, T Arguments::*offset, const T& defaultValue) {
    return std::make_shared<TypedPositionalArgument<Arguments, T>>(
            description, offset, defaultValue);
}

template <typename Arguments>
inline std::string
Parse(const std::vector<std::shared_ptr<GenericArgument<Arguments>>>& options,
      int argc,
      char const* const* const argv,
      Arguments* arguments) {
    return "";
}
} // namespace argparse
