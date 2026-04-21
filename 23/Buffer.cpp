#include "Buffer.h"

/*
#pragma once
#include <string>
#include <iostream>

class Buffer{
private:
    std::string buf_;

public:
    Buffer();
    ~Buffer();

    void append(const char *data, size_t size); //把数据追加到buf_中
    size_t size();                              //返回buf_的大小
    const char *data();                         //返回buf_的首地址
    void clear()                                //清空buf_     
};
*/

Buffer::Buffer()
{

}

Buffer::~Buffer()
{

}

void Buffer::append(const char *data, size_t size)
{
    buf_.append(data, size);
}

size_t Buffer::size()
{
    return buf_.size();
}

const char * Buffer::data()
{
    return buf_.data();
}

 //擦除从pos开始的nn个字节
void Buffer::erase(size_t pos, size_t nn)
{
    buf_.erase(pos, nn);
}   

void Buffer::clear()
{
    buf_.clear();
}  