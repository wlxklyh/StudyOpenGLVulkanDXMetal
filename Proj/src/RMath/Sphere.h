//
// Created by wlxklyh on 2021/10/16.
//

#ifndef ROOKIERENDERER_SPHERE_H
#define ROOKIERENDERER_SPHERE_H

#include "Hitable.h"
#include "Material.h"

namespace RMath {
    class Sphere : public Hitable {
    public:
        FPoint3D Center;
        float Radius;
        Material *MatPtr;

        Sphere() {};

        Sphere(FVec3 center, float radius, Material *matPtr) : Center(center), Radius(radius), MatPtr(matPtr) {};

        virtual bool Hit(const Ray &ray, float t_min, float t_max, HitRecord &rec) const;
    };
}


#endif //ROOKIERENDERER_SPHERE_H
