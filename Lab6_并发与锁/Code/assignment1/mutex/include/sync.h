#ifndef SYNC_H
#define SYNC_H

#include "os_type.h"

class Mutex
{
private:
    uint32 bolt;
public:
    Mutex();
    void initialize();
    void lock();
    void unlock();
};
#endif
