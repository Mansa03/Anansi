//
// Created by winig on 8/4/2026.
//

#ifndef ANANSI_ANANSITYPES_H
#define ANANSI_ANANSITYPES_H
#include <string_view>
#include "type_traits"
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>
#include <optional>
#include <string>
#include <stdexcept>
#include <variant>
template<typename T>
concept validKey = std::is_convertible_v<T, std::string_view> || std::is_arithmetic_v<T>;

template<typename V>
concept validValue = std::is_arithmetic_v<V> || std::is_convertible_v<V, std::string_view> || std::is_array_v<V>;

enum TypeTags {
    STRING = 0x03,
    UINT8 = 0x04,
    UINT16 = 0x05,
    UINT32 = 0x06,
    UINT64 = 0x07,
    INT8 = 0x08,
    INT16 = 0x09,
    INT32 = 0x0a,
    INT64 = 0x0b,
    FLOAT = 0x0c,
    DOUBLE = 0x0d,
    BOOL = 0x0e,
    ARRAY = 0x0f,
    NULL_VALUE = 0x10,
};
template<typename T>
struct TagOf;
template<> struct TagOf<std::string_view> {static constexpr TypeTags value = STRING;};
template<> struct TagOf<uint8_t> {static constexpr TypeTags value = UINT8;};
template<> struct TagOf<uint16_t> {static constexpr TypeTags value = UINT16;};
template<> struct TagOf<uint32_t> {static constexpr TypeTags value = UINT32;};
template<> struct TagOf<uint64_t> {static constexpr TypeTags value = UINT64;};
template<> struct TagOf<int8_t> {static constexpr TypeTags value = INT8;};
template<> struct TagOf<int16_t> {static constexpr TypeTags value = INT16;};
template<> struct TagOf<int32_t> {static constexpr TypeTags value = INT32;};
template<> struct TagOf<int64_t> {static constexpr TypeTags value = INT64;};
template<> struct TagOf<float> {static constexpr TypeTags value = FLOAT;};
template<> struct TagOf<double> {static constexpr TypeTags value = DOUBLE;};
template<> struct TagOf<bool> {static constexpr TypeTags value = BOOL;};
template<> struct TagOf<std::vector<uint8_t>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<uint16_t>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<uint32_t>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<uint64_t>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<int8_t>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<int16_t>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<int32_t>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<int64_t>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<float>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<double>> {static constexpr TypeTags value = ARRAY;};
template<> struct TagOf<std::vector<bool>> {static constexpr TypeTags value = ARRAY;};

struct column {
    std::string columnName;
    TypeTags type;
};
struct KeyValuePair : column  {
    std::byte *value{};
    std::optional<int> length;
    KeyValuePair() = default;
    KeyValuePair(std::string field, const TypeTags tag, std::byte *value, int length): value{value}, length {length} {
        columnName = std::move(field);
        type = tag;
    }
 };
struct AnansiData {
    KeyValuePair primaryKeyAndValue;
    KeyValuePair secondaryKeyAndValue;
    std::vector<KeyValuePair> fields;
};

enum CompactionLevel {
    LOOM,
    WEAVE,
    SNARE,
    VAULT,
    ARCHIVE,
};

constexpr std::string getCompactionLevelName(const CompactionLevel level) {
    switch(level) {
        case LOOM:
            return "loom";
            break;
        case WEAVE:
            return "weave";
            break;
        case SNARE:
            return "snare";
            break;
        case VAULT:
            return "vault";
            break;
        case ARCHIVE:
            return "archive";
            break;
        default:
            return "loom";
            break;
    }
}

inline int bytesNeeded(const TypeTags tag) {
    switch (tag) {
        case UINT8:
        case INT8:
        case BOOL:
            return 1;
        case UINT16:
        case INT16:
            return 2;
        case UINT32:
        case INT32:
        case FLOAT:
            return 4;
        case UINT64:
        case INT64:
        case DOUBLE:
            return 8;
        default:
            return 0;
    }
}

using AnansiValue = std::variant<uint8_t, int8_t, bool, uint16_t, int16_t,
    uint32_t, int32_t, float, uint64_t, int64_t, double,
    std::string>;

// Fixed-width types: read exactly sizeof(T) bytes via memcpy.
// memcpy (not reinterpret_cast) avoids strict-aliasing UB and misaligned
// reads, since a std::byte* buffer has no guaranteed alignment for T.
template<typename T>
T readAs(const std::byte* value, int length) {
    if (length != static_cast<int>(sizeof(T))) {
        throw std::runtime_error("readValue: length mismatch");
    }
    T v;
    std::memcpy(&v, value, sizeof(T));
    return v;
}

// Variable-width: STRING. reinterpret_cast is fine here specifically
// because char/unsigned char/std::byte are exempt from strict-aliasing
// rules, and std::string's (ptr, length) ctor is binary-safe (handles
// embedded nulls correctly, no null-terminator assumption).
inline std::string readAsString(const std::byte* value, int length) {
    if (length < 0) {
        throw std::runtime_error("readValue: negative length for STRING");
    }
    return std::string{reinterpret_cast<const char*>(value), static_cast<size_t>(length)};
}

inline AnansiValue readValue(const TypeTags tag, const std::byte* value, int length) {
    switch (tag) {
        case UINT8:  return readAs<uint8_t>(value, length);
        case INT8:   return readAs<int8_t>(value, length);
        case BOOL:   return readAs<uint8_t>(value, length) != 0;
        case UINT16: return readAs<uint16_t>(value, length);
        case INT16:  return readAs<int16_t>(value, length);
        case UINT32: return readAs<uint32_t>(value, length);
        case INT32:  return readAs<int32_t>(value, length);
        case FLOAT:  return readAs<float>(value, length);
        case UINT64: return readAs<uint64_t>(value, length);
        case INT64:  return readAs<int64_t>(value, length);
        case DOUBLE: return readAs<double>(value, length);
        case STRING: return readAsString(value, length);
        default:
            throw std::runtime_error("readValue: unsupported/unknown tag");
    }
}

#endif //ANANSI_ANANSITYPES_H
