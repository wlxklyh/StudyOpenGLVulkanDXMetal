//
// Created by wlxklyh on 2021/10/16.
//

#ifndef ROOKIERENDERER_MATERIAL_H
#define ROOKIERENDERER_MATERIAL_H

#include "Ray.h"
#include "Hitable.h"

RMath::FVec3 Reflect(const RMath::FVec3 &v, const RMath::FVec3 &n);

double RandomDouble();

//看这个：https://blog.csdn.net/yinhun2012/article/details/79472364
bool Refract(const RMath::FVec3 &v, const RMath::FVec3 &n, float ni_over_nt, RMath::FVec3 &refracted);

float Schlick(float cosine, float refIdx);

RMath::FVec3 RandomInUnitSphere();

class Material {
public:
    virtual bool Scatter(const RMath::Ray &rayIn, const RMath::HitRecord &rec,
                         RMath::FVec3 &attenuation, RMath::Ray &scattered) const = 0;
};


#endif //ROOKIERENDERER_MATERIAL_H
