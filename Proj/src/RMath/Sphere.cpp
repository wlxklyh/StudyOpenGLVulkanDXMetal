//
// Created by wlxklyh on 2021/10/16.
//

#include "Sphere.h"


namespace RMath {
    bool Sphere::Hit(const Ray &ray, float t_min, float t_max, HitRecord &rec) const {
        FVec3 Origin2Center = ray.Origin() - Center;
        float a = dot(ray.Direction(), ray.Direction());
        float b = 2.0 * dot(Origin2Center, ray.Direction());
        float c = dot(Origin2Center, Origin2Center) - Radius * Radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return false;
        }
        float t[2];
        t[0] = (-sqrt(discriminant) - b) / (2.0 * a);
        t[1] = (+sqrt(discriminant) - b) / (2.0 * a);
        for (int i = 0; i < 2; i++) {
            if (t[i] < t_max && t[i] > t_min) {
                rec.t = t[i];
                rec.p = ray.PointAtParameter(t[i]);
                rec.normal = (rec.p - Center) / Radius;
                rec.mat = MatPtr;
                return true;
            }
        }
        return false;
    }

}