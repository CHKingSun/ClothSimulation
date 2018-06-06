//
// Created by KingSun on 2018/05/09
//

#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <unordered_set>
#include <GL/glew.h>
#include "../Header.h"
#include "./BackBuffer.h"

namespace KBuffer {
	enum BufferType {
		VERTEX, INDEX
	};

	class VertexBuffer {
	private:
		Kuint id;
		GLenum type;

	public:
		VertexBuffer(Kuint size, const void *data = nullptr, BufferType bufferType = VERTEX) {
			if (bufferType == VERTEX) this->type = GL_ARRAY_BUFFER;
			else this->type = GL_ELEMENT_ARRAY_BUFFER;
			glGenBuffers(1, &id);
			glBindBuffer(this->type, id);
			glBufferData(this->type, size, data, GL_STATIC_DRAW);
		}
		~VertexBuffer() {
			if (glIsBuffer(id)) glDeleteBuffers(1, &id);
		}

		void allocate(Kuint offset, Kuint size, const void* data)const {
			glBindBuffer(type, id);
			glBufferSubData(type, offset, size, data);
		}

		void bindToBackBuffer(Kuint index, const BackBuffer* back)const {
			if (back == nullptr) return;
			back->bindBuffer(index, id);
		}

		void copyDataFormBuffer(Kuint index, const BackBuffer* back)const {
			if (back == nullptr) return;
			back->copyDataToBuffer(index, id, type);
		}

		void bind()const {
			glBindBuffer(type, id);
		}

		void unBind()const {
			glBindBuffer(type, 0);
		}

		GLenum getType()const {
			return type;
		}
	};
}

#endif //VERTEX_BUFFER_H