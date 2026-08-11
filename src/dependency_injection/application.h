//
// Created by winig on 8/7/2026.
//

#ifndef ANANSI_APPLICATION_H
#define ANANSI_APPLICATION_H
#include <string>

std::string constexpr DATABASE_PATH = "volume";
std::string constexpr DATABASE_NAME = "database";

inline std::string getDatabasePath() {
    char* path = std::getenv("DATABASE_PATH");
    return path==nullptr?DATABASE_PATH:path;
}
#endif //ANANSI_APPLICATION_H
