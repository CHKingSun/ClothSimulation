//
// Created by KingSun on 2018/05/13
//

#ifndef BACK_BUFFER_H
#define BACK_BUFFER_H

#include "../Header.h"
#include "./Shader.h"

namespace KBuffer {
	//Transform feedback buffer
	//It's better that you just use a vertex shader (also can with compute or other else) to
	//get transform feedback, because when you discard something from fragment shader, you will
	//get a crash. And remember to glEnable(GL_RASTERIZER_DISCARD)

	class BackBuffer{
	private:
		Kuint program;
		Kuint bufferSize;
		std::vector<Kuint> buffers;

	public:
		BackBuffer(const KShader::Shader* shader, const std::vector<std::string>& varyings,
			Kuint bufferSize) : program(shader->program), bufferSize(bufferSize) {
			Kuint size = varyings.size();
			const auto names = new const char*[size];
			for (int i = 0; i < size; ++i) {
				names[i] = varyings[i].data();
			}
			glTransformFeedbackVaryings(program, size, names, GL_INTERLEAVED_ATTRIBS);
			glLinkProgram(program);

			Kuint tfbo;
			glGenBuffers(1, &tfbo);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfbo);
			glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
			buffers.emplace_back(tfbo);
		}

		BackBuffer(const KShader::Shader* shader, const std::vector<std::string>& varyings,
			const std::vector<Kuint>& buffers, GLenum bufferMode)
			: program(shader->program), buffers(buffers), bufferSize(0) {
			Kuint size = varyings.size();
			const auto names = new const char*[size];
			for (int i = 0; i < size; ++i) {
				names[i] = varyings[i].data();
			}
			glTransformFeedbackVaryings(program, size, names, bufferMode);
			glLinkProgram(program);
		}

		void bind() {
			int index = 0;
			for (auto &it : buffers) {
				glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index++, it);
			}
		}

		void enable(GLenum type) {
			//Be sure not to discard somthing form your fragment or some other shader
			//or you will get a crash
			glBeginTransformFeedback(type);
		}

		void disable() {
			glEndTransformFeedback();
		}

		template <typename T>
		const T* getData() {
			static_assert(false, "BlockBuffer::getData<T> is just for some type");
			//compile error
		}

		template<>
		const Kfloat* getData<Kfloat>() {
			if (bufferSize == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[0]);
			auto data = new float[bufferSize / sizeof(Kfloat)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bufferSize, data);
			return data;
		}

		template<>
		const KVector::Vec2* getData<KVector::Vec2>() {
			if (bufferSize == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[0]);
			auto data = new KVector::Vec2[bufferSize / sizeof(KVector::Vec2)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bufferSize, data);
			return data;
		}

		template<>
		const KVector::Vec3* getData<KVector::Vec3>() {
			if (bufferSize == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[0]);
			auto data = new KVector::Vec3[bufferSize / sizeof(KVector::Vec3)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bufferSize, data);
			return data;
		}

		template<>
		const KVector::Vec4* getData<KVector::Vec4>() {
			if (bufferSize == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[0]);
			auto data = new KVector::Vec4[bufferSize / sizeof(KVector::Vec4)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bufferSize, data);
			return data;
		}

		template<>
		const KMatrix::Mat3* getData<KMatrix::Mat3>() {
			if (bufferSize == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[0]);
			auto data = new KMatrix::Mat3[bufferSize / sizeof(KMatrix::Mat3)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bufferSize, data);
			return data;
		}

		template<>
		const KMatrix::Mat4* getData<KMatrix::Mat4>() {
			if (bufferSize == 0) {
				std::cerr << "Cannot manage buffer size!" << std::endl;
				return nullptr;
			}
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffers[0]);
			auto data = new KMatrix::Mat4[bufferSize / sizeof(KMatrix::Mat4)];
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bufferSize, data);
			return data;
		}

		const Kuint getBufferSize()const {
			return bufferSize;
		}
	};
}

#endif //BACK_BUFFER_H
