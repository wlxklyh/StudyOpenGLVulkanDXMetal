//
// Created by wlxklyh on 2021/10/17.
//

#ifndef ROOKIERENDERER_LAMBERTIAN_H
#define ROOKIERENDERER_LAMBERTIAN_H

#include "Vec3.h"
#include "Material.h"

class Lambertian : public Material {
public:
    RMath::FColorRGB Albedo;

    Lambertian(const RMath::FColorRGB &a) : Albedo(a) {};

    bool Scatter(const RMath::Ray &rayIn, const RMath::HitRecord &rec,
                 RMath::FVec3 &attenuation, RMath::Ray &scattered) const override {
        RMath::FPoint3D target = rec.p + rec.normal + RandomInUnitSphere();
        scattered = RMath::Ray(rec.p, target - rec.p);
        attenuation = Albedo;
        return true;
    }
};


#endif //ROOKIERENDERER_LAMBERTIAN_H
