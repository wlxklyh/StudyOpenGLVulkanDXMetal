//
// Created by wlxklyh on 2021/10/16.
//

#include "Vec3.h"

namespace RMath {
    template<class T>
    inline std::istream &operator>>(std::istream &is, Vec3<T> &v) {
        is >> v[0] >> v[1] >> v[2];
        return is;
    }

    template<class T>
    inline std::ostream &operator<<(std::ostream &os, Vec3<T> &v) {
        os << v[0] << v[1] << v[2];
        return os;
    }
}

