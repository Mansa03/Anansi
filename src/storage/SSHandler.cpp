//
// Created by winig on 8/7/2026.
//

#include "SSHandler.h"

#include <fcntl.h>
#include <fstream>
#include <string>
#include <iostream>
#include <io.h>
#include "../models/tables.h"
#include <cassert>
#include <iomanip>
#include <filesystem>
SSHandler::SSHandler(std::string dbPath):
dbPath(std::move(dbPath)){}
SSHandler::~SSHandler() = default;
int MAX_SEGMENT_SIZE = 4096;
int blockOffset = 1000;

void SSHandler::createTable(const Table& tableContext) const {
    assert(!tableContext.primaryKey.empty());
    assert(!tableContext.tableName.empty());
    auto path =  dbPath + "/" + CATALOG_FOLDER;
    auto tableFile = path + "/" + TABLE_CATALOG;
    std::fstream tableCatalogFile;
    tableCatalogFile.open(tableFile, std::ios::out | std::ios::ate | std::ios::binary | std::ios::app);
    std::vector<int> byteOffsets = std::vector<int>();
    if (tableCatalogFile.is_open()) {
        tableCatalogFile << std::hex << std::setw(2) << std::setfill('0') << TypeTags::STRING << " "  << std::setw(2) << tableContext.tableName.length() << " " << std::setw(2) << tableContext.tableName << " " << std::setw(2)
    << TypeTags::STRING << " " << std::setw(2)<< tableContext.primaryKey.length() << " "  << std::setw(2)<< tableContext.primaryKey << " "  << std::setw(2)<< tableContext.primaryKeyType << " "
     << std::setw(2) <<TypeTags::STRING << " "  << std::setw(2)<< tableContext.sortKey.value_or("NULL").length() << " "  << std::setw(2)<< tableContext.sortKey.value_or("NULL") << " " << std::setw(2) << tableContext.sortKeyType.value_or(TypeTags::NULL_VALUE) << "\n";
    tableCatalogFile.close();
    } else {
        std::cout << "SSHHandler::createTable: Cannot create table \"" << tableContext.tableName << " " << "error:" << std::strerror(errno) << "\n";
        std::cout << "Path: " << tableFile << "\n";
    }
}

void SSHandler::deleteTable(const std::string& tableName) const {}

void SSHandler::flush(const std::string& tableName, std::vector<AnansiData> records, AnsContext ctx) const {
    auto min = records.begin()->primaryKeyAndValue.value;
    auto max = records.end()->primaryKeyAndValue.value;
    auto path = dbPath + "/" + TABLES_FOLDER + "/" + tableName;
    auto segmentsPath= path + "/" + SEGMENTS_FOLDER + "/" + getCompactionLevelName(CompactionLevel::LOOM) + "/";
    auto metaDataPath = path + "/" + METADATA_FOLDER + "/" + getCompactionLevelName(CompactionLevel::LOOM) + "/";
    if (!std::filesystem::exists(segmentsPath)) {
        std::filesystem::create_directories(segmentsPath);
    }
    if (!std::filesystem::exists(metaDataPath)) {
        std::filesystem::create_directories(metaDataPath);
    }
    const std::filesystem::directory_iterator seg_itr{segmentsPath};
    std::vector<int> indexes = std::vector<int>();
    auto seg_vector = std::vector{seg_itr};
    int nextSegmentId = seg_vector.empty()? 0:stoi(seg_vector[seg_vector.size()-1]->path().stem().string().substr(SEGMENT.length()+2));
    auto curr_size = 0;
    auto segmentFile = segmentsPath + "/" + SEGMENT + "_" + std::to_string(nextSegmentId) + ".ans";
    auto segmentStream = std::ofstream(segmentFile);
    if (segmentStream.is_open()) {
        indexes.push_back(segmentStream.tellp());
        for (AnansiData record : records) {
            if (curr_size + sizeof(record) > MAX_SEGMENT_SIZE) {
                auto currpos = segmentStream.tellp();
                segmentStream.seekp(currpos + std::streampos{blockOffset});
                indexes.push_back(segmentStream.tellp());
            }

        }
    }
}

