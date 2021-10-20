//
// Created by wlxklyh on 2021/10/16.
//

#ifndef ROOKIERENDERER_HITTABLELIST_H
#define ROOKIERENDERER_HITTABLELIST_H

#include "Hitable.h"

namespace RMath {
    class HitTableList : public Hitable {
    public:
        int Size;
        Hitable **List;

        HitTableList() {};

        HitTableList(Hitable **l, int size) : List(l), Size(size) {};

        virtual bool Hit(const Ray &ray, float t_min, float t_max, HitRecord &rec) const override;
    };
}


#endif //ROOKIERENDERER_HITTABLELIST_H
