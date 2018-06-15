//
// Created by KingSun on 2018/06/06
//

#ifndef EULER_CLOTH_H
#define EULER_CLOTH_H

#include "../math/Vec4.h"
#include "./Object3D.h"
#include "../util/Material.h"
#include "../render/BackBuffer.h"
#include "../render/TextureBuffer.h"

namespace KObject {
	using tvec2 = KVector::Vec2;
	using tvec3 = KVector::Vec3;
	using tvec4 = KVector::Vec4;

	//Use GPU and Euler mathod
	class EulerCloth : public Object3D {
	private:
		const Kfloat length = 9.6f;
		Ksize size;
		Ksize count;

		const Kfloat a_resistance = -0.0125f;
		const tvec3 f_wind = tvec3(0.f);
		
		const Kfloat mass = 0.1f;
		const Kfloat ks = 15.f;
		const Kfloat kd = 0.96f;
		const Kfloat ks_bend = 0.036f;
		const Kfloat kd_bend = 0.96f;

		const Kfloat delta_time = 1.f / 60.f;

		Kfloat rest_length; //length / size
		Kfloat diag_length; //rest_length * sqrt(2)

		KBuffer::BackBuffer* back_buffer;
		KBuffer::TextureBuffer* constraints_sampler;
		KBuffer::TextureBuffer* vertices_sampler;
		KBuffer::TextureBuffer* velocities_sampler;

		std::vector<tvec3>* vertices; //It will be deleted in CLoth class
		std::vector<tvec2>* texcoords;
		std::vector<Kuint>* indices;
		std::vector<tvec3>* normals;

		KMaterial::Material* material;

		void generate() {
			vertices = new std::vector<tvec3>();
			vertices->reserve(size * size);
			texcoords = new std::vector<tvec2>();
			texcoords->reserve(size * size);
			//normals = new std::vector<tvec3>();
			//normals->reserve(size * size);

			rest_length = length / size;
			diag_length = rest_length * sqrt(2);
			Kfloat pertex = 1.f / size;
			//Kfloat y = length;
			Kfloat y = length / 2.f;
			Kfloat ty = 0.f;
			for (int i = 0; i < size; ++i, y -= rest_length, ty += pertex) {
				Kfloat x = -length / 2.f;
				Kfloat tx = 0.f;
				for (int j = 0; j < size; ++j, x += rest_length, tx += pertex) {
					//vertices->emplace_back(x, y, 0.f);
					vertices->emplace_back(x, length, y);
					texcoords->emplace_back(tx, ty);
				}
			}
			
			vertices_sampler = new KBuffer::TextureBuffer(vertices->size() * sizeof(tvec3),
				vertices->data());
			velocities_sampler = new KBuffer::TextureBuffer(vertices->size() * sizeof(tvec3));

			auto constraints = new Kubyte[size * size];
			memset(constraints, 0, size * size * sizeof(Kubyte));
			for (int i = 0; i < size; ++i) {
				constraints[i] = true;
			}
			constraints[0] = true;
			constraints[size - 1] = true;
			constraints_sampler = new KBuffer::TextureBuffer(size * size * sizeof(Kubyte), constraints, GL_RGBA8UI);
			delete constraints;

			indices = new std::vector<Kuint>();
#define PRIMITIVE
#ifdef PRIMITIVE
			count = (size - 1) * (size * 2 + 1) - 1;
			indices->reserve(count + 1);
			Kuint index = 0;
			for (int i = 0; i < size - 1; ++i) {
				indices->emplace_back(index);
				for (int j = 0; j < size - 1; ++j) {
					indices->emplace_back(index + size);
					indices->emplace_back(++index);
				}
				indices->emplace_back(index + size);
				++index;
				indices->emplace_back(0XFFFFFFFF);
			}
#else
			//It will create other triangles.
			count = (2 * (size - 1) + 1) * (size - 1) + 1;
			indices->reserve(count);
			Kuint index = 0;
			Kint tmp = 1;
			indices->emplace_back(index);
			for (Kuint i = 0; i < size - 1; ++i) {
				for (int j = 0; j < size - 1; ++j) {
					indices->emplace_back(index + size);
					indices->emplace_back(index += tmp);
				}
				indices->emplace_back(index += size);
				tmp = -tmp;
			}
#endif
		}

