//
// Created by KingSun on 2018/5/1.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include "../Header.h"
#include "../render/Shader.h"
#include "../math/Vec3.h"
#include "../math/Mat4.h"
#include "../math/transform.h"
#include "../math/Quaternion.h"

namespace KCamera{
	class Camera {
		using tvec3 = KVector::Vec3;
		using tmat3 = KMatrix::Mat3;
		using tquaternion = KMatrix::Quaternion;
		using tmat4 = KMatrix::Mat4;

	protected:
        tvec3 position;
        tquaternion view; //view rotation
        tmat4 projection;
        tquaternion rotate; //camera rotation

		const static std::string EYE; //p_eye
		const static std::string VIEW; //u_view
		const static std::string PROJ; //u_proj

		tmat4 toViewMatrix()const {
			tmat3 tmp(view.toMat3());
			return tmat4(tmp, tmp * -position) *= rotate.toMat4();
		}

    public:
        explicit Camera(const tvec3 &pos = tvec3()): position(pos),
                 view(tquaternion()), rotate(tquaternion()){
			setOrtho(-1, 1, -1, 1, -1, 1);
		} //ortho
        Camera(const tvec3 &eye, const tvec3 &center, const tvec3 &up):
			rotate(tquaternion()) {
			setOrtho(-1, 1, -1, 1, -1, 1);
            setView(eye, center, up);
        }
        Camera(const Kfloat &fovy, const Kfloat &aspect,
               const Kfloat &zNear, const Kfloat &zFar,
               const tvec3 &pos = tvec3()):position(pos),
			view(tquaternion()), rotate(tquaternion()) {
            setPerspective(fovy, aspect, zNear, zFar);
        }
		virtual ~Camera() = default;

		void bindUniform(const KShader::Shader *shader)const {
			shader->bindUniform3f(EYE, rotate * position);
			shader->bindUniformMat4(VIEW, toViewMatrix());
			shader->bindUniformMat4(PROJ, projection);
		}

		void bindPosition(const KShader::Shader* shader)const {
			shader->bindUniform3f(EYE, rotate * position);
			shader->bindUniformMat4(VIEW, toViewMatrix());
		}

        void setPosition(const tvec3 &v){
            position = v;
        }
		void setRotation(const Kfloat& angle, const tvec3& axis) {
			rotate = tquaternion(-angle, axis);
		}
        void setView(const tvec3 &eye, const tvec3 &center, const tvec3 &up){
            //u-v-n is left-hand coordinate
            const tvec3 n((center - eye).normalize());
            const tvec3 u(tvec3::cross(n, up).normalize());
            const tvec3 v(tvec3::cross(u, n).normalize());

            position = eye;
            view = tquaternion().fromMatrix(tmat3(u, v, -n));
        }
        void setPerspective(const Kfloat &fovy, const Kfloat &aspect,
			                const Kfloat &zNear, const Kfloat &zFar){
            projection = KFunction::perspective(fovy, aspect, zNear, zFar);

        }
        void setOrtho(const Kfloat &left, const Kfloat &right, const Kfloat &bottom,
			          const Kfloat &top, const Kfloat &near, const Kfloat &far){
            projection = KFunction::ortho(left, right, bottom, top, near, far);
        }
        void setFrustum(const Kfloat &left, const Kfloat &right, const Kfloat &bottom,
                           const Kfloat &top, const Kfloat &near, const Kfloat &far){
            projection = KFunction::frustum(left, right, bottom, top, near, far);
        }

        void rotateCamera(const Kfloat &angle, const tvec3 &v){
            rotate *= tquaternion(-angle, v);
        }
        void rotateView(const Kfloat &angle, const tvec3 &v){
            //note: view is a inverse rotate matrix(also transpose matrix),
            //that means once you want to add rotate to it,
            //you should right multiply a transpose rotate matrix
            //(rot * originView).inverse = originView.inverse * rot.inverse = view * rot.transpose
            view *= tquaternion(-angle, v); //view *= tquaternion(angle, v).getConjugate();
        }

		void translate(const tvec3 &v) {
			position += v;
		}

#ifdef IMGUI_ENABLE
		void drawImGui() {
			//bind the value into ImGui and the you can change it.
			//Be sure to use it between ImGui::Begin() and ImGui::End();
			//Maybe the function will be private in the future.
			//Remember to rebind the value when you want to change the value in OpenGL.
			ImGui::SliderFloat3("camera", &position[0], -10, 10);
		}
#endif // IMGUI_ENABLE

    };

	const std::string Camera::EYE("p_eye");
	const std::string Camera::VIEW("u_view");
	const std::string Camera::PROJ("u_proj");
}

#endif CAMERA_H
