//
// Created by wlxklyh on 2021/10/16.
//

#ifndef ROOKIERENDERER_CAMERA_H
#define ROOKIERENDERER_CAMERA_H

#include "../RMath/Vec3.h"
#include "../RMath/Ray.h"

using namespace RMath;

FVec3 RandomInUnitDisk();

class Camera {
public:
    FPoint3D Origin;
    FPoint3D LeftBottomCorner;
    FVec3 Horizontal;
    FVec3 Vertical;
    FVec3 u, v, w;
    float LensRadius;

    Camera(FPoint3D lookFrom, FVec3 lookAt, FVec3 vUp, float vfov, float aspect, float aperture, float focusDist) {
        LensRadius = aperture / 2;
        float theta = vfov * M_PI / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;
        Origin = lookFrom;
        w = unit_vector(lookFrom - lookAt);
        u = unit_vector(cross(vUp, w));
        v = cross(w, u);
        LeftBottomCorner = Origin - half_width * focusDist * u - half_height * focusDist * v - w * focusDist;
        Horizontal = 2 * half_width * focusDist * u;
        Vertical = 2 * half_height * focusDist * v;
    }

    Ray GetRay(float s, float t) {
        FVec3 rd = LensRadius * RandomInUnitDisk();
        FVec3 offset = u * rd.X() + v * rd.Y();
        //offset = FVec3(0,0,0);
        //这里很关键 要思考为什么这么偏移就可以模糊 近景和远景都可以模糊
        return Ray(Origin + offset, LeftBottomCorner + s * Horizontal + t * Vertical - Origin - offset);
    }

};


#endif //ROOKIERENDERER_CAMERA_H
