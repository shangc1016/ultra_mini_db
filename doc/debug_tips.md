


#### 发生core dump，怎么debug core dump文件

1. 默认的coredump文件好像在`/var/lib/apport/coredump`
2. 然后gdb <binary file> <coredump file> 调试
3. 然后bt可以查看函数调用栈
