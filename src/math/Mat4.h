//
// Created by KingSun on 2018/4/23.
//

#ifndef MAT4_H
#define MAT4_H

#include <cassert>
#include <iosfwd>
#include "../Header.h"
#include "./Vec4.h"
#include "./Mat3.h"

namespace KMatrix{
    class Mat4{
        using Row = KVector::Vec4;

    private:
        Row values[4]; //row-first

    public:
        Mat4():Mat4(static_cast<Kfloat>(1)){}
        Mat4(const Mat4 &m){
            this->values[0] = m[0];
            this->values[1] = m[1];
            this->values[2] = m[2];
            this->values[3] = m[3];
        }
        explicit Mat4(const Kfloat &c){
            this->values[0] = Row(c, 0, 0, 0);
            this->values[1] = Row(0, c, 0, 0);
            this->values[2] = Row(0, 0, c, 0);
            this->values[3] = Row(0, 0, 0, c);
        }
        Mat4(const Row &v0, const Row &v1, const Row &v2, const Row &v3){
            this->values[0] = v0;
            this->values[1] = v1;
            this->values[2] = v2;
            this->values[3] = v3;
        }
        Mat4(const Kfloat &x0, const Kfloat &y0, const Kfloat &z0, const Kfloat &w0,
                const Kfloat &x1, const Kfloat &y1, const Kfloat &z1, const Kfloat &w1,
                const Kfloat &x2, const Kfloat &y2, const Kfloat &z2, const Kfloat &w2,
                const Kfloat &x3, const Kfloat &y3, const Kfloat &z3, const Kfloat &w3){
            this->values[0] = Row(x0, y0, z0, w0);
            this->values[1] = Row(x1, y1, z1, w1);
            this->values[2] = Row(x2, y2, z2, w2);
            this->values[3] = Row(x3, y3, z3, w3);
        }
        explicit Mat4(const Mat3 &m, const KVector::Vec3 &v = KVector::Vec3()){
            this->values[0] = Row(m[0], v[0]);
            this->values[1] = Row(m[1], v[1]);
            this->values[2] = Row(m[2], v[2]);
            this->values[3] = Row(0, 0, 0, 1);
        }

        const Kfloat* data()const {
            return &this->values[0][0];
        }

        Row& operator[](Kuint n){
            assert(n >= 0 && n < 4);
            return this->values[n];
        }
        const Row& operator[](Kuint n)const {
            assert(n >= 0 && n < 4);
            return this->values[n];
        }

        Mat4& operator=(const Mat4 &m){
            this->values[0] = m[0];
            this->values[1] = m[1];
            this->values[2] = m[2];
            this->values[3] = m[3];
			return *this;
        }
        Mat4& operator+=(const Mat4 &m){
            this->values[0] += m[0];
            this->values[1] += m[1];
            this->values[2] += m[2];
            this->values[3] += m[3];
            return *this;
        }
        Mat4& operator-=(const Mat4 &m){
            this->values[0] -= m[0];
            this->values[1] -= m[1];
            this->values[2] -= m[2];
            this->values[3] -= m[3];
            return *this;
        }
        Mat4& operator*=(const Mat4 &m){
            //matrix multiply. m * n != n * m;
            /**
                * Mat4 row(*this);
                * Mat4 col(m).transpose();
                * for(int i = 0; i < 4; ++i){
                *      for(int j = 0; j < 4; ++j){
                *          this->values[i][j] = row[i] * col[j];
                *      }
                * }
                */

            Row row0(this->values[0]);
            Row row1(this->values[1]);
            Row row2(this->values[2]);
            Row row3(this->values[3]);

            Row col0(m[0][0], m[1][0], m[2][0], m[3][0]);
            Row col1(m[0][1], m[1][1], m[2][1], m[3][1]);
            Row col2(m[0][2], m[1][2], m[2][2], m[3][2]);
            Row col3(m[0][3], m[1][3], m[2][3], m[3][3]);

            this->values[0][0] = KFunction::dot(row0, col0);
            this->values[0][1] = KFunction::dot(row0, col1);
            this->values[0][2] = KFunction::dot(row0, col2);
            this->values[0][3] = KFunction::dot(row0, col3);

            this->values[1][0] = KFunction::dot(row1, col0);
            this->values[1][1] = KFunction::dot(row1, col1);
            this->values[1][2] = KFunction::dot(row1, col2);
            this->values[1][3] = KFunction::dot(row1, col3);

            this->values[2][0] = KFunction::dot(row2, col0);
            this->values[2][1] = KFunction::dot(row2, col1);
            this->values[2][2] = KFunction::dot(row2, col2);
            this->values[2][3] = KFunction::dot(row2, col3);

            this->values[3][0] = KFunction::dot(row3, col0);
            this->values[3][1] = KFunction::dot(row3, col1);
            this->values[3][2] = KFunction::dot(row3, col2);
            this->values[3][3] = KFunction::dot(row3, col3);

            return *this;
        }

