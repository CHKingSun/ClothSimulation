//
// Created by KingSun on 2018/05/26
//

#ifndef CLOTH_H
#define  CLOTH_H

#include <vector>
#include <cmath>
#include "../Header.h"
#include "../math/Vec3.h"
#include "../util/Material.h"
#include "./Object3D.h"

namespace KObject {
	using tvec2 = KVector::Vec2;
	using tvec3 = KVector::Vec3;

	class Cloth : public Object3D {
	private:
		class Spring {
		private:
			static const Kfloat ks;
			static const Kfloat kd;
			const Cloth* const parent;
			Kuint vertex_index[2];
			Kfloat rest_length;

			tvec3 getVelcityDirection(Kuint index)const {
				if (!isValid()) return tvec3();
				if (index == vertex_index[0]) return parent->points->at(vertex_index[0])->getVelocity()
					- parent->points->at(vertex_index[1])->getVelocity();
				else if (index == vertex_index[1]) return parent->points->at(vertex_index[1])->getVelocity()
					- parent->points->at(vertex_index[0])->getVelocity();
				return tvec3();
			}

		public:
			Spring(Kuint one, Kuint other, Kfloat rest, const Cloth* parent) :
				parent(parent), rest_length(rest) {
				vertex_index[0] = one;
				vertex_index[1] = other;
			}

			Kfloat getCurrentLength()const {
				if (parent->last_vertices == nullptr || vertex_index[0] >= parent->last_vertices->size() ||
					vertex_index[1] >= parent->last_vertices->size()) return 0;
				return KFunction::distance(parent->last_vertices->at(vertex_index[0]),
					parent->last_vertices->at(vertex_index[1]));
			}

			Kfloat getRestLength()const {
				return rest_length;
			}

			Kboolean isValid()const {
				return !(parent == nullptr || parent->last_vertices == nullptr ||
					vertex_index[0] >= parent->last_vertices->size() ||
					vertex_index[1] >= parent->last_vertices->size());
			}

			tvec3 getDirection(Kuint index)const {
				if (!isValid()) return tvec3();
				if (index == vertex_index[0]) return parent->last_vertices->at(vertex_index[0])
					- parent->last_vertices->at(vertex_index[1]);
				else if (index == vertex_index[1]) return parent->last_vertices->at(vertex_index[1])
					- parent->last_vertices->at(vertex_index[0]);
				return tvec3();
			}

			tvec3 getForce(Kuint index)const {
				Kfloat delta_length = getCurrentLength() - rest_length;
				if (delta_length <= 0.f) return tvec3(0.f);
				tvec3 dp(getDirection(index).normalize());
				return -(ks * delta_length + kd * dp.dot(getVelcityDirection(index))) * dp;
			}
		};

		class ClothPoint {
		private:
			Kuint index; //vertex index
			const Cloth* const parent;

			Kboolean is_constraint;
			Kfloat mass;
			tvec3 f_air;
			tvec3 acceleration;
			tvec3 velocity;
			std::vector<const Spring*>* springs;

			static const tvec3 f_wind;
			static const tvec3 gravity;

			void calAirForce() {
				//f_air = tvec3(0.f); return;
				static const Kfloat a_resistance = -0.0125f;
				if(!velocity.isZero()) f_air = (a_resistance * velocity.dot(velocity)) * KFunction::normalize(velocity);
				else f_air = tvec3(0.f);
			}

			void calAcceleration() {
				calAirForce();
				acceleration = mass * gravity + f_wind + f_air;
				if (springs != nullptr) {
					for (auto it : *springs) {
						acceleration += it->getForce(index);
					}
				}
				acceleration /= mass;
			}

		public:
			ClothPoint(Kuint index, const Cloth* parent, Kboolean is_constraint = false):
				index(index), parent(parent), is_constraint(is_constraint) {}
			~ClothPoint() {
				if (springs != nullptr) {
					for (auto &it : *springs) {
						delete it;
						it = nullptr;
					}
					delete springs;
				}
			}

			void setConstraint(Kboolean is_constraint = true) {
				this->is_constraint = is_constraint;
			}

			void updateMass(Kfloat mass = 0.02f) {
				this->mass = mass;
			}

