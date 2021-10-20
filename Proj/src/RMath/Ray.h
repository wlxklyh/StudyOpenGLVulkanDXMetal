//
// Created by wlxklyh on 2021/10/16.
//

#ifndef ROOKIERENDERER_RAY_H
#define ROOKIERENDERER_RAY_H

#include "Vec3.h"

namespace RMath {
    class Ray {
    public:
        FVec3 A;
        FVec3 B;

        Ray() {};

        Ray(const Vec3<float> &a, const Vec3<float> &b) {
            A = a;
            B = b;
        }

        FVec3 Origin() const { return A; }

        FVec3 Direction() const { return B; }

        FVec3 PointAtParameter(float t) const { return A + B * t; }
    };

}


#endif //ROOKIERENDERER_RAY_H
