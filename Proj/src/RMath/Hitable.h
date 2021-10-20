//
// Created by wlxklyh on 2021/10/16.
//

#ifndef ROOKIERENDERER_HITABLE_H
#define ROOKIERENDERER_HITABLE_H

#include "Ray.h"

class Material;

namespace RMath {
    struct HitRecord {
        float t;
        FVec3 p;
        FVec3 normal;
        Material *mat;
    };

    class Hitable {
    public:
        virtual bool Hit(const Ray &ray, float t_min, float t_max, HitRecord &rec) const = 0;
    };
}


#endif //ROOKIERENDERER_HITABLE_H
