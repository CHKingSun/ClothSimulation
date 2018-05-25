//
// Created by KingSun on 2018/4/23.
//

#ifndef MAT3_H
#define MAT3_H

#include <cassert>
#include <iosfwd>
#include "../Header.h"
#include "./Vec3.h"

namespace KMatrix{
    class Mat3{
        using Row = KVector::Vec3;

    private:
        Row values[3]; //row-first

    public:
        Mat3():Mat3(static_cast<Kfloat>(1)){}
        Mat3(const Mat3 &m){
            this->values[0] = m[0];
            this->values[1] = m[1];
            this->values[2] = m[2];
        }
        explicit Mat3(const Kfloat &c){
            this->values[0] = Row(c, 0, 0);
            this->values[1] = Row(0, c, 0);
            this->values[2] = Row(0, 0, c);
        }
        Mat3(const Row &v0, const Row &v1, const Row &v2){
            this->values[0] = v0;
            this->values[1] = v1;
            this->values[2] = v2;
        }
        Mat3(const Kfloat &x0, const Kfloat y0, const Kfloat &z0,
                const Kfloat &x1, const Kfloat &y1, const Kfloat &z1,
                const Kfloat &x2, const Kfloat &y2, const Kfloat &z2){
            values[0] = Row(x0, y0, z0);
            values[1] = Row(x1, y1, z1);
            values[2] = Row(x2, y2, z2);
        }

        const Kfloat* data()const {
            return &this->values[0][0];
        }

        Row& operator[](Kuint n){
            assert(n >= 0 && n < 3);
            return this->values[n];
        }
        const Row& operator[](Kuint n)const {
            assert(n >= 0 && n < 3);
            return this->values[n];
        }

        Mat3& operator=(const Mat3 &m){
            this->values[0] = m[0];
            this->values[1] = m[1];
            this->values[2] = m[2];
            return *this;
        }
        Mat3& operator+=(const Mat3 &m){
            this->values[0] += m[0];
            this->values[1] += m[1];
            this->values[2] += m[2];
            return *this;
        }
        Mat3& operator-=(const Mat3 &m){
            this->values[0] -= m[0];
            this->values[1] -= m[1];
            this->values[2] -= m[2];
            return *this;
        }
        Mat3& operator*=(const Mat3 &m){
            //matrix multiply. m * n != n * m;
            /**
                * Mat4 row(*this);
                * Mat4 col(m).transpose();
                * for(int i = 0; i < 3; ++i){
                *      for(int j = 0; j < 3; ++j){
                *          this->values[i][j] = row[i] * col[j];
                *      }
                * }
                */

            Row row0(this->values[0]);
            Row row1(this->values[1]);
            Row row2(this->values[2]);

            Row col0(m[0][0], m[1][0], m[2][0]);
            Row col1(m[0][1], m[1][1], m[2][1]);
            Row col2(m[0][2], m[1][2], m[2][2]);

            this->values[0][0] = KFunction::dot(row0, col0);
            this->values[0][1] = KFunction::dot(row0, col1);
            this->values[0][2] = KFunction::dot(row0, col2);

            this->values[1][0] = KFunction::dot(row1, col0);
            this->values[1][1] = KFunction::dot(row1, col1);
            this->values[1][2] = KFunction::dot(row1, col2);

            this->values[2][0] = KFunction::dot(row2, col0);
            this->values[2][1] = KFunction::dot(row2, col1);
            this->values[2][2] = KFunction::dot(row2, col2);

            return *this;
        }

        template <typename C>
        Mat3& operator*=(const C &c){
            this->values[0] *= c;
            this->values[1] *= c;
            this->values[2] *= c;
            return *this;
        }
        template <typename C>
        Mat3& operator/=(const C &c){
            this->values[0] /= c;
            this->values[1] /= c;
            this->values[2] /= c;
            return *this;
        }

