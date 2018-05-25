//
// Created by KingSun on 2018/05/11
//

#ifndef OBJECT3D_H
#define OBJECT3D_H

//大部分情况我们以物体本身作为中心进行缩放平移旋转
//Nmatrix = mat3(modelMatrix)

#include "../Header.h"
#include "../math/Vec3.h"
#include "../math/Quaternion.h"
#include "../render/Shader.h"
#include "../render/VertexArray.h"
#include "../render/VertexBuffer.h"

namespace KObject {
	class Object3D {
	protected :
		KVector::Vec3 position;
		KMatrix::Quaternion rotation;
		KVector::Vec3 m_scale;

		KBuffer::VertexArray* vao;
		KBuffer::VertexBuffer* ibo;

		KBuffer::VertexBuffer* vbo;
		KBuffer::VertexBuffer* tbo;
		KBuffer::VertexBuffer* nbo;

		std::string type;

		const static std::string U_POSITION;
		const static std::string U_ROTATION;
		const static std::string U_SCALE;

		const static Kint A_POSITION; // = 1;
		const static Kint A_NORMAL; // = 2;
		const static Kint A_TEXCOORD; // = 3;

	protected:
		Object3D(std::string type, const KVector::Vec3& pos = KVector::Vec3()) :
			type(type), vao(nullptr), ibo(nullptr), position(pos),
			rotation(KMatrix::Quaternion()), m_scale(KVector::Vec3(1.0f)),
			vbo(nullptr), tbo(nullptr), nbo(nullptr) {}

	public:
		virtual ~Object3D() {
			std::cout << type << std::endl;
			delete vao;
			delete ibo;
			delete vbo;
			delete tbo;
			delete nbo;
		}

		const std::string& getType()const {
			return type;
		}

		void bind()const {
			if (vao != nullptr) {
				vao->bind();
				vao->enableVertexArray();
			}
			if(ibo != nullptr) ibo->bind();
		}

		void unBind()const {
			if (vao != nullptr) {
				vao->unBind();
				vao->disableVertexArray();
			}
			if(ibo != nullptr) ibo->unBind();
		}

		void setPosition(const KVector::Vec3& v) {
			position = v;
		}

		void setRotation(Kfloat angle, const KVector::Vec3& v) {
			rotation = KMatrix::Quaternion(angle, v);
		}

		void setScale(const KVector::Vec3& v) {
			m_scale = v;
		}

		void translate(const KVector::Vec3& v) {
			position += v;
		}

		void rotate(Kfloat angle, const KVector::Vec3& v) {
			rotation *= KMatrix::Quaternion(angle, v);
		}

		void scale(const KVector::Vec3 &v) {
			m_scale *= v;
		}

		virtual void bindUniform(const KShader::Shader* shader)const {
			shader->bindUniform3f(U_POSITION, position);
			shader->bindUniformMat3(U_ROTATION, rotation.toMat3());
			shader->bindUniform3f(U_SCALE, m_scale);
		}

		void bindPosition(const KShader::Shader* shader)const {
			shader->bindUniform3f(U_POSITION, position);
		}

		void bindRotation(const KShader::Shader* shader)const {
			shader->bindUniformMat3(U_ROTATION, rotation.toMat3());
		}

		void bindScale(const KShader::Shader* shader)const {
			shader->bindUniform3f(U_SCALE, m_scale);
		}

		virtual void render()const = 0;
		virtual Kuint getCount()const = 0;

#ifdef IMGUI_ENABLE
		virtual void drawImGui() {
			//bind the value into ImGui and the you can change it.
			//Be sure to use it between ImGui::Begin() and ImGui::End();
			//Maybe the function will be private in the future.
			//Remember to rebind the value when you want to change the value in OpenGL.
			ImGui::SliderFloat3("position", &position[0], -10, 10);
			ImGui::SliderFloat3("scale", &m_scale[0], -10, 10);
		}
#endif // IMGUI_ENABLE

	};

	const std::string Object3D::U_POSITION("u_mPos");
	const std::string Object3D::U_ROTATION("u_mRotate");
	const std::string Object3D::U_SCALE("u_mScale");

	const Kint Object3D::A_POSITION = 1;
	const Kint Object3D::A_NORMAL = 2;
	const Kint Object3D::A_TEXCOORD = 3;
}

#endif //OBJECT3D_H
