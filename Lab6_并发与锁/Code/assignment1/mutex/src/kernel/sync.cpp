#include "sync.h"
#include "asm_utils.h"
#include "stdio.h"
#include "os_modules.h"

Mutex::Mutex()
{
    initialize();
}

void Mutex::initialize()
{
    bolt = 1;
}

void Mutex::lock()
{
    while(bolt == 0);
    bolt = 0;
}

void Mutex::unlock()
{
    bolt = 1;
}
