//
// Created by KingSun on 2018/06/06
//

#ifndef TEXTURE_BUFFER_H
#define TEXTURE_BUFFER_H

#include "../Header.h"
#include "./Shader.h"
#include "./BackBuffer.h"

namespace KBuffer {
	class TextureBuffer {
	private:
		Kuint tex_id;
		Kuint buffer;
		GLenum format;

	public:
		TextureBuffer(Kuint size, const void* data = nullptr, GLenum format = GL_RGB32F):
			format(format) {
			glGenTextures(1, &tex_id);
			glBindTexture(GL_TEXTURE_BUFFER, tex_id);

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_TEXTURE_BUFFER, buffer);
			glBufferData(GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);

			glTexBuffer(GL_TEXTURE_BUFFER, format, buffer);
		}
		~TextureBuffer() {
			if (glIsBuffer(buffer)) glDeleteBuffers(1, &buffer);
			if (glIsTexture(tex_id)) glDeleteTextures(1, &tex_id);
		}

		void allocate(Kuint offset, Kuint size, const void* data)const {
			glBindBuffer(GL_TEXTURE_BUFFER, buffer);
			glBufferSubData(GL_TEXTURE_BUFFER, offset, size, data);
		}

		void bind(const KShader::Shader* shader, const std::string& name, Kint index)const {
			if (shader == nullptr) return;
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_BUFFER, tex_id);
			shader->bindUniform1i(name, index);
		}

		void copyDataFromBuffer(Kuint index, const BackBuffer* back)const {
			if (back == nullptr) return;
			back->copyDataToBuffer(index, buffer, GL_TEXTURE_BUFFER);
		}

		void bindToBackBuffer(Kuint index, const BackBuffer* back)const {
			if (back == nullptr) return;
			back->bindBuffer(index, buffer);
		}
	};
}

#endif // !TEXTURE_BUFFER_H
