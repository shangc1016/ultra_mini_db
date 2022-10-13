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
#include <cstdio>


namespace minikv {




StorageEngine::StorageEngine(){
    this->_databaseFile = "minikv.db";
    Status *status;
    status = this->OpenDBFile();
    if(status->_stateCode != STATUS_OKAY){
        status->Print();
        delete status;
        exit(EXIT_FAILURE);
    }
    // 这个是OpenDBFile中返回的status对象
    delete status;

    status = this->InitBucket();
    if(status->_stateCode != STATUS_OKAY){
        status->Print();
        delete status;
        exit(EXIT_FAILURE);
    }
    // 这个是InitBucket中返回的status对象
    delete status;
}

StorageEngine::~StorageEngine(){

    Status *status;

    this->_openedFile.close();
    status = this->CloseBucket();
    if(status->_stateCode != STATUS_OKAY){
        status->Print();
    }
    delete status;
}



Status* StorageEngine::CloseBucket(){

    for(auto item : *this->_bucket->_bucketSlot){
        ReleaseSlotRecords(item->_record);
        delete item;
    }

    // clear只删除元素， 不真正释放内存
    this->_bucket->_bucketSlot->clear();
    // 缩小内存(capacity)到size
    this->_bucket->_bucketSlot->shrink_to_fit();

    delete this->_bucket->_bucketSlot;
    delete this->_bucket;
    return new Status(STATUS_OKAY, "release [vector] memory");
}

Status* StorageEngine::OpenDBFile(){
    Status *status = new Status(STATUS_OKAY, "");
    std::ios::openmode mask = std::ios::in | std::ios::out | std::ios::app | std::ios::binary;
    this->_openedFile.open(this->_databaseFile, mask);
    return status;
}


Status* StorageEngine::InitBucket(){

    // 一个bucket array的长度是10
    this->_bucket = new Bucket(0x1000, 0x010000);

    // 分配BucketItem的内存，
    this->_bucket->_bucketSlot = new std::vector<BucketSlot*>();
    for(uint64_t i =0; i < this->_bucket->_bucketSlotNum; i++){
        this->_bucket->_bucketSlot->push_back(new BucketSlot());
    }

    return new Status(STATUS_OKAY, "");
}



uint64_t StorageEngine::Hash64(const std::string &str){
    return CityHash64(str.c_str(), str.size());
}

uint64_t StorageEngine::Hash32(const std::string &str){
    return CityHash32(str.c_str(), str.size());
}

uint32_t StorageEngine::KeySlotIndex(const std::string &key){
    return this->Hash64(key) % this->_bucket->_bucketSlotNum;
}


Status* StorageEngine::Get(const std::string key, std::string* valuePlaceHolder){
    
    Status *status;

    if(!this->_openedFile.is_open()){
        return new Status(STATUS_FILE_NOT_OPEN, "fil3e not open");
    }
    // 调用哈希函数
    uint32 keyIndex = this->KeySlotIndex(key);

    BucketSlot *slot = this->_bucket->_bucketSlot->at(keyIndex);
   
    slot->_lock.lock();
    // printf("[GET][key]:%s, [keyIndex]:%d\n", key.c_str(), keyIndex);
    status = SearchBST(slot->_record, key, valuePlaceHolder);
    // 有可能，指针是空的，返回错误
    if(status->_stateCode == STATUS_EMPTY_PTR){
        printf("[Empty_Ptr]:-----------------key:%s;\tkeyIndex;%d\n", key.c_str() , keyIndex);
    }
    slot->_lock.unlock();

    // std::cout << "[Get]: slotIndex = " << keyIndex << std::endl;
    return status;
}

Status* StorageEngine::Put(const std::string key, const std::string value){

    if(!this->_openedFile.is_open()){
        return new Status(STATUS_FILE_NOT_OPEN, "file not open");
    }

    // 调用哈希函数
    uint32_t keyIndex = this->KeySlotIndex(key);

    BucketSlot *slot = this->_bucket->_bucketSlot->at(keyIndex);
    slot->_lock.lock();
    // printf("[PUT][key]:%s, [keyIndex]:%d\n", key.c_str(), keyIndex); 
    slot->_record = InsertBST(slot->_record, key, value);
    // slot的记录条数增加
    slot->_recordNums += 1;
    slot->_lock.unlock();


    return new Status(STATUS_OKAY, "");
}


Status* StorageEngine::Delete(const std::string key){
    if(!this->_openedFile.is_open()){
        return new Status(STATUS_FILE_NOT_OPEN, "[Delete]: file not open");
    }
    // 得到key经过哈希之后，在bucket中的索引
    uint32_t keyIndex = this->KeySlotIndex(key);
    BucketSlot *slot = this->_bucket->_bucketSlot->at(keyIndex);
    slot->_lock.lock();
    // 设置删除字段
    Record *record = FindRecord(slot->_record, key);
    if(!record){
        return  new Status(STATUS_EMPTY_PTR, "[Delete]: record not exist");
    }
    record->_deleted = true;
    slot->_recordNums -= 1;
    slot->_lock.unlock();

    return new Status(STATUS_OKAY, "");
}

BucketSlot* StorageEngine::GetBucketSlot(uint32_t slotIndex){
    return this->_bucket->_bucketSlot->at(slotIndex);
}


Record* StorageEngine::ReleaseSlotRecords(Record *root){
    
    if(!root) return  nullptr;
    root->_left = ReleaseSlotRecords(root->_left);
    root->_right = ReleaseSlotRecords(root->_right);
    delete root;
    return nullptr;
}



uint32 StorageEngine::BucketSlotNums(){
    return this->_bucket->_bucketSlotNum;
}

uint32_t StorageEngine::SlotRecordNums(BucketSlot* slots){
    return slots->_recordNums;
}


Status* StorageEngine::SearchBST(Record *root, const std::string key, std::string *value){

    if(!root){
        return new Status(STATUS_EMPTY_PTR, "[searchBST]: key:" + key + " empty pointer");
    }
    if(root->_key.compare(key) == 0) {
        *value = root->_value;
        return new Status(STATUS_OKAY, "");
    } else if(root->_key.compare(key) > 0){
        return SearchBST(root->_left, key, value);
    } else {
        return SearchBST(root->_right, key, value);
    }
}

 
Record* StorageEngine::InsertBST(Record *root, const std::string key, const std::string &value){
    if(!root){
        root = new Record(key, value, 0);
        return root;
    }
    // put操作修改value对应下面
    if(root->_key.compare(key) == 0){
        root->_value = value;
        return root;
    } else if(root->_key.compare(key) > 0) {
        root->_left = InsertBST(root->_left, key, value);
    } else {
        root->_right = InsertBST(root->_right, key, value);
    }
    return root;
}



// 遍历record是不需要返回值的， 总会遇到nullptr，我傻了
void StorageEngine::TraverseBST(Record* record){
    
    if(!record) return;
    TraverseBST(record->_left);
    std::cout << "[key]:" << record->_key
              << ";\t[value]:" << record->_value 
              << ";\t[deleted]:" << record->_deleted << std::endl;
    TraverseBST(record->_right);
    new Status(STATUS_OKAY, "");
}


Record* StorageEngine::FindRecord(Record *root, const std::string &key){

    if(!root) return nullptr;
    if(root->_key.compare(key) == 0) {
        return root;
    } else if(root->_key.compare(key) > 0){
        return FindRecord(root->_left, key);
    } else {
        return FindRecord(root->_right, key);
    }
}









}