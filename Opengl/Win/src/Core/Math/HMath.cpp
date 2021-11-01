//
// Created by linyanhou on 2021/10/23.
//

#include <cstdlib>
#include "HMath.h"
float frand(float start,float end)
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    return r * (end - start) + start;
}