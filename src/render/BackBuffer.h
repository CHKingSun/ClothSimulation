//
// Created by KingSun on 2018/05/13
//

#ifndef BACK_BUFFER_H
#define BACK_BUFFER_H

#include <vector>
#include "../Header.h"
#include "./Shader.h"

namespace KBuffer {
	//Transform feedback buffer
	//It's better that you just use a vertex shader (also can with compute or other else) to
	//get transform feedback, because when you discard something from fragment shader, you will
	//get a crash. And remember to glEnable(GL_RASTERIZER_DISCARD)

	class BackBuffer{
	private:
		std::vector<Kuint> buffers_size;
		Kuint* buffers;
		Kuint n_buffers;

	public:
		BackBuffer(const KShader::Shader* shader, const std::vector<const char*>& varyings,
			const std::vector<Kuint>& buffers_size, GLenum mode = GL_INTERLEAVED_ATTRIBS):
			buffers_size(buffers_size), buffers(nullptr) {
			glTransformFeedbackVaryings(shader->program, varyings.size(), varyings.data(), mode);
			glLinkProgram(shader->program);

			n_buffers = buffers_size.size();
			if (n_buffers == 0) return;
			//note: remember that you are not necessary to create following buffers
			//because you can use bindBuffer() function to bind other buffer to index.
			buffers = new Kuint[n_buffers];
			glGenBuffers(n_buffers, buffers);
			for (int i = 0; i < n_buffers; ++i) {
				glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[i]);
				glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, buffers_size[i], nullptr, GL_STATIC_DRAW);
				glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, i, buffers[i]);
			}
		}
		~BackBuffer() {
			if (buffers != nullptr) glDeleteBuffers(n_buffers, buffers);
		}

		void bindBuffer(Kuint index, Kuint buffer)const {
			if (index >= n_buffers || !glIsBuffer(buffer)) return;
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer);
		}

		void enable(GLenum type = GL_POINTS)const {
			//Be sure not to discard somthing form your fragment or some other shader
			//or you will get a crash
			glBeginTransformFeedback(type);
		}

		void disable()const {
			glEndTransformFeedback();
		}

		void copyDataToBuffer(Kuint index, Kuint buffer, GLenum type)const {
			if (index >= n_buffers) return;
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[index]);
			glBindBuffer(type, buffer);
			glCopyBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, type, 0, 0, buffers_size[index]);
		}

		template <typename T>
		const T* getData(Kuint index = 0)const {
			static_assert(false, "BlockBuffer::getData<T> is just for some type");
			//compile error
		}

		template<>
		const Kfloat* getData<Kfloat>(Kuint index)const {
			if (index >= n_buffers) return nullptr;
			if (buffers_size[index] == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[index]);
			auto data = new float[buffers_size[index] / sizeof(Kfloat)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffers_size[index], data);
			return data;
		}

		template<>
		const KVector::Vec2* getData<KVector::Vec2>(Kuint index)const {
			if (buffers_size[index] == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[index]);
			auto data = new KVector::Vec2[buffers_size[index] / sizeof(KVector::Vec2)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffers_size[index], data);
			return data;
		}

		template<>
		const KVector::Vec3* getData<KVector::Vec3>(Kuint index)const {
			if (buffers_size[index] == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[index]);
			auto data = new KVector::Vec3[buffers_size[index] / sizeof(KVector::Vec3)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffers_size[index], data);
			return data;
		}

		template<>
		const KVector::Vec4* getData<KVector::Vec4>(Kuint index)const {
			if (buffers_size[index] == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[index]);
			auto data = new KVector::Vec4[buffers_size[index] / sizeof(KVector::Vec4)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffers_size[index], data);
			return data;
		}

		template<>
		const KMatrix::Mat3* getData<KMatrix::Mat3>(Kuint index)const {
			if (buffers_size[index] == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[index]);
			auto data = new KMatrix::Mat3[buffers_size[index] / sizeof(KMatrix::Mat3)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffers_size[index], data);
			return data;
		}

		template<>
		const KMatrix::Mat4* getData<KMatrix::Mat4>(Kuint index)const {
			if (buffers_size[index] == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[index]);
			auto data = new KMatrix::Mat4[buffers_size[index] / sizeof(KMatrix::Mat4)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffers_size[index], data);
			return data;
		}

		const Kuint getBufferSize(Kuint index = 0)const {
			if (index >= n_buffers) return 0;
			return buffers_size[index];
		}
	};
}

#endif //BACK_BUFFER_H
