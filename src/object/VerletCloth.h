//
// Created by KingSun on 2018/06/06
//

#ifndef VERLET_CLOTH_H
#define VERLET_CLOTH_H

#include "../math/Vec4.h"
#include "./Object3D.h"
#include "../util/Material.h"
#include "../render/BackBuffer.h"
#include "../render/TextureBuffer.h"

namespace KObject {
	using tvec2 = KVector::Vec2;
	using tvec3 = KVector::Vec3;
	using tvec4 = KVector::Vec4;

	//Use GPU and Verlet mathod
	class VerletCloth : public Object3D {
	private:
		tvec2 length = tvec2(9.6f);
		Ksize size_x, size_y;
		Ksize count;

		const Kfloat a_resistance = -0.0125f;
		const tvec3 f_wind = tvec3(0.1f, 0.f, -0.02f);

		const Kfloat mass = 0.1f;
		const Kfloat ks = 100.f;
		const Kfloat kd = 0.48f;
		const Kfloat ks_bend = 3.6f;
		const Kfloat kd_bend = 0.48f;

		const Kfloat delta_time = 1.f / 60.f;
		const Kfloat last_dt = 1.f / 60.f;

		tvec2 rest_length = tvec2(1.f); //length / size
		Kfloat diag_length = rest_length.length(); //rest_length.length()

		KBuffer::BackBuffer* back_buffer;
		KBuffer::TextureBuffer* constraints_sampler;
		KBuffer::TextureBuffer* vertices_sampler;
		KBuffer::TextureBuffer* last_vertices_sampler;

		std::vector<tvec3>* vertices; //It will be deleted in CLoth class
		std::vector<tvec2>* texcoords;
		std::vector<Kuint>* indices;
		std::vector<tvec3>* normals;

		KMaterial::Material* material;

		void generate() {
			vertices = new std::vector<tvec3>();
			vertices->reserve(size_x * size_y);
			texcoords = new std::vector<tvec2>();
			texcoords->reserve(size_x * size_y);
			//normals = new std::vector<tvec3>();
			//normals->reserve(size_x * size_y);

			rest_length = length / tvec2(size_x - 1, size_y - 1);
			diag_length = rest_length.length();

			Kfloat pertex_x = 1.f / (size_x - 1);
			Kfloat pertex_y = 1.f / (size_y - 1);
			//Kfloat y = length.y;
			Kfloat y = length.y / 2.f;
			Kfloat ty = 0.f;
			for (int i = 0; i < size_y; ++i, y -= rest_length.y, ty += pertex_y) {
				Kfloat x = length.x / -2.f;
				Kfloat tx = 0.f;
				for (int j = 0; j < size_x; ++j, x += rest_length.x, tx += pertex_x) {
					//vertices->emplace_back(x, y, 0.f);
					vertices->emplace_back(x, length.y, y);
					texcoords->emplace_back(tx, ty);
				}
			}

			//m_scale = tvec3(length.x / (size_x - 1), length.y / (size_y - 1), 1.f);
			//m_scale = tvec3(length.x / (size_x - 1), 1.f, length.y / (size_y - 1));

			vertices_sampler = new KBuffer::TextureBuffer(vertices->size() * sizeof(tvec3),
				vertices->data());
			last_vertices_sampler = new KBuffer::TextureBuffer(vertices->size() * sizeof(tvec3),
				vertices->data());

			auto constraints = new Kubyte[size_x * size_y];
			memset(constraints, 0, size_x * size_y * sizeof(Kubyte));
			for (int i = 0; i < size_x; ++i) {
				constraints[i] = true;
			}
			constraints[0] = true;
			constraints[size_x - 1] = true;
			constraints_sampler = new KBuffer::TextureBuffer(size_x * size_y * sizeof(Kubyte), constraints, GL_RGBA8UI);
			delete constraints;

			indices = new std::vector<Kuint>();
			count = (size_y - 1) * (size_x * 2 + 1) - 1;
			indices->reserve(count + 1);
			Kuint index = 0;
			for (int i = 0; i < size_y - 1; ++i) {
				for (int j = 0; j < size_x; ++j, ++index) {
					indices->emplace_back(index);
					indices->emplace_back(index + size_x);
				}
				indices->emplace_back(0XFFFFFFFF);
			}
		}

