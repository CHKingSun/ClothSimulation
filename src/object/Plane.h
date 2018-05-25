//
// Created by KingSun 0n 2018/05/11
//

#ifndef PLANE_H
#define KENGINE_PLANE_H

#include <vector>
#include "../Header.h"
#include "./Object3D.h"
#include "./Face.h"
#include "../math/Vec3.h"
#include "../math/Vec2.h"
#include "../util/Material.h"

namespace KObject {
	using tvec2 = KVector::Vec2;
	using tvec3 = KVector::Vec3;

	class Plane : public Object3D {
	private:
		std::vector<tvec2>* vertices; //left z to default(0)
		std::vector<tvec2>* tex_coords;
		std::vector<Face<Kuint>>* indices;
		Kuint count;
		KMaterial::Material* material;

		void generate(Kfloat width, Kfloat height, Kuint xslices, Kuint yslices) {
			Kfloat px = -width / 2.0f;
			Kfloat py = -height / 2.0f;
			Kfloat perx = width / xslices;
			Kfloat pery = height / yslices;
			for (int i = 0; i < xslices; ++i) {
				for (int j = 0; j < yslices; ++j) {
					Kfloat x = px + perx * i;
					Kfloat y = py + pery * j;

					vertices->emplace_back(x, y);
					vertices->emplace_back(x + perx, y);
					vertices->emplace_back(x + perx, y + pery);
					vertices->emplace_back(x, y + pery);

					tex_coords->emplace_back(0.0f, 0.0f);
					tex_coords->emplace_back(1.0f, 0.0f);
					tex_coords->emplace_back(1.0f, 1.0f);
					tex_coords->emplace_back(0.0f, 1.0f);

					Kuint t = (i * yslices + j) * 4;
					indices->emplace_back(t, t + 1, t + 2);
					indices->emplace_back(t + 2, t + 3, t);
				}
			}
		}

		void initArray() {
			vao = new KBuffer::VertexArray();

			vbo = new KBuffer::VertexBuffer(vertices->data(),
				sizeof(tvec2) * vertices->size());
			vao->allocate(vbo, A_POSITION, 2, GL_FLOAT);

			tbo = new KBuffer::VertexBuffer(tex_coords->data(),
				sizeof(tvec2) * tex_coords->size());
			vao->allocate(tbo, A_TEXCOORD, 2, GL_FLOAT);

			vao->setVertexAttrib3f(A_NORMAL, tvec3(0.0f, 0.0f, 1.0f));

			ibo = new KBuffer::VertexBuffer(indices->data(),
				sizeof(Face<Kuint>) * indices->size(), KBuffer::BufferType::INDEX);

			delete vertices; vertices = nullptr;
			delete tex_coords; tex_coords = nullptr;
			delete indices; indices = nullptr;
		}

	public:
		Plane(Kfloat width = 1.0f, Kfloat height = 1.0f,
			Ksize xslices = 1, Ksize yslices = 1): Object3D("Plane"),
			vertices(nullptr), tex_coords(nullptr), indices(nullptr) {
			count = xslices * yslices;
			vertices = new std::vector<tvec2>();
			vertices->reserve(count * 4);
			tex_coords = new std::vector<tvec2>();
			tex_coords->reserve(count * 4);
			indices = new std::vector<Face<Kuint>>();
			indices->reserve(count * 2);
			material = new KMaterial::Material();
			material->setTexture(RES_PATH + "stone.png");

			generate(width, height, xslices, yslices);
			initArray();
		}
		~Plane()override {
			delete material;
			delete vertices;
			delete tex_coords;
			delete indices;
		}

		void setMaterial(KMaterial::Material* material) {
			if (material != nullptr) {
				delete this->material;
				this->material = material;
			}
		}

		void bindUniform(const KShader::Shader* shader)const override {
			Object3D::bindUniform(shader);
			material->bindUniform(shader);
		}

		void render()const override {
			bind();

			glDrawElements(GL_TRIANGLES, count * 6, GL_UNSIGNED_INT, nullptr);

			unBind();
		}

		Ksize getCount()const override {
			return count * 6;
		}
	};
}

#endif //PLANE_H
