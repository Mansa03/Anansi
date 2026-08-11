//
// Created by winig on 8/10/2026.
//

#ifndef ANANSI_TABLES_H
#define ANANSI_TABLES_H
#include <map>
#include <string>
#include "../types/AnansiTypes.h"

struct Table {
    std::string tableName;
    std::string primaryKey;
    TypeTags primaryKeyType;
    std::optional<std::string> sortKey;
    std::optional<TypeTags> sortKeyType;
};

struct Metadata {
    std::string tableName;
    uint32_t segmentId;
    std::byte* min;
    std::byte* max;
    time_t segmentStartTime;
    time_t segmentEndTime;
    std::vector<uint32_t> indexByteOffsets;
    std::vector<bool> bloomFilter;
    std::vector<std::string> hashFunctions;
};

struct ColumnTagId {
    std::string columnName;
    uint32_t columnId;
};

struct AnsContext {
    std::string tableName;
    std::map<std::string, uint32_t> columnIds;
};
#endif //ANANSI_TABLES_H