		void initArray() {
			vao = new KBuffer::VertexArray();

			vbo = new KBuffer::VertexBuffer(vertices->size() * sizeof(tvec3), vertices->data());
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
		VerletCloth(Ksize xslices = 30, Kfloat yslices = 20):
			Object3D("Cloth"), size_x(xslices + 1), size_y(yslices + 1),
			back_buffer(nullptr), vertices_sampler(nullptr),
			constraints_sampler(nullptr), last_vertices_sampler(nullptr),
			vertices(nullptr), texcoords(nullptr), normals(nullptr),
			indices(nullptr), material(nullptr) {
			material = new KMaterial::Material();
			//material->ambient = tvec4(0.f, 0.67f, 0.56f, 1.f);
			//material->diffuse = tvec4(0.41f, 0.69f, 0.67f, 1.f);
			//material->specular = tvec4(0.40f, 0.73f, 0.72f, 1.f);
			material->shininess = 3.0;
			material->setTexture(RES_PATH + "earth.jpg");

			generate();
			initArray();
		}
		~VerletCloth()override {
			delete vertices;
			delete texcoords;
			delete indices;
			delete normals;
			delete material;

			delete back_buffer;
			delete constraints_sampler;
			delete vertices_sampler;
			delete last_vertices_sampler;
		}

		void bindUniform(const KShader::Shader* shader)const override {
			Object3D::bindUniform(shader);
			material->bindUniform(shader);
		}

		void initBackBuffer(const KShader::Shader* back_shader) {
			back_buffer = new KBuffer::BackBuffer(back_shader,
			{
				"o_last_vertex",
				"o_vertex"
			},
			{
				size_x * size_y * sizeof(tvec3),
				size_x * size_y * sizeof(tvec3)
			},
			GL_SEPARATE_ATTRIBS);
		}

		void bindBackUniform(const KShader::Shader* back_shader)const {
			back_shader->bindUniform2i("size", size_x, size_y);
			back_shader->bindUniform2f("rest_length", rest_length);
			back_shader->bindUniform1f("diag_length", diag_length);

			back_shader->bindUniform1f("mass", mass);
			back_shader->bindUniform1f("a_resistance", a_resistance);
			back_shader->bindUniform3f("f_wind", f_wind);

			back_shader->bindUniform1f("last_dt", last_dt);
			back_shader->bindUniform1f("delta_time", delta_time);

			back_shader->bindUniform1f("ks", ks);
			back_shader->bindUniform1f("kd", kd);
			back_shader->bindUniform1f("ks_bend", ks_bend);
			back_shader->bindUniform1f("kd_bend", kd_bend);

			back_shader->bindUniform3f("u_position", position);

			constraints_sampler->bind(back_shader, "constraints_tbo", 0);
			last_vertices_sampler->bind(back_shader, "last_vertices_tbo", 1);
			vertices_sampler->bind(back_shader, "vertices_tbo", 2);
		}

		void renderBack()const {
			//remember to bind uniform and delta time before.
			glEnable(GL_RASTERIZER_DISCARD);
			back_buffer->enable();

			glDrawArrays(GL_POINTS, 0, size_x * size_y);

			back_buffer->disable();
			glDisable(GL_RASTERIZER_DISCARD);


			last_vertices_sampler->copyDataFromBuffer(0, back_buffer);
			vbo->copyDataFormBuffer(1, back_buffer);
			vertices_sampler->copyDataFromBuffer(1, back_buffer);
			
			//const tvec3* data0 = back_buffer->getData<tvec3>(0);
			//const tvec3* data1 = back_buffer->getData<tvec3>(1);
		}

		void render()const override {
			bind();

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_PRIMITIVE_RESTART);
			glPrimitiveRestartIndex(0XFFFFFFFF);
			glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, nullptr);
			glDisable(GL_PRIMITIVE_RESTART);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			unBind();
		}
	};
}

#endif // !VERLET_CLOTH_H

