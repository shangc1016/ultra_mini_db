#ifndef __STORAGEENGINE_H_
#define __STORAGEENGINE_H_

#include "status.h"
#include <city.h>
#include <cstdint>
#include <fstream>
#include <string>
#include <mutex>
#include <vector>

namespace minikv {


/*
从<hashed-kay:64, location:64>中得反向计算得到key的位置

*/



struct MetaData{
    uint64_t    _metaDataOffset;
    uint64_t    _metaDataLength;

    uint64_t    _bucketOffset;
    uint64_t    _bucketItemNums;


    std::string _dbName;
    std::string _dbPath;
};


// Record 每个记录，树
struct Record{
    std::string  _key;
    std::string  _value;
    Record      *_left;
    Record      *_right;
    uint64_t     _offset;
    // 增加一个字段  表示是否被删除
    bool         _deleted;

    Record(std::string k, std::string v, uint64_t off){
        _key     = k;
        _value   = v;
        _left    = NULL;
        _right   = NULL;
        _offset  = off;
        _deleted = false;
    }

};


struct BucketSlot{
    Record     *_record;
    std::mutex  _lock;
    // 增加一个记录条数信息
    uint64_t    _recordNums;

    BucketSlot(){
        _record     = nullptr;
        _recordNums = 0;
    }
};

// 哈希表,
struct Bucket{
    uint64_t                  _bucketOffset;
    uint64_t                  _bucketSlotNum;
    std::vector<BucketSlot*> *_bucketSlot;

    Bucket(uint64_t _offset, uint64_t _num){
        this->_bucketOffset   = _offset;
        this->_bucketSlotNum  = _num;
    }

};


class StorageEngine{

public:

    StorageEngine();
    ~StorageEngine();

    Status* OpenDBFile();

    Status* InitBucket();
    Status* InitRecord();


    uint64_t Hash64(const std::string&);
    uint64_t Hash32(const std::string&);


    Status* Get(const std::string, std::string*);
    Status* Put(const std::string, const std::string);
    Status* Delete(const std::string);


    uint32_t KeySlotIndex(const std::string &);


    BucketSlot *GetBucketSlot(uint32_t);
    Record* ReleaseSlotRecords(Record*);

    uint32_t SlotRecordNums(BucketSlot*);

    uint32_t BucketSlotNums();


    Status* SearchBST(Record*, const std::string, std::string *);
    Record* InsertBST(Record*, const std::string, const std::string&);
    void    TraverseBST(Record*);
    // 在树中，找到一条记录，先找到记录，然后修改
    Record* FindRecord(Record*, const std::string&);



    // TODO
    // 1. 析构函数，释放对象内存

    // 2. 持久化
    // 在持久化的时候，再计算文件offset
    Status* FlushBucketSlot(BucketSlot*);



    Status* CloseBucket();
    

    

private:

    // 文件
    std::string   _databaseFile;
    std::fstream  _openedFile;

    // 哈希表
    Bucket       *_bucket;


};





}


#endif