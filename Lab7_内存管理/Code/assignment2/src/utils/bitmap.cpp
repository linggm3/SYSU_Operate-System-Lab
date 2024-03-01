#include "bitmap.h"
#include "stdlib.h"
#include "stdio.h"

BitMap::BitMap()
{
}

void BitMap::initialize(char *bitmap, const int length)
{
    this->bitmap = bitmap;
    this->length = length;

    int bytes = ceil(length, 8);

    for (int i = 0; i < bytes; ++i)
    {
        bitmap[i] = 0;
    }
}

bool BitMap::get(const int index) const
{
    int pos = index / 8;
    int offset = index % 8;

    return (bitmap[pos] & (1 << offset));
}

void BitMap::set(const int index, const bool status)
{
    int pos = index / 8;
    int offset = index % 8;

    // 清0
    bitmap[pos] = bitmap[pos] & (~(1 << offset));

    // 置1
    if (status)
    {
        bitmap[pos] = bitmap[pos] | (1 << offset);
    }
}

int BitMap::allocate(const int count)
{
    if (count == 0)
        return -1;

    int index, empty, start;
    int smallest_fit_empty = 0x7fffffff, smallest_fit_start = -1;

    index = 0;
    while (index < length)
    {
        // 越过已经分配的资源
        while (index < length && get(index))
            ++index;

        // 不存在连续的count个资源
        if (index == length)
            break;

        // 找到1个未分配的资源
        // 检查是否存在从index开始的连续count个资源
        empty = 0;
        start = index;
        // 直到搜索到头 或者 搜索到已分配资源
        while ((index < length) && !get(index))
        {
            ++empty;
            ++index;
        }

        // 存在大于等于count个的空位，且空位数量小于已知最小数量
        if (empty >= count && empty < smallest_fit_empty)
        {
            smallest_fit_empty = empty;
            smallest_fit_start = start;
        }
    }
    if(smallest_fit_start != -1){
        for (int i = 0; i < count; ++i)
            {
                set(smallest_fit_start + i, true);
            }
        }
    return smallest_fit_start;
}

void BitMap::release(const int index, const int count)
{
    for (int i = 0; i < count; ++i)
    {
        set(index + i, false);
    }
}

char *BitMap::getBitmap()
{
    return (char *)bitmap;
}

int BitMap::size() const
{
    return length;
}
