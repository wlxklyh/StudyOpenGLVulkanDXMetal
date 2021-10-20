//
// Created by wlxklyh on 2021/10/17.
//

#ifndef ROOKIERENDERER_METAL_H
#define ROOKIERENDERER_METAL_H

#include "Vec3.h"
#include "Material.h"

class Metal : public Material {
public:
    RMath::FColorRGB Albedo;
    float Fuzz;

    Metal(const RMath::FColorRGB &a, float fuzz) : Albedo(a), Fuzz(fuzz) {};

    virtual bool Scatter(const RMath::Ray &rayIn, const RMath::HitRecord &rec,
                         RMath::FVec3 &attenuation, RMath::Ray &scattered) const override {
        RMath::FVec3 reflected = Reflect(RMath::unit_vector(rayIn.Direction()), rec.normal);
        scattered = RMath::Ray(rec.p, reflected + Fuzz * RandomInUnitSphere());
        attenuation = Albedo;
        return dot(scattered.Direction(), rec.normal) > 0;
    }

};

#endif //ROOKIERENDERER_METAL_H
