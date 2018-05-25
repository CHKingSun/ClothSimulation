//
// Created by KingSun on 2018/5/2.
//

#ifndef KENGINE_QUATERNION_H
#define KENGINE_QUATERNION_H

#include "../Header.h"
#include "./Mat4.h"

namespace KMatrix{
    class Quaternion{
        //normalized quaternion, w * w + x * x + y * y + z * z = 1;

    private:
        Kfloat w, x, y, z;

        Quaternion(const Kfloat &w, const Kfloat &x, const Kfloat &y, const Kfloat &z):
                w(w), x(x), y(y), z(z){}

    public:
        Quaternion(): w(1), x(0), y(0), z(0){} //rotate zero degree
        Quaternion(const Kfloat &angle, const KVector::Vec3 &v){
            set(angle, v);
        }

        bool isValid()const {
            return KFunction::abs(w*w+x*x+y*y+z*z - static_cast<Kfloat >(1))
                    < static_cast<Kfloat>(EPSILON_E6);
        }

        void set(const Kfloat &w, const Kfloat &x, const Kfloat &y, const Kfloat &z){
            if(KFunction::abs(w * w + x * x + y * y + z * z - 1) >= EPSILON_E6)
                return;
            this->w = w;
            this->x = x;
            this->y = y;
            this->z = z;
        }
        void set(const Kfloat &angle, KVector::Vec3 v){
            v.normalize();
            const auto half = KFunction::toRadian<Kfloat>(angle / static_cast<Kfloat>(2));
            const auto s = static_cast<Kfloat>(sin(half));
            this->w = static_cast<Kfloat>(cos(half));
            this->x = v.x * s;
            this->y = v.y * s;
            this->z = v.z * s;
        }
        void setDefault(){
            this->w = 1;
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }

		Quaternion& operator=(const Quaternion& q) = default;
        Quaternion& operator*=(const Quaternion &q){
            //i*i = j*j = k*k = ijk = -1
            //ij = k, ki = -k;
            //jk = i, kj = -i;
            //ki = j, ik = -j;

            const Kfloat tw = this->w;
            const Kfloat tx = this->x;
            const Kfloat ty = this->y;
            const Kfloat tz = this->z;

            this->w = tw * q.w - tx * q.x - ty * q.y - tz * q.z;
            this->x = tw * q.x + tx * q.w + ty * q.z - tz * q.y;
            this->y = tw * q.y - tx * q.z + ty * q.w + tz * q.x;
            this->z = tw * q.z + tx * q.y - ty * q.x + tz * q.w;

            return *this;
        }
        Quaternion& fromMatrix(const Mat3 &m){
            this->w = static_cast<Kfloat>(std::sqrt(m[0][0]+m[1][1]+m[2][2]+1) / 2.0);
            this->x = (m[2][1] - m[1][2]) / (static_cast<Kfloat>(4) * w);
            this->y = (m[0][2] - m[2][0]) / (static_cast<Kfloat>(4) * w);
            this->z = (m[1][0] - m[0][1]) / (static_cast<Kfloat>(4) * w);
            if(!isValid()) setDefault();
            return *this;
        }
        Quaternion& fromMatrix(const Mat4 &m){
            this->w = static_cast<Kfloat>(std::sqrt(m[0][0]+m[1][1]+m[2][2]+1) / 2.0);
            this->x = (m[2][1] - m[1][2]) / (static_cast<Kfloat>(4) * w);
            this->y = (m[0][2] - m[2][0]) / (static_cast<Kfloat>(4) * w);
            this->z = (m[1][0] - m[0][1]) / (static_cast<Kfloat>(4) * w);
            if(!isValid()) setDefault();
            return *this;
        }

        Quaternion getConjugate()const {
            return Quaternion(w, -x, -y, -z);
        }
        Mat3 toMat3()const {
            Kfloat w2 = w * w;
            Kfloat x2 = x * x;
            Kfloat y2 = y * y;
            Kfloat z2 = z * z;

            Kfloat wx = w * x;
            Kfloat wy = w * y;
            Kfloat wz = w * z;

            Kfloat xy = x * y;
            Kfloat xz = x * z;
            Kfloat yz = y * z;

            return Mat3(
                    w2+x2-y2-z2, 2*(xy-wz), 2*(xz+wy),
                    2*(xy+wz), w2-x2+y2-z2, 2*(yz-wx),
                    2*(xz-wy), 2*(yz+wx), w2-x2-y2+z2
            );
        }
        Mat4 toMat4()const {
            Kfloat w2 = w * w;
            Kfloat x2 = x * x;
            Kfloat y2 = y * y;
            Kfloat z2 = z * z;

            Kfloat wx = w * x;
            Kfloat wy = w * y;
            Kfloat wz = w * z;

            Kfloat xy = x * y;
            Kfloat xz = x * z;
            Kfloat yz = y * z;

            return Mat4(
                    w2+x2-y2-z2, 2*(xy-wz), 2*(xz+wy), 0,
                    2*(xy+wz), w2-x2+y2-z2, 2*(yz-wx), 0,
                    2*(xz-wy), 2*(yz+wx), w2-x2-y2+z2, 0,
                    0, 0, 0, 1
            );
        }

        friend KVector::Vec3 operator*(const Quaternion &q, const KVector::Vec3 &v);
        friend std::istream& operator>>(std::istream &is, Quaternion &q);
        friend std::ostream& operator<<(std::ostream &os, const Quaternion &q);
    };

	Quaternion operator-(const Quaternion& q) {
		return q.getConjugate();
	}

    Quaternion operator*(const Quaternion &q1, const Quaternion &q2) {
        return Quaternion(q1)*=q2;
    }
    KVector::Vec3 operator*(const Quaternion &q, const KVector::Vec3 &v){
        Quaternion p = (q * Quaternion(0, v.x, v.y, v.z)) * q.getConjugate();
        //do not use Quaternion(angle, v) for vector will be normalized.
        return KVector::Vec3(p.x, p.y, p.z);
    }

    std::istream& operator>>(std::istream &is, Quaternion &q) {
        is >> q.w >> q.x >> q.y >> q.z;
        return is;
    }
    std::ostream& operator<<(std::ostream &os, const Quaternion &q) {
        os << q.w << " " << q.x << " " << q.y << " " << q.z;
        return os;
    }
}

#endif //QUATERNION_H