		void initArray() {
			vao = new KBuffer::VertexArray();

			vbo = new KBuffer::VertexBuffer(vertices->size() * sizeof(tvec3));
			//we will add data by transform feed back
			vao->allocate(vbo, A_POSITION, 3, GL_FLOAT, false, sizeof(tvec3));

			tbo = new KBuffer::VertexBuffer(texcoords->size() * sizeof(tvec2), texcoords->data());
			vao->allocate(tbo, A_TEXCOORD, 2, GL_FLOAT);

			//nbo = new KBuffer::VertexBuffer(normals->size() * sizeof(tvec3), normals->data());
			//vao->allocate(nbo, A_NORMAL, 3, GL_FLOAT);

			ibo = new KBuffer::VertexBuffer(count * sizeof(Ksize), indices->data(), KBuffer::INDEX);

			delete texcoords; texcoords = nullptr;
			delete indices; indices = nullptr;
		}

	public:
		EulerCloth(Ksize size = 30): Object3D("Cloth"), size(size),
			back_buffer(nullptr), vertices_sampler(nullptr),
			constraints_sampler(nullptr), velocities_sampler(nullptr),
			vertices(nullptr), texcoords(nullptr), normals(nullptr),
			indices(nullptr), material(nullptr) {
			material = new KMaterial::Material();
			material->ambient = tvec4(0.f, 0.67f, 0.56f, 1.f);
			material->diffuse = tvec4(0.41f, 0.69f, 0.67f, 1.f);
			material->specular = tvec4(0.40f, 0.73f, 0.72f, 1.f);
			material->shininess = 3.0;

			generate();
			initArray();
		}
		~EulerCloth()override {
			delete vertices;
			delete texcoords;
			delete indices;
			delete normals;
			delete material;

			delete back_buffer;
			delete constraints_sampler;
			delete vertices_sampler;
			delete velocities_sampler;
		}

		void bindUniform(const KShader::Shader* shader)const override {
			Object3D::bindUniform(shader);
			material->bindUniform(shader);
		}

		void initBackBuffer(const KShader::Shader* back_shader) {
			back_buffer = new KBuffer::BackBuffer(back_shader,
			{
				"o_vertex",
				"o_velocity"
			},
			{
				size * size * sizeof(tvec3),
				size * size * sizeof(tvec3)
			},
			GL_SEPARATE_ATTRIBS);

			//vbo->bindToBackBuffer(0, back_buffer);
			//vertices_sampler->bindToBackBuffer(0, back_buffer);
			//velocities_sampler->bindToBackBuffer(1, back_buffer);
		}

		void bindBackUniform(const KShader::Shader* back_shader)const {
			back_shader->bindUniform1i("size", size);

			back_shader->bindUniform1f("mass", mass);
			back_shader->bindUniform1f("a_resistance", a_resistance);
			back_shader->bindUniform3f("f_wind", f_wind);
			
			back_shader->bindUniform1f("ks", ks);
			back_shader->bindUniform1f("kd", kd);
			back_shader->bindUniform1f("ks_bend", ks_bend);
			back_shader->bindUniform1f("kd_bend", kd_bend);
			back_shader->bindUniform1f("delta_time", delta_time);

			back_shader->bindUniform1f("rest_length", rest_length);
			back_shader->bindUniform1f("diag_length", diag_length);

			back_shader->bindUniform3f("u_position", position);

			constraints_sampler->bind(back_shader, "constraints_tbo", 0);
			vertices_sampler->bind(back_shader, "vertices_tbo", 1);
			velocities_sampler->bind(back_shader, "velocities_tbo", 2);
		}

		void renderBack()const {
			//remember to bind uniform and delta time before.
			glEnable(GL_RASTERIZER_DISCARD);
			back_buffer->enable();

			glDrawArrays(GL_POINTS, 0, size * size);

			back_buffer->disable();
			glDisable(GL_RASTERIZER_DISCARD);

			vbo->copyDataFormBuffer(0, back_buffer);
			vertices_sampler->copyDataFromBuffer(0, back_buffer);
			velocities_sampler->copyDataFromBuffer(1, back_buffer);

			//const tvec3* data0 = back_buffer->getData<tvec3>(0);
			//const tvec3* data1 = back_buffer->getData<tvec3>(1);
		}

		void render()const override {
			bind();

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#ifdef PRIMITIVE
			glEnable(GL_PRIMITIVE_RESTART);
			glPrimitiveRestartIndex(0XFFFFFFFF);
			glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, nullptr);
			glDisable(GL_PRIMITIVE_RESTART);
#else
			glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, nullptr);
#endif
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			unBind();
		}
	};
}

#endif // !EULER_CLOTH_H

