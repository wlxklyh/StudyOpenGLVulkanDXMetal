//
// Created by linyanhou on 2021/10/18.
//
#ifdef _WIN32
#ifndef ROOKIERENDERER_PLATWINDOWS_H
#define ROOKIERENDERER_PLATWINDOWS_H


#ifndef DRAND48_H
#define DRAND48_H

#include <stdlib.h>

#define _USE_MATH_DEFINES
#define MAXFLOAT 99999999999999999.0f

#define RAND_TEMP_M 0x100000000LL
#define RAND_TEMP_C 0xB16
#define RAND_TEMP_A 0x5DEECE66DLL

static unsigned long long seed = 1;

double drand48(void);

void srand48(unsigned int i);

#endif

class PlatWindows {

};


#endif //ROOKIERENDERER_PLATWINDOWS_H
#endif