        template <typename C>
        Mat4& operator*=(const C &c){
            this->values[0] *= c;
            this->values[1] *= c;
            this->values[2] *= c;
            this->values[3] *= c;
            return *this;
        }
        template <typename C>
        Mat4& operator/=(const C &c){
            this->values[0] /= c;
            this->values[1] /= c;
            this->values[2] /= c;
            this->values[3] /= c;
            return *this;
        }

        Mat4& transpose(){
            for(int i = 0; i < 4; ++i){
                for(int j = i + 1; j < 4; ++j){
                    Kfloat t = this->values[i][j];
                    this->values[i][j] = this->values[j][i];
                    this->values[j][i] = t;
                }
            }
            return *this;
        }
        Mat4& inverse(){
            // | 00 01 02 03 |
            // | 10 11 12 13 |
            // | 20 21 22 23 |
            // | 30 31 32 33 |
            const Kfloat det0 = values[2][0] * values[3][1] - values[3][0] * values[2][1];
            const Kfloat det1 = values[2][0] * values[3][2] - values[3][0] * values[2][2];
            const Kfloat det2 = values[2][0] * values[3][3] - values[3][0] * values[2][3];
            const Kfloat det3 = values[2][1] * values[3][2] - values[3][1] * values[2][2];
            const Kfloat det4 = values[2][1] * values[3][3] - values[3][1] * values[2][3];
            const Kfloat det5 = values[2][2] * values[3][3] - values[3][2] * values[2][3];

            const Kfloat cof00 = values[1][1] * det5 - values[1][2] * det4 + values[1][3] * det3;
            const Kfloat cof01 = values[1][0] * det5 - values[1][2] * det2 + values[1][3] * det1;
            const Kfloat cof02 = values[1][0] * det4 - values[1][1] * det2 + values[1][3] * det0;
            const Kfloat cof03 = values[1][0] * det3 - values[1][1] * det1 + values[1][2] * det0;

            const Kfloat detM = values[0][0] * cof00 - values[0][1] * cof01 +
                            values[0][2] * cof02 - values[0][3] * cof03;
            if(fabs(detM) <= 1E-6){
                for (auto &value : values)
                    for(int j = 0; j < 4; ++j)
                        value[j] = static_cast<Kfloat>(KNAN);
                return *this;
            }

            const Kfloat cof10 = values[0][1] * det5 - values[0][2] * det4 + values[0][3] * det3;
            const Kfloat cof11 = values[0][0] * det5 - values[0][2] * det2 + values[0][3] * det1;
            const Kfloat cof12 = values[0][0] * det4 - values[0][1] * det2 + values[0][3] * det0;
            const Kfloat cof13 = values[0][0] * det3 - values[0][1] * det1 + values[0][2] * det0;

            const Kfloat det6 = values[0][0] * values[1][1] - values[1][0] * values[0][1];
            const Kfloat det7 = values[0][0] * values[1][2] - values[1][0] * values[0][2];
            const Kfloat det8 = values[0][0] * values[1][3] - values[1][0] * values[0][3];
            const Kfloat det9 = values[0][1] * values[1][2] - values[1][1] * values[0][2];
            const Kfloat det10 = values[0][1] * values[1][3] - values[1][1] * values[0][3];
            const Kfloat det11 = values[0][2] * values[1][3] - values[1][2] * values[0][3];

            const Kfloat cof20 = values[3][1] * det11 - values[3][2] * det10 + values[3][3] * det9;
            const Kfloat cof21 = values[3][0] * det11 - values[3][2] * det8 + values[3][3] * det7;
            const Kfloat cof22 = values[3][0] * det10 - values[3][1] * det8 + values[3][3] * det6;
            const Kfloat cof23 = values[3][0] * det9 - values[3][1] * det7 + values[3][2] * det6;
            const Kfloat cof30 = values[2][1] * det11 - values[2][2] * det10 + values[2][3] * det9;
            const Kfloat cof31 = values[2][0] * det11 - values[2][2] * det8 + values[2][3] * det7;
            const Kfloat cof32 = values[2][0] * det10 - values[2][1] * det8 + values[2][3] * det6;
            const Kfloat cof33 = values[2][0] * det9 - values[2][1] * det7 + values[2][2] * det6;

			//remember that adjoint matrix need transpose
            values[0][0] = cof00 / detM;
            values[0][1] = -cof10 / detM;
            values[0][2] = cof20 / detM;
            values[0][3] = -cof30 / detM;

            values[1][0] = -cof01 / detM;
            values[1][1] = cof11 / detM;
            values[1][2] = -cof21 / detM;
            values[1][3] = cof31 / detM;

            values[2][0] = cof02 / detM;
            values[2][1] = -cof12 / detM;
            values[2][2] = cof22 / detM;
            values[2][3] = -cof32 / detM;

            values[3][0] = -cof03 / detM;
            values[3][1] = cof13 / detM;
            values[3][2] = -cof23 / detM;
            values[3][3] = cof33 / detM;

            return *this;
        }
        template <typename F = Kdouble>
        F determinant()const {
            const F det0 = values[2][0] * values[3][1] - values[3][0] * values[2][1];
            const F det1 = values[2][0] * values[3][2] - values[3][0] * values[2][2];
            const F det2 = values[2][0] * values[3][3] - values[3][0] * values[2][3];
            const F det3 = values[2][1] * values[3][2] - values[3][1] * values[2][2];
            const F det4 = values[2][1] * values[3][3] - values[3][1] * values[2][3];
            const F det5 = values[2][2] * values[3][3] - values[3][2] * values[2][3];

            const F cof00 = values[1][1] * det5 - values[1][2] * det4 + values[1][3] * det3;
            const F cof01 = values[1][0] * det5 - values[1][2] * det2 + values[1][3] * det1;
            const F cof02 = values[1][0] * det4 - values[1][1] * det2 + values[1][3] * det0;
            const F cof03 = values[1][0] * det3 - values[1][1] * det1 + values[1][2] * det0;

            return values[0][0] * cof00 - values[0][1] * cof01 +
                    values[0][2] * cof02 - values[0][3] * cof03;
        }
        Mat3 toMat3()const {
            return Mat3(
                    values[0][0], values[0][1], values[0][2],
                    values[1][0], values[1][1], values[1][2],
                    values[2][0], values[2][1], values[2][2]
            );
        }
    };

