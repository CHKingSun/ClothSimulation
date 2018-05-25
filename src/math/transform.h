//
// Created by KingSun on 2018/4/23.
//

#ifndef TRANSFORM_H
#define KENGINE_TRANSFORM_H

//translate rotate scale
//lookAt perspective ortho frustum
//project unproject(vec3 obj, mat4 model, mat4 proj, vec4 viewport)

#include "../Header.h"
#include "./function.h"
#include "./Mat4.h"

namespace KFunction{
    using namespace KVector;
    using namespace KMatrix;

    Mat4 translate(const Vec3 &v){
        return Mat4(
                1, 0, 0, v.x,
                0, 1, 0, v.y,
                0, 0, 1, v.z,
                0, 0, 0, 1
        );
    }

    Mat4 rotate(const Kfloat &angle, const Vec3 &v){
        const auto ang = toRadian<Kfloat>(angle);
        const auto cosA = static_cast<Kfloat>(cos(ang));
        const auto sinA = static_cast<Kfloat>(sin(ang));

        Vec3 axis(normalize(v));
        Vec3 tmp((static_cast<Kfloat>(1) - cosA) * axis);
        Mat4 m;

        m[0][0] = cosA + axis.x * tmp.x;
        m[0][1] = axis.x * tmp.y - axis.z * sinA;
        m[0][2] = axis.x * tmp.z + axis.y * sinA;

        m[1][0] = axis.y * tmp.x + axis.z * sinA;
        m[1][1] = cosA + axis.y * tmp.y;
        m[1][2] = axis.y * tmp.z - axis.x * sinA;

        m[2][0] = axis.z * tmp.x - axis.y * sinA;
        m[2][1] = axis.z * tmp.y + axis.x * sinA;
        m[2][2] = cosA + axis.z * tmp.z;

        return m;
    }

    Mat4 scale(const Vec3 &v){
        return Mat4(
                v.x, 0, 0, 0,
                0, v.y, 0, 0,
                0, 0, v.z, 0,
                0, 0, 0, 1
        );
    }

    Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up){
        //if eye == center or up = 0, the matrix will be nan
        //u-v-n is left-hand coordinate
        const Vec3 n((center - eye).normalize());
        const Vec3 u(Vec3::cross(n, up).normalize());
        const Vec3 v(Vec3::cross(u, n).normalize());

        return Mat4(
                u.x, u.y, u.z, -dot<Vec3, Kfloat>(u, eye),
                v.x, v.y, v.z, -dot<Vec3, Kfloat>(v, eye),
                -n.x, -n.y, -n.z, dot<Vec3, Kfloat>(n, eye),
                0, 0, 0, 1
        );
    }

	Mat4 ortho(const Kfloat &left, const Kfloat &right, const Kfloat &bottom,
		const Kfloat &top, const Kfloat &near, const Kfloat &far) {
		Mat4 m;

		m[0][0] = static_cast<Kfloat>(2) / (right - left);
		m[1][1] = static_cast<Kfloat>(2) / (top - bottom);
		m[2][2] = static_cast<Kfloat>(-2) / (far - near);

		m[0][3] = -(right + left) / (right - left);
		m[1][3] = -(top + bottom) / (top - bottom);
		m[2][3] = -(far + near) / (far - near);

		return m;
	}

	Mat4 frustum(const Kfloat &left, const Kfloat &right, const Kfloat &bottom,
		const Kfloat &top, const Kfloat &near, const Kfloat &far) {
		Mat4 m(0);

		m[0][0] = static_cast<Kfloat>(2) * near / (right - left);
		m[1][1] = static_cast<Kfloat>(2) * near / (top - bottom);
		m[0][2] = -(right + left) / (right - left);
		m[1][2] = -(top + bottom) / (top - bottom);

		m[2][2] = -(far + near) / (far - near);
		m[2][3] = static_cast<Kfloat>(-2) * far * near / (far - near);
		m[3][2] = static_cast<Kfloat>(-1);

		return m;
	}

	Mat4 perspective(const Kfloat &fovy, const Kfloat &aspect, const Kfloat &zNear, const Kfloat &zFar) {
		assert((aspect > static_cast<Kfloat>(EPSILON_E6)) &&
			((zFar - zNear) > static_cast<Kfloat>(EPSILON_E6)) &&
			fovy > 0 && fovy < 180.0);

		const auto cot = static_cast<Kfloat>(1.0 / tan(toRadian(fovy / 2.0)));
		Mat4 m(0);

		m[0][0] = cot / aspect;
		m[1][1] = cot;
		m[2][2] = -(zFar + zNear) / (zFar - zNear);
		m[2][3] = static_cast<Kfloat>(-2) * zFar * zNear / (zFar - zNear);
		m[3][2] = static_cast<Kfloat>(-1);

		return m;
	}

    Vec3 project(const Vec3 &v, const Mat4 &model, const Mat4 &proj, const Vec4 &viewport){
        Vec4 pos = proj * (model * Vec4(v, static_cast<Kfloat>(1))); //裁剪坐标系
        pos /= pos.w; //转换到[-1, 1]
        pos /= static_cast<Kfloat>(2); //转换到[-0.5, 0.5]
        pos += static_cast<Kfloat>(0.5); //转换到[0, 1]

        pos.x = pos.x * Kfloat(viewport[2]) + Kfloat(viewport[0]); //转换到[v[0], v[2]]
        pos.y = pos.y * Kfloat(viewport[3]) + Kfloat(viewport[1]); //转换到[v[1], v[3]]

        return Vec3(pos.x, pos.y, pos.z);
    };

    Vec3 unProject(const Vec3 &v, const Mat4 &model, const Mat4 &proj, const Vec4 &viewport){
        Vec4 pos = Vec4(v, static_cast<Kfloat>(1));
        pos.x = (pos.x - viewport[0]) / viewport[2]; //转换到[0, 1]
        pos.y = (pos.y - viewport[1]) / viewport[3]; //转换到[0, 1]
        pos *= static_cast<Kfloat>(2); //[0, 2]
        pos -= static_cast<Kfloat>(1); //[-1, 1]

        pos = (proj * model).inverse() * pos;
        pos /= pos.w;

        return Vec3(pos.x, pos.y, pos.z);
    };
}

#endif //TRANSFORM_H
