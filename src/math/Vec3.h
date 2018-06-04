//
// Created by KingSun on 2018/4/22.
//

#ifndef VEC3_H
#define VEC3_H

#include <cassert>
#include <iosfwd>
#include "../Header.h"
#include "./Vec2.h"

namespace KVector{
    class Vec3{
    public:
        union {
            Kfloat values[3];
            struct { Kfloat x, y, z; };
            struct { Kfloat r, g ,b; };
        };

        Vec3():Vec3(0){}
        Vec3(const Vec3 &v):x(v.x), y(v.y), z(v.z){}
        explicit Vec3(const Kfloat &c):x(c), y(c), z(c){}
        Vec3(const Kfloat &x, const Kfloat &y, const Kfloat &z):x(x), y(y), z(z){}
        Vec3(const Vec2 &v, const Kfloat &z):x(v.x), y(v.y), z(z){}
        Vec3(const Kfloat &x, const Vec2 &v):x(x), y(v.x), z(v.y){}

        inline Kint dimension()const {
            return 3;
        }
        const Kfloat* data()const {
            return this->values;
        }
//            Kfloat* data(){
//                return this->values;
//            }

        void set(const Kfloat &x, const Kfloat &y, const Kfloat &z){
            this->x = x;
            this->y = y;
            this->z = z;
        }
        void set(const Kfloat &c){
            this->x = c;
            this->y = c;
            this->z = c;
        }
        Kfloat& operator[](Kuint n){
            assert(n >= 0 && n < 3);
            return this->values[n];
        }
        const Kfloat& operator[](Kuint n)const {
            assert(n >= 0 && n < 3);
            return this->values[n];
		}

		inline Kboolean operator==(const Vec3& v)const {
			return x == v.x && y == v.y && z == v.z;
		}
		inline Kboolean operator!=(const Vec3& v)const {
			//return x != v.x || y != v.y || z != v.z;
			return !this->operator==(v);
		}

        Vec3& operator=(const Vec3 &v){
            set(v.x, v.y, v.z);
            return *this;
        }
        inline Vec3& operator+=(const Vec3 &v){
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            return *this;
        }
        inline Vec3& operator-=(const Vec3 &v){
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            return *this;
        }
        inline Vec3& operator*=(const Vec3 &v){
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            return *this;
		}
		inline Vec3& operator/=(const Vec3 &v) {
			this->x /= v.x;
			this->y /= v.y;
			this->z /= v.z;
			return *this;
		}

        template <typename C>
        inline Vec3& operator*=(const C &c){
            this->x *= c;
            this->y *= c;
            this->z *= c;
            return *this;
        }
        template <typename C>
        inline Vec3& operator/=(const C &c){
            this->x /= c;
            this->y /= c;
            this->z /= c;
            return *this;
        }

        template <typename F = Kdouble>
        F length()const {
            return KFunction::length<Vec3, F>(*this);
        }
        Kboolean isZero()const {
            return KFunction::isZero(length());
        }
        template <typename F = Kdouble>
        F dot(const Vec3 &v)const {
            return KFunction::dot<Vec3, F>(*this, v);
        }
        Vec3& normalize(){
            //maybe need judging length != 0
            const auto len = length<Kfloat>();
            if(!KFunction::isZero(len)) this->operator/=(len);
            else set(static_cast<Kfloat>(KNAN));
            return *this;
        }
        Vec3& cross(const Vec3 &v){
            Kfloat tx = this->x, ty = this->y, tz = this->z;
            this->x = ty * v.z - v.y * tz;
            this->y = - (tx * v.z - v.x * tz);
            this->z = tx * v.y - v.x * ty;
            return *this;
        }
        template <typename F = Kdouble>
        F getAngle(const Vec3 &v)const {
            const F len1 = length<F>();
            const F len2 = v.length<F>();
            if(KFunction::isZero(len1) || KFunction::isZero(len2)) return static_cast<F>(KNAN);

            return F(acos(dot<F>(v) / (len1 * len2)));
        }

        static Vec3 cross(const Vec3 &v1, const Vec3 &v2){
            Kfloat tx = v1.x, ty = v1.y, tz = v1.z;
            Kfloat vx = v2.x, vy = v2.y, vz = v2.z;
            Kfloat rx = ty * vz - vy * tz;
            Kfloat ry = - (tx * vz - vx * tz);
            Kfloat rz = tx * vy - vx * ty;
            return Vec3(rx, ry, rz);
        }
    };

    Vec3 operator-(const Vec3 &v){
        return Vec3() -= v;
    }
    Vec3 operator+(const Vec3 &v1, const Vec3 &v2){
        return Vec3(v1) += v2;
    }
    Vec3 operator-(const Vec3 &v1, const Vec3 &v2){
        return Vec3(v1) -= v2;
    }
    Vec3 operator*(const Vec3 &v1, const Vec3 &v2){
        return Vec3(v1) *= v2;
	}
	Vec3 operator/(const Vec3 &v1, const Vec3 &v2) {
		return Vec3(v1) /= v2;
	}
    template <typename C>
    Vec3 operator*(const Vec3 &v, const C &c){
        return Vec3(v) *= c;
    };
    //防止与Mat3 * Vec3 发生冲突，不将常数c设置为其他类型
    Vec3 operator*(const Kfloat &c, const Vec3 &v){
        return Vec3(v) *= c;
    };
    template <typename C>
    Vec3 operator/(const Vec3 &v, const C &c){
        return Vec3(v) /= c;
    };
    std::istream& operator>>(std::istream &is, Vec3 &v){
        is >> v.x >> v.y >> v.z;
        return is;
    }
    std::ostream& operator<<(std::ostream &os, const Vec3 &v){
        os << v.x << " " << v.y << " " << v.z;
        return os;
    }
}

#endif //VEC3_H
