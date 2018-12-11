#include "../src/argparse.h"
#include <cstdint>
#include <gtest/gtest.h>

template <typename T>
struct ArgparseAllTypes : public ::testing::Test {};

template <typename T>
struct ArgparseAllArithmeticTypes : public ::testing::Test {};

template <typename T>
struct ArgparseSignedIntegers : public ::testing::Test {};

using AllTypes = ::testing::Types<uint64_t, uint32_t, int64_t, int32_t, double, float, std::string>;
using AllArithmeticTypes = ::testing::Types<uint64_t, uint32_t, int64_t, int32_t, double, float>;
using SignedIntegers = ::testing::Types<int64_t, int32_t>;

TYPED_TEST_CASE(ArgparseAllTypes, AllTypes);
TYPED_TEST_CASE(ArgparseAllArithmeticTypes, AllArithmeticTypes);
TYPED_TEST_CASE(ArgparseSignedIntegers, SignedIntegers);

template <typename T>
std::vector<std::pair<std::string, T>> GetValidTokens();

template <>
std::vector<std::pair<std::string, uint64_t>> GetValidTokens() {
    return {{"123", 123}, {"45", 45}, {"0x12", 0x12}, {"0xFFFFFFFFFFFFFFFF", 0xFFFFFFFFFFFFFFFF}};
}

template <>
std::vector<std::pair<std::string, uint32_t>> GetValidTokens() {
    return {{"123", 123}, {"45", 45}, {"0x12", 0x12}, {"0xFFFFFFFF", 0xFFFFFFFF}};
}

template <>
std::vector<std::pair<std::string, int64_t>> GetValidTokens() {
    return {{"123", 123},
            {"45", 45},
            {"0x12", 0x12},
            {"-123", -123},
            {"-0x12", -0x12},
            {"0x7FFFFFFFFFFFFFFF", 0x7FFFFFFFFFFFFFFF},
            {"-0x8000000000000000", -0x8000000000000000}};
}

template <>
std::vector<std::pair<std::string, int32_t>> GetValidTokens() {
    return {{"123", 123},
            {"45", 45},
            {"0x12", 0x12},
            {"-123", -123},
            {"-0x12", -0x12},
            {"0x7FFFFFFF", 0x7FFFFFFF},
            {"-0x80000000", -0x80000000}};
}

template <>
std::vector<std::pair<std::string, double>> GetValidTokens() {
    return {{"1.23", 1.23}, {"4.0e5", 4.0e5}, {"1.0e308", 1.0e308}};
}

template <>
std::vector<std::pair<std::string, float>> GetValidTokens() {
    return {{"1.23", 1.23f}, {"4.0e5", 4.0e5f}, {"1.0e38", 1.0e38f}};
}

template <>
std::vector<std::pair<std::string, std::string>> GetValidTokens() {
    return {{"foo", "foo"}, {"bar", "bar"}};
}

template <typename T>
std::string GetOverflowToken();

template <>
std::string GetOverflowToken<uint64_t>() {
    return "0x10000000000000000";
}

template <>
std::string GetOverflowToken<uint32_t>() {
    return "0x100000000";
}

template <>
std::string GetOverflowToken<int64_t>() {
    return "0x8000000000000000";
}

template <>
std::string GetOverflowToken<int32_t>() {
    return "0x80000000";
}

template <>
std::string GetOverflowToken<double>() {
    return "1.0e309";
}

template <>
std::string GetOverflowToken<float>() {
    return "1.0e39";
}

template <typename T>
std::string GetUnderflowToken();

template <>
std::string GetUnderflowToken<int64_t>() {
    return "-0x8000000000000001";
}

template <>
std::string GetUnderflowToken<int32_t>() {
    return "-0x80000001";
}

TYPED_TEST(ArgparseAllTypes, HandlesValidTokens) {
    for (const auto& pair : GetValidTokens<TypeParam>()) {
        TypeParam x;
        EXPECT_TRUE(argparse::ParseToken(pair.first, &x).empty()) << pair.first;
        EXPECT_EQ(pair.second, x);
    }
}

TYPED_TEST(ArgparseAllArithmeticTypes, HandlesGarbage) {
    TypeParam x;
    const std::vector<std::string> garbageTokens = {"a12", "45a", "45 "};
    for (const std::string& token : garbageTokens) {
        EXPECT_FALSE(argparse::ParseToken(token, &x).empty());
    }
}

TYPED_TEST(ArgparseAllArithmeticTypes, HandlesOverflow) {
    TypeParam x;
    EXPECT_FALSE(argparse::ParseToken(GetOverflowToken<TypeParam>(), &x).empty());
}

TYPED_TEST(ArgparseSignedIntegers, HandlesUnderflow) {
    TypeParam x;
    EXPECT_FALSE(argparse::ParseToken(GetUnderflowToken<TypeParam>(), &x).empty());
}

TEST(Argparse, Argparse) {
    struct Arguments {
        int intField;
        std::string stringField;
    };

    const std::vector<const char*> tokens = {"program", "--foo", "1"};

    Arguments arguments;
    std::string errorMessage = argparse::Parse(
            {argparse::Option("foo", "description", &Arguments::intField),
             argparse::Positional("description", &Arguments::stringField)},
            tokens.size(),
            tokens.data(),
            &arguments);
}
