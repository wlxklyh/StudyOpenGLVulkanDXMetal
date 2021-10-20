//
// Created by wlxklyh on 2021/10/16.
//

#include "Camera.h"

FVec3 RandomInUnitDisk() {
    FVec3 p;
    do {
        p = 2.0 * FVec3(drand48(), drand48(), 0) - FVec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}