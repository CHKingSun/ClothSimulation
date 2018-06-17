//
// Created by KingSun on 2018/05/24
//

#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>
#include <vector>
#include "../Header.h"
#include "../util/Material.h"
#include "./Object3D.h"
#include "./Face.h"

namespace KObject {
	class Sphere : public Object3D {
		using tvec3 = KVector::Vec3;
		using tvec2 = KVector::Vec2;

	private:
		Kfloat radius;
		Ksize count;

		std::vector<tvec3>* vertices; //remember sphere's normal is its position when it center is (0, 0, 0)
		std::vector<tvec2>* texcoords;
		std::vector<Kushort>* indices; //Kushort can just store 65526 vertices(with 180 aslices and 180 rsclices)

		KMaterial::Material* material;

		void generate(Ksize aslices, Ksize rslices) {
			count = ((rslices - 1) * 4 + 1) * aslices + 1;
			Ksize v_count = (aslices + 1) * (rslices - 1) * 2 + 2;
			vertices = new std::vector<tvec3>();
			vertices->reserve(v_count);
			texcoords = new std::vector<tvec2>();
			texcoords->reserve(v_count);
			indices = new std::vector<Kushort>();
			indices->reserve(count);

			Kfloat per_xangle = PI * 2.0f / aslices;
			Kfloat per_yangle = PI / (rslices * 2);
			Kfloat pertx = 1.0f / aslices;
			Kfloat perty = 0.5f / rslices;
			Kfloat yangle = PI - per_yangle; //form bottom to top(except two poles).
			Kfloat ty = 1.0f - perty;
			for (int i = 0; i < ((rslices - 1) << 1); ++i, yangle -= per_yangle, ty -= perty) {
				Kfloat xangle = 0; //draw a circle
				Kfloat tx = 0;
				Kfloat y = radius * cos(yangle);
				Kfloat r = radius * sin(yangle);
				for (int j = 0; j <= aslices; ++j, xangle += per_xangle, tx += pertx) {
					vertices->emplace_back(cos(xangle) * r, y, sin(xangle) * r);
					texcoords->emplace_back(tx, ty);
				}
			}
			vertices->emplace_back(0, -radius, 0); //bottom pole [v_count - 2]
			texcoords->emplace_back(1, 1);
			vertices->emplace_back(0, radius, 0); //top pole [v_count - 1]
			texcoords->emplace_back(0, 0);

			int rt = aslices + 1;
			indices->emplace_back(v_count - 2);
			Ksize index = 0;
			for (Ksize at = 0; at < aslices; ++at, ++index) {
				for (int i = 0; i < ((rslices - 1) << 1); ++i, index += rt) {
					indices->emplace_back(index);
					indices->emplace_back(index + 1);
				}
				index -= rt;
				if (rt > 0) indices->emplace_back(v_count - 1);
				else indices->emplace_back(v_count - 2);
				rt = -rt;
			}
		}

		void initArray() {
			vao = new KBuffer::VertexArray();

			vbo = new KBuffer::VertexBuffer(vertices->size() * sizeof(tvec3), vertices->data());
			vao->allocate(vbo, A_POSITION, 3, GL_FLOAT);

			tbo = new KBuffer::VertexBuffer(texcoords->size() * sizeof(tvec2), texcoords->data());
			vao->allocate(tbo, A_TEXCOORD, 2, GL_FLOAT);

			vao->allocate(vbo, A_NORMAL, 3, GL_FLOAT);

			ibo = new KBuffer::VertexBuffer(indices->size() * sizeof(Kushort),
				indices->data(), KBuffer::INDEX);

			delete vertices; vertices = nullptr;
			delete texcoords; texcoords = nullptr;
			delete indices; indices = nullptr;
		}

	public:
		Sphere(Kfloat radius = 1.0f, Ksize aslices = 20, Ksize rslices = 20):
			Object3D("sphere"), radius(radius), vertices(nullptr), texcoords(nullptr),
			indices(nullptr), material(nullptr) {
			material = new KMaterial::Material(RES_PATH + "earth.jpg");
			generate(aslices, rslices);
			initArray();
		}

		~Sphere()override {
			delete vertices;
			delete texcoords;
			delete indices;
			delete material;
		}

		void bindUniform(const KShader::Shader* shader)const override {
			Object3D::bindUniform(shader);
			material->bindUniform(shader);
		}

		void unActiveTexture(const KShader::Shader* shader)const {
			material->unactiveTexture(shader);
		}

		void render()const override {
			bind();

			glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, nullptr);

			unBind();
		}

		Kfloat getRadius()const {
			return radius;
		}
	};
}

#endif //SPHERE_H
