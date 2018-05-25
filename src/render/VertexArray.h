//
// Created by KingSun on 2018/05/10
//

#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <unordered_set>
#include <GL/glew.h>
#include "../Header.h"
#include "../math/Vec3.h"
#include "./VertexBuffer.h"

namespace KBuffer {
	class VertexArray {
	private:
		Kuint id;
		std::unordered_set<Kint> *locations;

	public:
		VertexArray() {
			glGenVertexArrays(1, &id);
			glBindVertexArray(id);
			locations = nullptr;
		}
		~VertexArray() {
			if (glIsVertexArray(id)) glDeleteVertexArrays(1, &id);
			delete locations;
		}

		void bind()const {
			glBindVertexArray(id);
		}

		void unBind()const {
			glBindVertexArray(0);
		}

		void allocate(const VertexBuffer* vb, Kint location, Kuint size, GLenum type,
			bool normalized = false, Kuint stride = 0, Kuint offset = 0) {
			bind();
			vb->bind();
			if (locations == nullptr) locations = new std::unordered_set<Kint>();
			locations->insert(location);
			glVertexAttribPointer(location, size, type, normalized, stride, (void*)offset);
		}

		void setVertexAttrib3f(Kint location, const KVector::Vec3& v)const {
			glVertexAttrib3fv(location, v.data());
		}

		void setVertexAttrib2f(Kint location, const KVector::Vec2& v)const {
			glVertexAttrib2fv(location, v.data());
		}

		void clearLocation() {
			delete locations;
			locations = nullptr;
		}

		void enableVertexArray()const {
			if (locations == nullptr) return;
			for (auto loc : *locations) {
				glEnableVertexAttribArray(loc);
			}
		}

		void disableVertexArray()const {
			if (locations == nullptr) return;
			for (auto loc : *locations) {
				glDisableVertexAttribArray(loc);
			}
		}
	};
}

#endif //VERTEX_ARRAY_H