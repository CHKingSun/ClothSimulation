//
// Created by KingSun on 2018/05/26
//

#ifndef CLOTH_H
#define  CLOTH_H

#include <vector>
#include "../Header.h"
#include "../math/Vec3.h"
#include "./Object3D.h"

namespace KObject {
	using tvec3 = KVector::Vec3;

	class Cloth : public Object3D {
	private:
		class Spring {
		private:
			static const Kfloat k;
			const Cloth* const parent;
			Kuint vertex_index[2];
			Kfloat rest_length;

		public:
			Spring(Kuint one, Kuint other, Kfloat rest, const Cloth* parent) :
				parent(parent), rest_length(rest) {
				vertex_index[0] = one;
				vertex_index[1] = other;
			}

			Kfloat getCurrentLength()const {
				if (parent->vertices == nullptr || vertex_index[0] >= parent->vertices->size() ||
					vertex_index[1] >= parent->vertices->size()) return 0;
				return KFunction::distance(parent->vertices->at(vertex_index[0]), parent->vertices->at(vertex_index[1]));
			}

			Kfloat getRestLength()const {
				return rest_length;
			}

			Kboolean isValid()const {
				return !(parent == nullptr || parent->vertices == nullptr ||
					vertex_index[0] >= parent->vertices->size() ||
					vertex_index[1] >= parent->vertices->size());
			}

			tvec3 getDirection(Kuint index)const {
				if (!isValid()) return tvec3();
				if (index == vertex_index[0]) return parent->vertices->at(vertex_index[1]) - parent->vertices->at(vertex_index[0]);
				else if (index == vertex_index[1]) return parent->vertices->at(vertex_index[0]) - parent->vertices->at(vertex_index[1]);
				return tvec3();
			}

			Kfloat getK()const {
				return k;
			}

			tvec3 getForce(Kuint index)const {
				return getDirection(index) *= k;
			}
		};

		class ClothPoint {
		private:
			Kuint index; //vertex index
			const Cloth* const parent;

			Kfloat mass;
			tvec3 f_air;
			tvec3 acceleration;
			tvec3 velocity;
			std::vector<const Spring*>* springs;

			static const tvec3 f_wind;
			static const tvec3 gravity;

			void calAirForce() {
				static const Kfloat a_resistance = 0.1;
				f_air = -a_resistance * velocity * velocity;
			}

			void calAcceleration() {
				calAirForce();
				acceleration = mass * gravity + f_wind + f_air;
				for (auto it : *springs) {
					acceleration += it->getForce(index);
				}
				acceleration /= mass;
				if (acceleration.y < 0 && isValid() && parent->vertices->at(index).y <= 0) acceleration.y = 0;
			}

		public:
			ClothPoint(Kuint index, const Cloth* parent) :
				index(index), parent(parent) {}

			void updateMass(Kfloat mass = 1.0f) {
				this->mass = mass;
			}

			Kboolean isValid()const {
				return !(parent == nullptr || parent->vertices == nullptr ||
					parent->vertices->size() <= index);
			}

			tvec3 getMovement(Kfloat t) {
				calAcceleration();
				tvec3 tmp(velocity);
				velocity += acceleration * t;
				return (tmp += velocity) *= (t / 2.0f);
			}

			void addSpring(const Spring* spring) {
				if (springs == nullptr || spring == nullptr) return;
				springs->emplace_back(spring);
			}
		};

	private:
		std::vector<tvec3>* vertices; //It will be deleted in CLoth class

	public:
	};

	const Kfloat Cloth::Spring::k = 0.1;
	const tvec3 Cloth::ClothPoint::gravity(0.0f, 9.8f, 0.0f);
	const tvec3 Cloth::ClothPoint::f_wind(0.0f, 0.0f, 0.0f);
}

#endif // CLOTH_H
