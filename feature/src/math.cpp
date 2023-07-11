#include "common.h"

int32 RoundUpToNearestPowerOfTwo(int32 n)
{
    if(n > 0)
    {
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        return n + 1;
    }
    else
    {
        return 0;
    }
}
