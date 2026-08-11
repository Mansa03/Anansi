//
// Created by winig on 8/7/2026.
//

#ifndef ANANSI_SSHANDLER_H
#define ANANSI_SSHANDLER_H
#include <string>
#include <vector>
#include "../types/AnansiTypes.h"
#include "../models/tables.h"
inline std::string CATALOG_FOLDER = "catalog";
inline std::string TABLES_FOLDER = "tables";
inline std::string METADATA_FOLDER = "metadata";
inline std::string SEGMENTS_FOLDER = "segments";
inline std::string TABLE_CATALOG = "tableCatalog.ans";
inline std::string SEGMENT = "segment";
struct SSHandler {
    std::string dbPath;
    explicit SSHandler(std::string dbPath);
    ~SSHandler();
    void createTable(const Table& tableContext) const;
    void deleteTable(const std::string& tableName) const;
    void flush(const std::string& tableName, std::vector<AnansiData> records, AnsContext ctx) const;
};


#endif //ANANSI_SSHANDLER_H
