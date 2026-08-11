//
// Created by winig on 8/10/2026.
//
#include "gtest/gtest.h"
#include "../../src/storage/SSHandler.h"
#include <string>

inline std::string DBPath = "C:/Users/winig/Documents/Personal_Projects/Anansi/test/db";
inline Table TestTable = {"TestTable",
    "userId",
    TypeTags::INT8,
    std::optional<std::string>(),
    std::optional<TypeTags>()};
class SSHandlerTester : public testing::Test {
    public:
        SSHandler handler;
        SSHandlerTester() : handler{DBPath} {}
        ~SSHandlerTester() override = default;
};

TEST_F(SSHandlerTester, createTable) {
    handler.createTable(TestTable);
}