        Mat3& transpose(){
            for(int i = 0; i < 3; ++i){
                for(int j = i + 1; j < 3; ++j){
                    Kfloat t = this->values[i][j];
                    this->values[i][j] = this->values[j][i];
                    this->values[j][i] = t;
                }
            }
            return *this;
        }
        Mat3& inverse(){
            const Kfloat cof00 = values[1][1] * values[2][2] - values[2][1] * values[1][2];
            const Kfloat cof01 = -(values[1][0] * values[2][2] - values[2][0] * values[1][2]);
            const Kfloat cof02 = values[1][0] * values[2][1] - values[2][0] * values[1][1];

            const Kfloat detM = values[0][0] * cof00 + values[0][1] * cof01 + values[0][2] * cof02;
            if(fabs(detM) <= EPSILON_E6){
                for (auto &value : values)
                    for(int j = 0; j < 3; ++j)
                        value[j] = static_cast<Kfloat>(KNAN);
                return *this;
            }

            const Kfloat cof10 = -(values[0][1] * values[2][2] - values[2][1] * values[0][2]);
            const Kfloat cof11 = values[0][0] * values[2][2] - values[2][0] * values[0][2];
            const Kfloat cof12 = -(values[0][0] * values[2][1] - values[2][0] * values[0][1]);

            const Kfloat cof20 = values[0][1] * values[1][2] - values[1][1] * values[0][2];
            const Kfloat cof21 = -(values[0][0] * values[1][2] - values[1][0] * values[0][2]);
            const Kfloat cof22 = values[0][0] * values[1][1] - values[1][0] * values[0][1];

            //remember transposing matrix
            values[0][0] = cof00 / detM;
            values[0][1] = cof10 / detM;
            values[0][2] = cof20 / detM;

            values[1][0] = cof01 / detM;
            values[1][1] = cof11 / detM;
            values[1][2] = cof21 / detM;

            values[2][0] = cof02 / detM;
            values[2][1] = cof12 / detM;
            values[2][2] = cof22 / detM;

            return *this;
        }
        template <typename F = Kdouble>
        F determinant()const {
            const F cof00 = values[1][1] * values[2][2] - values[2][1] * values[1][2];
            const F cof01 = -(values[1][0] * values[2][2] - values[2][0] * values[1][2]);
            const F cof02 = values[1][0] * values[2][1] - values[2][0] * values[1][1];

            return values[0][0] * cof00 + values[0][1] * cof01 + values[0][2] * cof02;
        }
    };

    Mat3 operator-(const Mat3 &m){
        return Mat3(0) -= m;
    }
    Mat3 operator+(const Mat3 &m1, const Mat3 &m2){
        return Mat3(m1) += m2;
    }
    Mat3 operator-(const Mat3 &m1, const Mat3 &m2){
        return Mat3(m1) -= m2;
    }
    Mat3 operator*(const Mat3 &m1, const Mat3 &m2){
        return Mat3(m1) *= m2;
    }
    KVector::Vec3 operator*(const Mat3 &m, const KVector::Vec3 &v){
        return KVector::Vec3(
                KFunction::dot(v, m[0]),
                KFunction::dot(v, m[1]),
                KFunction::dot(v, m[2])
        );
    }
    template <typename C>
    Mat3 operator*(const Mat3 &m1, const C &c){
        return Mat3(m1) *= c;
    }
    template <typename C>
    Mat3 operator*(const C &c, const Mat3 &m1){
        return Mat3(m1) *= c;
    }
    template <typename C>
    Mat3 operator/(const Mat3 &m1, const C &c){
        return Mat3(m1) /= c;
    }
    std::istream& operator>>(std::istream &is, Mat3 &m){
        is >> m[0] >> m[1] >> m[2];
        return is;
    }
    std::ostream& operator<<(std::ostream &os, const Mat3 &m){
        os << m[0] << '\n';
        os << m[1] << '\n';
        os << m[2];
        return os;
    }
}

#endif //MAT3_H
