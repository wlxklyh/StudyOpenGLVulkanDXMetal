//
// Created by linyanhou on 2021/10/18.
//

#include "PlatWindows.h"
double drand48(void)
{
    seed = (RAND_TEMP_A * seed + RAND_TEMP_C) & 0xFFFFFFFFFFFFLL;
    unsigned int x = seed >> 16;
    return  ((double)x / (double)RAND_TEMP_M);

}

void srand48(unsigned int i)
{
    seed  = (((long long int)i) << 16) | rand();
}