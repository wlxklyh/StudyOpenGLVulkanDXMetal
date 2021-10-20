//
// Created by wlxklyh on 2021/10/16.
//

#ifndef ROOKIERENDERER_VEC3_H
#define ROOKIERENDERER_VEC3_H

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "PlatWindows.h"

namespace RMath {
    template<class T>
    class Vec3 {
    private:
        T e[3];
    public:
        Vec3() {};

        Vec3(T xx, T yy, T zz) {
            e[0] = xx;
            e[1] = yy;
            e[2] = zz;
        };

        inline T R() const { return e[0]; }

        inline T G() const { return e[1]; }

        inline T B() const { return e[2]; }

        inline T X() const { return e[0]; }

        inline T Y() const { return e[1]; }

        inline T Z() const { return e[2]; }

        inline const Vec3<T> &operator+() const { return *this; }

        inline Vec3<T> operator-() const { return Vec3<T>(-e[0], -e[1], -e[2]); }

        inline T operator[](int i) const { return e[i]; }

        inline T &operator[](int i) { return e[i]; }

        inline bool operator==(const Vec3<T> &rhs) const {
            return rhs[0] == e[0] && rhs[1] == e[1] && rhs[2] == e[2];
        }

        inline T length() const {
            return sqrt(squared_length());
        }

        inline T squared_length() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        inline Vec3<T> &operator+=(const Vec3<T> &v) {
            e[0] += v[0];
            e[1] += v[1];
            e[2] += v[2];
            return *this;
        };

        inline Vec3<T> &operator-=(const Vec3<T> &v) {
            e[0] -= v[0];
            e[1] -= v[1];
            e[2] -= v[2];
            return *this;
        }

        inline Vec3<T> &operator*=(const Vec3<T> &v) {
            e[0] *= v[0];
            e[1] *= v[1];
            e[2] *= v[2];
            return *this;
        }

        inline Vec3<T> &operator/=(const Vec3<T> &v) {
            e[0] /= v[0];
            e[1] /= v[1];
            e[2] /= v[2];
            return *this;
        }

        inline Vec3<T> &operator*=(const T v) {
            e[0] *= v;
            e[1] *= v;
            e[2] *= v;
            return *this;
        }

        inline Vec3<T> &operator/=(const T v) {
            e[0] /= v;
            e[1] /= v;
            e[2] /= v;
            return *this;
        }

        inline void make_unit_vector() {
            float k = 1.0 / length();
            e[0] *= k;
            e[1] *= k;
            e[2] *= k;
        }
    };


    template<class T>
    inline Vec3<T> operator+(const Vec3<T> &v1, const Vec3<T> &v2) {
        return Vec3<T>(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
    }

    template<class T>
    inline Vec3<T> operator-(const Vec3<T> &v1, const Vec3<T> &v2) {
        return Vec3<T>(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
    }

    template<class T>
    inline Vec3<T> operator*(const Vec3<T> &v1, const Vec3<T> &v2) {
        return Vec3<T>(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
    }

    template<class T>
    inline Vec3<T> operator/(const Vec3<T> &v1, const Vec3<T> &v2) {
        return Vec3<T>(v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2]);
    }

    template<class T>
    inline Vec3<T> operator/(const float v, const Vec3<T> &v1) {
        return Vec3<T>(v / v1[0], v / v1[1], v / v1[2]);
    }

    template<class T>
    inline Vec3<T> operator*(const float v, const Vec3<T> &v1) {
        return Vec3<T>(v1[0] * v, v1[1] * v, v1[2] * v);
    }

    template<class T>
    inline Vec3<T> operator*(const Vec3<T> &v1, const float v) {
        return Vec3<T>(v1[0] * v, v1[1] * v, v1[2] * v);
    }

    template<class T>
    inline Vec3<T> operator/(const Vec3<T> &v1, const float v) {
        return Vec3<T>(v1[0] / v, v1[1] / v, v1[2] / v);
    }

    template<class T>
    inline Vec3<T> operator-(const Vec3<T> &v1, const float v) {
        return Vec3<T>(v1[0] - v, v1[1] - v, v1[2] - v);
    }

    template<class T>
    inline Vec3<T> operator+(const Vec3<T> &v1, const float v) {
        return Vec3<T>(v1[0] + v, v1[1] + v, v1[2] + v);
    }

    template<class T>
    inline float dot(const Vec3<T> &v1, const Vec3<T> &v2) {
        return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
    }

    template<class T>
    inline Vec3<T> cross(const Vec3<T> &v1, const Vec3<T> &v2) {
        return Vec3<T>(
                v1[1] * v2[2] - v1[2] * v2[1],
                v1[2] * v2[0] - v1[0] * v2[2],
                v1[0] * v2[1] - v1[1] * v2[0]
        );
    }

    template<class T>
    inline Vec3<T> unit_vector(Vec3<T> v) {
        return v / v.length();
    }

    typedef Vec3<float> FVec3;
    typedef Vec3<float> FPoint3D;
    typedef Vec3<float> FColorRGB;

    typedef Vec3<int> IntVec3;
    typedef Vec3<float> IntPoint3D;
    typedef Vec3<int> IntColorRGB;
}

#endif //ROOKIERENDERER_VEC3_H