    Mat4 operator-(const Mat4 &m){
        return Mat4(0) -= m;
    }
    Mat4 operator+(const Mat4 &m1, const Mat4 &m2){
        return Mat4(m1) += m2;
    }
    Mat4 operator-(const Mat4 &m1, const Mat4 &m2){
        return Mat4(m1) -= m2;
    }
    Mat4 operator*(const Mat4 &m1, const Mat4 &m2){
        return Mat4(m1) *= m2;
    }
    KVector::Vec4 operator*(const Mat4 &m, const KVector::Vec4 &v){
        return KVector::Vec4(
                KFunction::dot(v, m[0]),
                KFunction::dot(v, m[1]),
                KFunction::dot(v, m[2]),
                KFunction::dot(v, m[3])
        );
    }
    template <typename C>
    Mat4 operator*(const Mat4 &m, const C &c){
        return Mat4(m) *= c;
    }
    template <typename C>
    Mat4 operator*(const C &c, const Mat4 &m){
        return Mat4(m) *= c;
    }
    template <typename C>
    Mat4 operator/(const Mat4 &m, const C &c){
        return Mat4(m) /= c;
    }
    std::istream& operator>>(std::istream &is, Mat4 &m){
        is >> m[0] >> m[1] >> m[2] >> m[3];
        return is;
    }
    std::ostream& operator<<(std::ostream &os, const Mat4 &m){
        os << m[0] << '\n';
        os << m[1] << '\n';
        os << m[2] << '\n';
        os << m[3];
        return os;
    }
}

#endif //MAT4_H
