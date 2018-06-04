//
// Created by KingSun on 2018/4/22.
//

#ifndef VEC4_H
#define VEC4_H

#include <cassert>
#include <iosfwd>
#include "../Header.h"
#include "./function.h"
#include "./Vec2.h"
#include "./Vec3.h"

namespace KVector{
    class Vec4{
    public:
        union {
            Kfloat values[4];
            struct { Kfloat x, y, z, w; };
            struct { Kfloat r, g, b, a; };
            struct { Kfloat s, t, p, q; };
        };

        Vec4():Vec4(0){}
        Vec4(const Vec4 &v):x(v.x), y(v.y), z(v.z), w(v.w){}
        explicit Vec4(const Kfloat &c):x(c), y(c), z(c), w(c){}
        Vec4(const Kfloat &x, const Kfloat &y, const Kfloat &z, const Kfloat &w):
                x(x), y(y), z(z), w(w){}
        Vec4(const Vec2 &v1, const Vec2 &v2):x(v1.x), y(v1.y), z(v2.x), w(v2.y){}
        Vec4(const Vec3 &v, const Kfloat &w):x(v.x), y(v.y), z(v.z), w(w){}

        inline Kint dimension()const {
            return 4;
        }
        const Kfloat* data()const {
            return this->values;
        }

        void set(const Kfloat &x, const Kfloat &y, const Kfloat &z, const Kfloat &w){
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }
        void set(const Kfloat &c){
            this->x = c;
            this->y = c;
            this->z = c;
            this->w = c;
        }
        Kfloat& operator[](Kuint n){
            assert(n >= 0 && n < 4);
            return this->values[n];

        }
        const Kfloat& operator[](Kuint n)const {
            assert(n >= 0 && n < 4);
            return this->values[n];
        }

		inline Kboolean operator==(const Vec4& v)const {
			return x == v.x && y == v.y && z == v.z && w == v.w;
		}
		inline Kboolean operator!=(const Vec4& v)const {
			//return x != v.x || y != v.y || z != v.z || w != v.w;
			return !this->operator==(v);
		}

        Vec4& operator=(const Vec4 &v){
            set(v.x, v.y, v.z, v.w);
            return *this;
        }
        inline Vec4& operator+=(const Vec4 &v){
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            this->w += v.w;
            return *this;
        }
        inline Vec4& operator-=(const Vec4 &v){
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            this->w -= v.w;
            return *this;
        }
        inline Vec4& operator*=(const Vec4 &v){
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            this->w *= v.w;
            return *this;
		}
		inline Vec4& operator/=(const Vec4 &v) {
			this->x /= v.x;
			this->y /= v.y;
			this->z /= v.z;
			this->w /= v.w;
			return *this;
		}

        template <typename T>
        inline Vec4& operator+=(const T &c){
            this->x += c;
            this->y += c;
            this->z += c;
            this->w += c;
            return *this;
        }
        template <typename T>
        inline Vec4& operator-=(const T &c){
            this->x -= c;
            this->y -= c;
            this->z -= c;
            this->w -= c;
            return *this;
        }
        template <typename C>
        inline Vec4& operator*=(const C &c){
            this->x *= c;
            this->y *= c;
            this->z *= c;
            this->w *= c;
            return *this;
        }
        template <typename C>
        inline Vec4& operator/=(const C &c){
            this->x /= c;
            this->y /= c;
            this->z /= c;
            this->w /= c;
            return *this;
        }

        template <typename F = Kdouble>
        F length()const {
            return KFunction::length<Vec4, F>(*this);
        }
        template <typename F = Kdouble>
        F dot(const Vec4 &v)const {
            return KFunction::dot<Vec4, F>(*this, v);
        }
        Vec4& normalize(){
            const auto len = length<Kfloat >();
            if(!KFunction::isZero(len)) this->operator/=(len);
            else set(static_cast<Kfloat>(KNAN));
            return *this;
        }

        Vec3 toVec3()const {
            return Vec3(x, y, z);
        }
    };

    Vec4 operator-(const Vec4 &v){
        return Vec4() -= v;
    }
    Vec4 operator+(const Vec4 &v1, const Vec4 &v2){
        return Vec4(v1) += v2;
    }
    Vec4 operator-(const Vec4 &v1, const Vec4 &v2){
        return Vec4(v1) -= v2;
    }
    Vec4 operator*(const Vec4 &v1, const Vec4 &v2){
        return Vec4(v1) *= v2;
	}
	Vec4 operator/(const Vec4 &v1, const Vec4 &v2) {
		return Vec4(v1) /= v2;
	}

    template <typename C>
    Vec4 operator*(const Vec4 &v, const C &c){
        return Vec4(v) *= c;
    }
    //防止与Mat4 * Vec4发生冲突，不将常数c设置为其他类型
    Vec4 operator*(const Kfloat &c, const Vec4 &v){
        return Vec4(v) *= c;
    }
    template <typename C>
    Vec4 operator/(const Vec4 &v, const C &c){
        return Vec4(v) /= c;
    }
    std::istream& operator>>(std::istream &is, Vec4 &v){
        is >> v.x >> v.y >> v.z >> v.w;
        return is;
    }
    std::ostream& operator<<(std::ostream &os, const Vec4 &v){
        os << v.x << " " << v.y << " " << v.z << " " << v.w;
        return os;
    }
}

#endif //VEC4_H
