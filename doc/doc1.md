哈希计算的结果就是key的索引



kingdb的key、value使用bytearray，key最大2^32-1，value最大2^64-1、


kingdb作为网络存储，使用memcached的网络协议库。但是只支持get、put、delete


数据以日志存储的方式写到日志文件，写操作和删除操作写到日志中，

使用hstable，hash table，就是一个简单文件。文件末尾有一个偏移数组，包含了哈希之后的key以及相应记录的位置，方便索引。


kingdb的迭代器可以遍历所有的数据条目，因为使用的是哈希表的方式，因此迭代不是按照哈希表的keyDev顺序，而是数据被写入的顺序。



database::get()->getRaw()

1. ->wb::get()   // writerBUffer::get()  先检查wrtebufer
2. ->se::get()   // storageEngine::get() writeBuffer没有的话，再去检查storageEngine



​	分别从writeBuffer的im_live这个buffer以及im_copy这个buffe查询记录





database::put()->PutPart()

​	检查key、value的长度是否超长

- 不超长：PutPartValidSize
- 超长：经过处理，PutPartValidSize



PutPartValidSize:

1. 压缩
2. 校验
3. writeBuffer::putPart->writeBuffer::WritePart
   1. writeBuffer::WritePart插入到im_live这个buffer，

writeBuffer有一个vector作为buffer，分为两个vector，分别是im_live_和im_copy,



#### get


get()->
    1. writeBuffer->get()
    2. storageEngine->get()
       1. GetWithIndex()
          1. 先对key哈希，
          2. 在index索引multimap中查找，倒序的找hashed-key相同的，(找到第一个就停止),,,这样找到的key的数据就是最新的
          3. ::GetEntry()
             1. 根据index索引中得到的记录的位置(文件位置，文件偏移)
             2. 打开文件，mmap，
             3. 把文件偏移位置读到结构体，得到了各个数据的偏移，
             4. 判断是否被删除了
             5. 读出数据






先从writeBuffer中查找，找不到的话再去storageEngine查找
writeBuffer:
1. 首先查找im_live_这个缓存:db写入的缓存
2. 然后查找im_copy_这个缓存：flush持久化的缓存

如果在writeubffer中找不到，就去storageEngine中找，se就是文件已经持久化了
1. 先查找索引，找到key对应的文件，以及文件中的偏移
2. 然后读文件

#### put

直接写到writeBuffer

> put、delete直接把数据写到writeBUffer就完成了

#### delete

为了保证一致性，delete必须直接写到磁盘中



#### class::writeBuffer

有两个buffer，income buffer和flush buffer。插入的时候先插入到income buffer，income buffer满的时候和flush buffer交换。然后把flush buffer交给eventmanager，刷新到磁盘。



交换write_buffer和flush_buffer，std::swap



writeBUffer有一个bufferManager线程用来落盘。


#### class::HSTableManager

这个数据结构是把记录写到文件时候的数据结构




class:storageEngine、class::writeBuffer共用一个eventmanager，实现线程之间的通知协作机制



#### 索引
使用了<hashed-key:64bit, location:64bit>这样的索引方式，快速定位记录的位置,
hashed-key长度64bit,location64bit，前32位记录哪个文件，后32位记录文件中的偏移.


索引使用多读单写的方式，保证一致性。索引致同股哟storageEngine更新，所以索引是批处理的方还是更新的，（因此存在一个main index和temporary index），这样一来，index占用锁的时间就降低了。


index为什么要使用std::multimap<uint64,uint64>的数据结构？



> 存在的问题：
> 1. index索引是怎么持久化的
> 2. 各个记录的偏移是怎么计算得到的    storageEngine: std::mulitmap<> index索引，