			Kboolean isValid()const {
				return !(parent == nullptr || parent->last_vertices == nullptr ||
					parent->last_vertices->size() <= index);
			}

			tvec3 getMovement(Kfloat t) {
				if (is_constraint) return tvec3(0.f);
				calAcceleration();
				if (isValid() && parent->last_vertices->at(index).y + parent->position.y <= 0) {
					if (acceleration.y < 0) acceleration.y = 0;
					if (velocity.y < 0)  velocity.y = 0;
				}
				tvec3 tmp(velocity);
				velocity += acceleration * t;
				return (tmp += velocity) *= (t / 2.0f);
			}

			void addSpring(const Spring* spring) {
				if (spring == nullptr) return;
				if (springs == nullptr) springs = new std::vector<const Spring*>();
				springs->emplace_back(spring);
			}

			const tvec3& getVelocity()const {
				return velocity;
			}

			const tvec3& getAcceleration()const {
				return acceleration;
			}
		};

	private:
		Ksize size;
		Ksize count;

		std::vector<tvec3>* vertices; //It will be deleted in CLoth class
		std::vector<tvec3>* last_vertices; //It will be deleted in CLoth class
		std::vector<ClothPoint*>* points; //Every vectex has a point
		
		std::vector<tvec2>* texcoords;
		std::vector<Kuint>* indices;
		std::vector<tvec3>* normals;

		KMaterial::Material* material;

		void generate() {
			vertices = new std::vector<tvec3>();
			vertices->reserve(size * size);	
			last_vertices = new std::vector<tvec3>();
			last_vertices->reserve(size * size);
			points = new std::vector<ClothPoint*>();
			points->reserve(size * size);
			texcoords = new std::vector<tvec2>();
			texcoords->reserve(size * size);
			//normals = new std::vector<tvec3>();
			//normals->reserve(size * size);

			Kfloat rest_length = 1.f;
			Kfloat diag_length = sqrt(2) * rest_length;
			Kfloat pertex = 1.f / size;
			//Kfloat y = 2.f + size * rest_length;
			Kfloat y = size * rest_length / 2.f;
			Kfloat ty = 0.f;
			for (int i = 0; i < size; ++i, y -= rest_length, ty += pertex) {
				Kfloat x = size * -rest_length / 2.f;
				Kfloat tx = 0.f;
				for (int j = 0; j < size; ++j, x += rest_length, tx += pertex) {
					//vertices->emplace_back(x, y, 0.f);
					//last_vertices->emplace_back(x, y, 0.f);
					vertices->emplace_back(x, size + 2, y);
					last_vertices->emplace_back(x, size + 2, y);
					texcoords->emplace_back(tx, ty);
					Kuint index = points->size(); //i * size + j
					auto p = new ClothPoint(index, this);
					if (i > 0) {
						//if(j > 0) p->addSpring(
						//	new Spring(index, index - size - 1, diag_length, this));
						p->addSpring(
							new Spring(index, index - size, rest_length, this));
						//if (j < size - 1) p->addSpring(
						//	new Spring(index, index - size + 1, diag_length, this));
					}
					if (j > 0) p->addSpring(
						new Spring(index, index - 1, rest_length, this));
					if (j < size - 1) p->addSpring(
						new Spring(index, index + 1, rest_length, this));
					if (i < size - 1) {
						//if (j > 0) p->addSpring(
						//	new Spring(index, index + size - 1, diag_length, this));
						p->addSpring(
							new Spring(index, index + size, rest_length, this));
						//if (j < size - 1) p->addSpring(
						//	new Spring(index, index + size + 1, diag_length, this));
					}
					if (j > 1) p->addSpring(
						new Spring(index, index - 2, rest_length * 2, this));
					if (j < size - 2) p->addSpring(
						new Spring(index, index + 2, rest_length * 2, this));
					if (i > 1) p->addSpring(
						new Spring(index, index - size * 2, rest_length * 2, this));
					if (i < size - 2) p->addSpring(
						new Spring(index, index + size * 2, rest_length * 2, this));
					points->emplace_back(p);
				}
			}
			for (int i = 0; i < size; ++i) {
				points->at(i)->setConstraint(true);
			}

			indices = new std::vector<Kuint>();
//#define PRIMITIVE
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

			vbo = new KBuffer::VertexBuffer(vertices->size() * sizeof(tvec3), vertices->data());
			vao->allocate(vbo, A_POSITION, 3, GL_FLOAT);

			tbo = new KBuffer::VertexBuffer(texcoords->size() * sizeof(tvec2), texcoords->data());
			vao->allocate(tbo, A_TEXCOORD, 2, GL_FLOAT);

			//nbo = new KBuffer::VertexBuffer(normals->size() * sizeof(tvec3), normals->data());
			//vao->allocate(nbo, A_NORMAL, 3, GL_FLOAT);

			ibo = new KBuffer::VertexBuffer(count * sizeof(Ksize), indices->data(), KBuffer::INDEX);

			delete texcoords; texcoords = nullptr;
			delete indices; indices = nullptr;
		}

