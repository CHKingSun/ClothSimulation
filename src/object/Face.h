//
// Created by KingSun on 2018/05/11
//

#ifndef FACE_H
#define FACE_H

#include <cassert>
#include <iosfwd>
#include "../Header.h"

namespace KObject {
	template <typename T = Kushort>
	class Face {
	private:
		T values[3];

	public:
		Face<T>() = default;
		Face<T>(T p0, T p1, T p2) {
			set(p0, p1, p2);
		}
		~Face<T>() = default;

		inline const T* data()const {
			return &values[0];
		}
		inline const T& operator[](Kuint n)const {
			assert(n >= 0 && n < 3);
			return values[n];
		}
		inline T& operator[](Kuint n) {
			assert(n >= 0 && n < 3);
			return values[n];
		}
		inline Face<T>& operator=(const Face<T>& face) {
			values[0] = face[0];
			values[1] = face[1];
			values[2] = face[2];
		}

		Face<T>& set(T p0, T p1, T p2) {
			values[0] = p0;
			values[1] = p1;
			values[2] = p2;
			return *this;
		}
			
		//Vec3 getNormal(const Kfloat* data)
		//judge if a point is in face. (const Kfloat* data, Ve3 point)
		//judge if a ray is intersect with face. (const Kfloat* data, Ray ray)
	};

	template <typename T>
	std::istream& operator>>(std::istream& is, Face<T>& face) {
		is >> face[0] >> face[1] >> face[2];
		return is;
	}

	template <typename T>
	std::ostream& operator<<(std::ostream& os, const Face<T> face) {
		os << face[0] << ", " << face[1] << ", " << face[2];
		return os;
	}
}

#endif //FACE_H
