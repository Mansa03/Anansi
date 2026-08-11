//
// Created by winig on 8/9/2026.
//

#ifndef ANANSI_TESTHELPER_H
#define ANANSI_TESTHELPER_H
#include "../src/types/AnansiTypes.h"   // for TypeTags, KeyValuePair, AnansiData
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

// ---------------------------------------------------------------------------
// Helpers to build a KeyValuePair from a typed value or a raw byte blob.
// ---------------------------------------------------------------------------

template <typename T>
KeyValuePair makeField(std::string fieldName, TypeTags tag, T value) {
    auto* buf = new std::byte[sizeof(T)];
    std::memcpy(buf, &value, sizeof(T));
    return KeyValuePair{std::move(fieldName), tag, buf, static_cast<int>(sizeof(T))};
}

// For string-like / variable-length data, e.g. call with TypeTags::STRING.
// Also works generically for any variable-length raw byte blob if you pass
// a different tag.
inline KeyValuePair makeBytesField(std::string fieldName, TypeTags tag, const std::string& raw) {
    auto* buf = new std::byte[raw.size()];
    std::memcpy(buf, raw.data(), raw.size());
    return KeyValuePair{std::move(fieldName), tag, buf, static_cast<int>(raw.size())};
}

// ---------------------------------------------------------------------------
// Cleanup helpers (KeyValuePair doesn't own its buffer via RAII, so free
// manually -- important since these are `new[]`-allocated in makeField/makeBytesField).
// ---------------------------------------------------------------------------

inline void freeKVP(KeyValuePair& kvp) {
    delete[] kvp.value;
    kvp.value = nullptr;
}

inline void freeAnansiData(AnansiData& row) {
    freeKVP(row.primaryKeyAndValue);
    freeKVP(row.secondaryKeyAndValue);
    for (auto& f : row.fields) freeKVP(f);
}

inline void freeAll(std::vector<AnansiData>& rows) {
    for (auto& row : rows) freeAnansiData(row);
}

// ---------------------------------------------------------------------------
// Test data: a small "users" table.
//   primary key   -> userId   (UINT64)
//   secondary key -> signupTs (UINT32, unix timestamp)
//   fields        -> name (raw bytes), age (UINT8), active (BOOL),
//                     rating (DOUBLE), loginCount (INT32)
// ---------------------------------------------------------------------------

inline std::vector<AnansiData> generateTestData() {
    std::vector<AnansiData> rows;

    // --- Row 1 ---
    {
        AnansiData row;
        row.primaryKeyAndValue   = makeField<uint64_t>("userId", TypeTags::UINT64, 1001);
        row.secondaryKeyAndValue = makeField<uint32_t>("signupTs", TypeTags::UINT32, 1700000000);
        row.fields.push_back(makeBytesField("name", TypeTags::STRING, "Alice Chen"));
        row.fields.push_back(makeField<uint8_t>("age", TypeTags::UINT8, 29));
        row.fields.push_back(makeField<bool>("active", TypeTags::BOOL, true));
        row.fields.push_back(makeField<double>("rating", TypeTags::DOUBLE, 4.87));
        row.fields.push_back(makeField<int32_t>("loginCount", TypeTags::INT32, 342));
        rows.push_back(row);
    }

    // --- Row 2 ---
    {
        AnansiData row;
        row.primaryKeyAndValue   = makeField<uint64_t>("userId", TypeTags::UINT64, 1002);
        row.secondaryKeyAndValue = makeField<uint32_t>("signupTs", TypeTags::UINT32, 1701234567);
        row.fields.push_back(makeBytesField("name", TypeTags::STRING, "Marcus Reid"));
        row.fields.push_back(makeField<uint8_t>("age", TypeTags::UINT8, 41));
        row.fields.push_back(makeField<bool>("active", TypeTags::BOOL, false));
        row.fields.push_back(makeField<double>("rating", TypeTags::DOUBLE, 3.15));
        row.fields.push_back(makeField<int32_t>("loginCount", TypeTags::INT32, 12));
        rows.push_back(row);
    }

    // --- Row 3: edge-case values ---
    {
        AnansiData row;
        row.primaryKeyAndValue   = makeField<uint64_t>("userId", TypeTags::UINT64, 0);           // min id
        row.secondaryKeyAndValue = makeField<uint32_t>("signupTs", TypeTags::UINT32, 0xFFFFFFFF); // max u32
        row.fields.push_back(makeBytesField("name", TypeTags::STRING, ""));                        // empty string
        row.fields.push_back(makeField<uint8_t>("age", TypeTags::UINT8, 0));
        row.fields.push_back(makeField<bool>("active", TypeTags::BOOL, true));
        row.fields.push_back(makeField<double>("rating", TypeTags::DOUBLE, -0.0));                // signed zero
        row.fields.push_back(makeField<int32_t>("loginCount", TypeTags::INT32, -1));              // negative
        rows.push_back(row);
    }

    // --- Row 4: large / boundary numeric values ---
    {
        AnansiData row;
        row.primaryKeyAndValue   = makeField<uint64_t>("userId", TypeTags::UINT64, UINT64_MAX);
        row.secondaryKeyAndValue = makeField<uint32_t>("signupTs", TypeTags::UINT32, 1712345678);
        row.fields.push_back(makeBytesField("name", TypeTags::STRING, "Zoe \xE2\x9C\xA8 Nakamura")); // utf-8 bytes
        row.fields.push_back(makeField<uint8_t>("age", TypeTags::UINT8, 255));
        row.fields.push_back(makeField<bool>("active", TypeTags::BOOL, true));
        row.fields.push_back(makeField<double>("rating", TypeTags::DOUBLE, 5.0));
        row.fields.push_back(makeField<int32_t>("loginCount", TypeTags::INT32, INT32_MAX));
        rows.push_back(row);
    }

    return rows;
}

// ---------------------------------------------------------------------------
// Minimal printer, useful for sanity-checking generated data.
// ---------------------------------------------------------------------------

inline void printKVP(const KeyValuePair& kvp) {
    std::cout << "  " << kvp.columnName << " (tag=" << static_cast<int>(kvp.type)
              << ", len=" << (kvp.length ? *kvp.length : -1) << ") bytes: ";
    int n = kvp.length ? *kvp.length : 0;
    for (int i = 0; i < n; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(kvp.value[i]) << " ";
    }
    std::cout << std::dec << "\n";
}

inline void printKVPValue(const KeyValuePair& kvp) {
    std::cout << "  " << kvp.columnName << " (tag=" << static_cast<int>(kvp.type)
              << ", len=" << (kvp.length ? *kvp.length : -1) << ") bytes: ";
    int n = kvp.length ? *kvp.length : 0;

    for (int i = 0; i < n; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(kvp.value[i]) << " ";
    }
    auto value = readValue(kvp.type,kvp.value, n);
    std::visit([](const auto& v) {
            std::cout << "value: " << v << " ";
    } , value);
    std::cout << std::dec <<  "\n";
}

#endif //ANANSI_TESTHELPER_H