	public:
		Cloth(Ksize size = 30): Object3D("Cloth"), size(size),
		vertices(nullptr), points(nullptr), texcoords(nullptr),
		normals(nullptr), indices(nullptr), material(nullptr) {
			material = new KMaterial::Material();
			material->ambient = KVector::Vec4(0.f, 0.67f, 0.56f, 1.f);
			material->diffuse = KVector::Vec4(0.41f, 0.69f, 0.67f, 1.f);
			material->specular = KVector::Vec4(0.40f, 0.73f, 0.72f, 1.f);
			material->shininess = 3.0;

			generate();
			initArray();
		}
		~Cloth()override {
			delete vertices;
			delete last_vertices;
			delete texcoords;
			delete normals;
			delete indices;
			delete material;
			if (points != nullptr) {
				for (auto &it : *points) {
					delete it;
					it = nullptr;
				}
				delete points;
			}
		}

		void bindUniform(const KShader::Shader* shader)const override {
			Object3D::bindUniform(shader);
			material->bindUniform(shader);
		}

		void updatePosition(Kfloat delta_time) {
			if (KFunction::isZero(delta_time)) return;
			for (Ksize i = 0; i < size * size; ++i) {
				last_vertices->at(i) = vertices->at(i);
			}
			for (Ksize i = 0; i < size * size; ++i) {
				points->at(i)->updateMass();
				vertices->at(i) += points->at(i)->getMovement(delta_time);
				if (vertices->at(i).y < 0) vertices->at(i).y = 0.00072;
			}
			vbo->allocate(0, vertices->size() * sizeof(tvec3), vertices->data());
			if (!isnan(vertices->at(size).y)) std::cout << vertices->at(size) << "\t"
				<< points->at(size)->getVelocity() << "\t"
				<< points->at(size)->getAcceleration() << "\n"
				<< vertices->at(size + 1) << "\t"
				<< points->at(size + 1)->getVelocity() << "\t"
				<< points->at(size + 1)->getAcceleration() << "\n"
				<< std::endl;
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

#ifdef IMGUI_ENABLE
		void drawGui() {
			Kuint index = size;
			tvec3 t = points->at(index)->getVelocity();
			ImGui::Text("Vel of p%d: %.2f, %.2f, %.2f", index, t.x, t.y, t.z);
			index = size * 2 - 1;
			t = points->at(index)->getVelocity();
			ImGui::Text("Vel of p%d: %.2f, %.2f, %.2f", index, t.x, t.y, t.z);

			index = size;
			t = points->at(index)->getAcceleration();
			ImGui::Text("Acc of p%d: %.2f, %.2f, %.2f", index, t.x, t.y, t.z);
			index = size * 2 - 1;
			t = points->at(index)->getAcceleration();
			ImGui::Text("Acc of p%d: %.2f, %.2f, %.2f", index, t.x, t.y, t.z);
		}
#endif
	};

	const Kfloat Cloth::Spring::ks = 15.f;
	const Kfloat Cloth::Spring::kd = 0.9f;
	const tvec3 Cloth::ClothPoint::gravity(0.0f, -9.8f, 0.0f);
	const tvec3 Cloth::ClothPoint::f_wind(0.0f, 0.0f, 0.0f);
}

#endif // CLOTH_H
