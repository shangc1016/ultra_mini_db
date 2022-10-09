#include "../include/storageengine.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <sys/types.h>
#include <city.h>
#include <iostream>
#include <vector>


namespace minikv {




StorageEngine::StorageEngine(){
    this->_databaseFile = "minikv.db";
    this->OpenDBFile();
    this->InitBucket();
}

StorageEngine::~StorageEngine(){
    this->_openedFile.close();
    
    // std::cout << "engine decostructor" << std::endl;
}

Status StorageEngine::OpenDBFile(){
    Status status(STATUS_OKAY, "");

    std::ios::openmode mask = std::ios::in | std::ios::out | std::ios::app | std::ios::binary;
    this->_openedFile.open(this->_databaseFile, mask);
    return status;
}


Status StorageEngine::InitBucket(){
    Status status(STATUS_OKAY, "");

    this->_bucket = new Bucket(0x1000, 0x1000);

    // 分配BucketItem的内存，
    this->_bucket->_bucketSlot = new std::vector<BucketSlot*>();
    for(uint64_t i =0; i < this->_bucket->_bucketSlotNum; i++){
        this->_bucket->_bucketSlot->push_back(new BucketSlot());
    }

    return status;
}



uint64_t StorageEngine::Hash64(const std::string &str){
    return CityHash64(str.c_str(), str.size());
}

uint64_t StorageEngine::Hash32(const std::string &str){
    return CityHash32(str.c_str(), str.size());
}

uint32_t StorageEngine::KeyBucketIndex(const std::string &key){
    return this->Hash64(key) % this->_bucket->_bucketSlotNum;
}


Status StorageEngine::Get(const std::string key, std::string* valuePlaceHolder){
    Status status(STATUS_OKAY, "");

    if(!this->_openedFile.is_open()){
        status._stateCode = STATUS_FILE_NOTOPEN;
        return status;
    }
    // 调用哈希函数
    uint32 keyIndex = this->KeyBucketIndex(key);

    BucketSlot *slot = this->_bucket->_bucketSlot->at(keyIndex);
   
    slot->_lock.lock();
    status = SearchBST(slot->_record, key, valuePlaceHolder);
    slot->_lock.unlock();

    // std::cout << "[Get]: slotIndex = " << keyIndex << std::endl;
    return status;
}

Status StorageEngine::Put(const std::string key, const std::string value){
    Status status(STATUS_OKAY, "");

    if(!this->_openedFile.is_open()){
        status._stateCode = STATUS_FILE_NOTOPEN;
        return status;
    }

    // 调用哈希函数
    uint32_t keyIndex = this->KeyBucketIndex(key);

    BucketSlot *slot = this->_bucket->_bucketSlot->at(keyIndex);
    slot->_lock.lock();
    slot->_record = InsertBST(slot->_record, key, value);
    slot->_lock.unlock();

    // std::cout << "[Put]: slotIndex = " << keyIndex << std::endl;
    return status;
}




Status StorageEngine::SearchBST(Record *root, const std::string &key, std::string *value){
    Status status(STATUS_OKAY, "");

    if(!root){
        status._stateCode = STATUS_EMPTY_PTR;
        return status;
    }
    if(root->_key.compare(key) == 0) {
        *value = root->_value;
        return status;
    } else if(root->_key.compare(key) > 0){
        return SearchBST(root->_right, key, value);
    } else {
        return SearchBST(root->_left, key, value);
    }
}

 
Record* StorageEngine::InsertBST(Record *root, const std::string &key, const std::string &value){
    if(!root){
        root = new Record(key, value, 0);
        return root;
    }
    if(root->_key >= key) {
        return InsertBST(root->_left, key, value);
    } else {
        return InsertBST(root->_right, key, value);
    }
    return root;
}


BucketSlot* StorageEngine::GetBucketslot(uint32_t slotIndex){
    return this->_bucket->_bucketSlot->at(slotIndex);
}


Status StorageEngine::TraverseBST(Record* record){
    std::cout << "traval" << std::endl;
    Status status(STATUS_OKAY, "");
    if(!record) {
        status._stateCode = STATUS_EMPTY_PTR;
        return status;
    }
    TraverseBST(record->_left);
    std::cout << "[key] = " << record->_key << "\t;[value] = " << record->_value << std::endl;
    TraverseBST(record->_right);
    return status;
}

}