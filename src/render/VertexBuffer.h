//
// Created by KingSun on 2018/05/09
//

#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <unordered_set>
#include <GL/glew.h>
#include "../Header.h"

namespace KBuffer {
	enum BufferType {
		VERTEX, INDEX
	};

	class VertexBuffer {
	private:
		Kuint id;
		GLenum type;

	public:
		VertexBuffer(Kuint size, BufferType bufferType = VERTEX) {
			if (bufferType == VERTEX) this->type = GL_ARRAY_BUFFER;
			else this->type = GL_ELEMENT_ARRAY_BUFFER;
			glGenBuffers(1, &id);
			glBindBuffer(this->type, id);
			glBufferData(this->type, size, nullptr, GL_STATIC_DRAW);
		}
		VertexBuffer(const void *data, Kuint size, BufferType bufferType = VERTEX) {
			if (bufferType == VERTEX) this->type = GL_ARRAY_BUFFER;
			else this->type = GL_ELEMENT_ARRAY_BUFFER;
			glGenBuffers(1, &id);
			glBindBuffer(this->type, id);
			glBufferData(this->type, size, data, GL_STATIC_DRAW);
		}

		~VertexBuffer() {
			if (glIsBuffer(id)) glDeleteBuffers(1, &id);
		}

		void allocate(Kuint offset, Kuint size, const void* data) {
			glBindBuffer(type, id);
			glBufferSubData(type, offset, size, data);
		}

		void bind()const {
			glBindBuffer(type, id);
		}

		void unBind()const {
			glBindBuffer(type, 0);
		}
	};
}

#endif //VERTEX_BUFFER_H