//
// Created by wlxklyh on 2021/10/16.
//

#include "HitTableList.h"

namespace RMath {
    bool HitTableList::Hit(const Ray &ray, float t_min, float t_max, HitRecord &rec) const {
        bool hitAnything = false;
        HitRecord recTmp;
        double closestTime = t_max;
        for (int i = 0; i < Size; ++i) {
            if (List[i]->Hit(ray, t_min, closestTime, recTmp)) {
                hitAnything = true;
                closestTime = recTmp.t;
                rec = recTmp;
            }
        }
        return hitAnything;
    }
}