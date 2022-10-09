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


struct MetaData{
    std::string _dbName;
};


// Record 每个记录，树
struct Record{
    std::string  _key;
    std::string  _value;
    Record      *_left;
    Record      *_right;
    uint64_t     _offset;

    Record(std::string k, std::string v, uint64_t off){
        _key = k;
        _value = v;
        _offset = off;
    }
};


struct BucketSlot{
    Record     *_record;
    std::mutex  _lock;

    BucketSlot(){
        _record = nullptr;
    }
};


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

    Status OpenDBFile();

    Status InitBucket();
    Status InitRecord();


    uint64_t Hash64(const std::string&);
    uint64_t Hash32(const std::string&);


    Status Get(const std::string, std::string*);
    Status Put(const std::string, const std::string);


    uint32_t KeyBucketIndex(const std::string &);


    Record* GetRecordRoot(uint32);


    Status SearchBST(Record*, const std::string &, std::string *);
    Record* InsertBST(Record*, const std::string&, const std::string&);
    Status TraverseBST(uint32);

    BucketSlot *GetBucketslot(uint32_t);

    Status TraverseBST(Record*);

    // TODO
    // 1. 析构函数，释放对象内存
    // 2. 持久化
    
    // Status* 
    

    

private:

    // 文件
    std::string   _databaseFile;
    std::fstream  _openedFile;


    Bucket       *_bucket;


};





}


#endif