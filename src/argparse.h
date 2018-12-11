#include <cstdlib>
#include <errno.h>
#include <limits.h>
#include <memory>
#include <string>
#include <vector>

namespace argparse {

namespace internal {

template <typename T> const char *GetTypeName();

template <> const char *GetTypeName<unsigned long long>() { return "unsigned long long"; }

template <> const char *GetTypeName<unsigned long>() { return "unsigned long"; }

template <> const char *GetTypeName<unsigned int>() { return "unsigned int"; }

template <> const char *GetTypeName<long long>() { return "long long"; }

template <> const char *GetTypeName<long>() { return "long"; }

template <> const char *GetTypeName<int>() { return "int"; }

template <> const char *GetTypeName<float>() { return "float"; }

template <> const char *GetTypeName<double>() { return "double"; }

template <> const char *GetTypeName<std::string>() { return "std::string"; }

template <typename T> std::string OutOfRangeMessage(const std::string &token) {
    return "Token \"" + token + "\" is out of range for type \"" + GetTypeName<T>() + "\"";
}

template <typename T> std::string CannotParseMssage(const std::string &token) {
    return "Token \"" + token + "\" cannot be parsed as type \"" + GetTypeName<T>() + "\"";
}

template <typename T> T strto(const char *str, char **endptr);

template <> long strto<long>(const char *str, char **strend) { return std::strtol(str, strend, 0); }

template <> unsigned long strto<unsigned long>(const char *str, char **strend) {
    return std::strtoul(str, strend, 0);
}

template <> unsigned int strto<unsigned int>(const char *str, char **strend) {
    const unsigned long long x = std::strtoull(str, strend, 0);
    if (x > UINT_MAX) {
        errno = ERANGE;
    }
    return x;
}

template <> long long strto<long long>(const char *str, char **strend) {
    return std::strtoll(str, strend, 0);
}

template <> unsigned long long strto<unsigned long long>(const char *str, char **strend) {
    return std::strtoull(str, strend, 0);
}

template <> int strto<int>(const char *str, char **strend) {
    const long x = std::strtoull(str, strend, 0);
    if (x > INT_MAX || x < INT_MIN) {
        errno = ERANGE;
    }
    return x;
}

template <> double strto<double>(const char *str, char **strend) {
    return std::strtod(str, strend);
}

template <> float strto<float>(const char *str, char **strend) { return std::strtof(str, strend); }

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
ParseToken(const std::string &token, T *x) {
    char *parsedEnd;
    errno = 0;
    *x = strto<T>(token.c_str(), &parsedEnd);
    if (errno == ERANGE) {
        errno = 0;
        return OutOfRangeMessage<T>(token);
    }
    if (std::distance<const char *>(token.data(), parsedEnd) != int64_t(token.size())) {
        return CannotParseMssage<T>(token);
    }
    return "";
}

inline std::string ParseToken(const std::string &token, std::string *x) {
    *x = token;
    return "";
}

// template <typename T>
// std::string Parse(const std::string &name,
//                   const std::vector<std::string> &tokens,
//                   std::vector<std::string> *tokenHasBeenParsedBy, T *x) {
//     return "";
// }
//
// class OptionBase {
//   public:
//     virtual std::string
//     parse(const std::vector<std::string> &tokens,
//           std::vector<std::string> *tokenHasBeenParsedBy) = 0;
//     virtual ~OptionBase() = default;
// };
//
// template <typename T> class TypedOption : public OptionBase {
//   public:
//     TypedOption(const std::string &name, T *const field)
//         : _name(name), _field(field) {}
//     std::string parse(const std::vector<std::string> &tokens,
//                       std::vector<std::string> *tokenHasBeenParsedBy)
//                       override {
//         return Parse(_name, tokens, tokenHasBeenParsedBy, _field);
//     }
//
//   private:
//     const std::string &_name;
//     T *const _field;
// };

} // namespace internal

// template <typename T>
// std::shared_ptr<internal::TypedOption<T>> Option(const std::string &name,
//                                                  T *const field) {
//     return std::make_shared<internal::TypedOption<T>>(name, field);
// }
//
// inline bool
// Parse(const std::vector<std::shared_ptr<internal::OptionBase>> &options,
//       int argc, char const *const *const argv) {
//     return true;
// }
} // namespace argparse
