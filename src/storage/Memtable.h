//
// Created by winig on 8/4/2026.
//

#ifndef ANANSI_MEMTABLE_H
#define ANANSI_MEMTABLE_H
#include <vector>

#include "RBTree.h"
#include "../types/AnansiTypes.h"
#include "C:/Users/winig/Documents/Personal_Projects/Anansi/src/storage/Nodes.h"


class Memtable{
    public:
    float memSize{};
    int size{};
    virtual ~Memtable() = default;
    virtual bool insert(AnansiData data)=0;
    virtual Node* find(std::byte *value)=0;
    virtual Node* remove(std::byte *value)=0;
};


#endif //ANANSI_MEMTABLE_H
