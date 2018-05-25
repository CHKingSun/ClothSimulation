//
// Created by KingSun on 2018/4/21.
//

#ifndef VEC2_H
#define VEC2_H

#include <cassert>
#include <iosfwd>
#include "./function.h"
#include "../Header.h"

namespace KVector{
    class Vec2{
    public:
        union {
            Kfloat values[2];
            struct { Kfloat x, y; };
            struct { Kfloat s, t; };
        };

        Vec2():Vec2(0){}
        Vec2(const Vec2 &v):x(v.x), y(v.y){}
        explicit Vec2(const Kfloat &c):x(c), y(c){}
        Vec2(const Kfloat &x, const Kfloat &y):x(x), y(y){}

        inline Kint dimension()const {
            return 2;
        }
        const Kfloat* data()const{
            return this->values;
        }
//            Kfloat* data(){
//                return this->values;
//            }

        void set(const Kfloat &x, const Kfloat &y){
            this->x = x;
            this->y = y;
        }
        void set(const Kfloat &c){
            this->x = c;
            this->y = c;
        }
        Kfloat& operator[](Kuint n){
            assert(n >= 0 && n < 2);
            return this->values[n]; //return reference, for assign
        }
        const Kfloat& operator[](Kuint n)const{
            assert(n >= 0 && n < 2);
            return this->values[n]; //for const object, just get value.
        }

        Vec2& operator=(const Vec2 &v){
            set(v.x, v.y);
            return *this;
        }
        inline Vec2& operator+=(const Vec2 &v){
            this->x += v.x;
            this->y += v.y;
            return *this;
        }
        inline Vec2& operator-=(const Vec2 &v){
            this->x -= v.x;
            this->y -= v.y;
            return *this;
        }
        inline Vec2& operator*=(const Vec2 &v){
            this->x *= v.x;
            this->y *= v.y;
            return *this;
		}
		inline Vec2& operator/=(const Vec2 &v) {
			this->x /= v.x;
			this->y /= v.y;
			return *this;
		}

		template <typename C>
		inline Vec2& operator/=(const C &c) {
			this->x /= c;
			this->y /= c;
			return *this;
		}
        template <typename C>
        inline Vec2& operator*=(const C &c){
            this->x *= c;
            this->y *= c;
            return *this;
        }

        template <typename F = Kdouble>
        F length()const {
            return KFunction::length<Vec2, F>(*this);
        }
        template <typename F = Kdouble>
        F dot(const Vec2 &v)const {
            return KFunction::dot<Vec2, F>(*this, v);
        }
        Vec2& normalize(){
            const auto len = length<Kfloat>();
            if(!KFunction::isZero(len)) this->operator/=(len);
            else this->set(static_cast<Kfloat>(KNAN));
            return *this;
        }
    };

    Vec2 operator-(const Vec2 &v){
        return Vec2() -= v;
    }
    Vec2 operator+(const Vec2 &v1, const Vec2 &v2){
        return Vec2(v1) += v2;
    }
    Vec2 operator-(const Vec2 &v1, const Vec2 &v2){
        return Vec2(v1) -= v2;
    }
    Vec2 operator*(const Vec2 &v1, const Vec2 &v2){
        return Vec2(v1) *= v2;
	}
	Vec2 operator/(const Vec2 &v1, const Vec2 &v2) {
		return Vec2(v1) /= v2;
	}
    template <typename C>
    Vec2 operator*(const Vec2 &v, const C &c){
        return Vec2(v) *= c;
    };
    template <typename C>
    Vec2 operator*(const C &c, const Vec2 &v){
        return Vec2(v) *= c;
    };
    template <typename C>
    Vec2 operator/(const Vec2 &v, const C &c){
        return Vec2(v) /= c;
    };
    std::istream& operator>>(std::istream &is, Vec2 &v){
        is >> v.x >> v.y;
        return is;
    }
    std::ostream& operator<<(std::ostream &os, const Vec2 &v){
        os << v.x << " " << v.y;
        return os;
    }
}


#endif //VEC2_H
