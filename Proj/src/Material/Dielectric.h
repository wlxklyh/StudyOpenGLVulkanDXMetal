//
// Created by wlxklyh on 2021/10/17.
//

#ifndef ROOKIERENDERER_DIELECTRIC_H
#define ROOKIERENDERER_DIELECTRIC_H

#include "Vec3.h"
#include "Material.h"

class Dielectric : public Material {
public:
    float RefIdx;

    Dielectric(float ri) : RefIdx(ri) {}

    virtual bool Scatter(const RMath::Ray &ray, const RMath::HitRecord &rec,
                         RMath::FVec3 &attenuation, RMath::Ray &scattered) const {
        RMath::FVec3 outwardNormal;
        RMath::FVec3 reflected = Reflect(ray.Direction(), rec.normal);
        float ni_over_nt;
        attenuation = RMath::FVec3(1.0, 1.0, 1.0);
        RMath::FVec3 refracted;
        float reflect_prob;
        float cosine;
        //如果大雨90入射
        if (dot(ray.Direction(), rec.normal) > 0) {
            outwardNormal = -rec.normal;
            ni_over_nt = RefIdx;
            // cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = dot(ray.Direction(), rec.normal) / ray.Direction().length();
            cosine = sqrt(1 - RefIdx * RefIdx * (1 - cosine * cosine));
        } else {
            outwardNormal = rec.normal;
            ni_over_nt = 1.0 / RefIdx;
            cosine = -dot(ray.Direction(), rec.normal) / ray.Direction().length();
        }

        if (Refract(ray.Direction(), outwardNormal, ni_over_nt, refracted))
            reflect_prob = Schlick(cosine, RefIdx);
        else
            reflect_prob = 1.0;
        if (RandomDouble() < reflect_prob)
            scattered = RMath::Ray(rec.p, reflected);
        else
            scattered = RMath::Ray(rec.p, refracted);
        return true;
    }

};


#endif //ROOKIERENDERER_DIELECTRIC_H
