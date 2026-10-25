#ifndef UDP_MANAGER_H
#define UDP_MANAGER_H

#include <stddef.h>

namespace UDPManager
{
    bool begin();

    bool receive(char* buffer, size_t bufferSize);
}

#endif