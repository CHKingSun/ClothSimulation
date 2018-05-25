//
// Created by KingSun on 2018/5/3.
//

#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <unordered_map>
#include "../Header.h"
#include "../math/Mat4.h"

static Kuint getSize(GLenum type) {
	switch (type)
	{
	case GL_FLOAT: return sizeof(GLfloat);
	case GL_UNSIGNED_INT: return sizeof(GLuint);
	case GL_UNSIGNED_SHORT: return sizeof(GLushort);
	case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
	case GL_FLOAT_VEC3: return sizeof(GLfloat) * 3;
	case GL_FLOAT_VEC4: return sizeof(GLfloat) * 4;
	case GL_FLOAT_MAT3: return sizeof(GLfloat) * 12;
	//When we use mat3, it will devide into 3 vec3,
	//but when GLSL read the buffer of vec3, it will read 4 value and discard the last value,
	//thus, be sure to push mat4x3(3 vec4) data into the buffer or you may get what you don't want.
	case GL_FLOAT_MAT4: return sizeof(GLfloat) * 16;
	case GL_FLOAT_MAT2: return sizeof(GLfloat) * 4;
	case GL_FLOAT_VEC2: return sizeof(GLfloat) * 2;
	case GL_INT: return sizeof(GLint);
	case GL_SHORT: return sizeof(GLshort);
	case GL_BYTE: return sizeof(GLbyte);
	case GL_DOUBLE: return sizeof(GLdouble);
	default: return 0;
	}
}

namespace KBuffer { class BackBuffer; }

namespace KShader{
	using tvec4 = KVector::Vec4;
	using tvec3 = KVector::Vec3;
	using tvec2 = KVector::Vec2;
	using tmat4 = KMatrix::Mat4;
	using tmat3 = KMatrix::Mat3;

    class Shader{
		friend class KBuffer::BackBuffer;

    private:
        Kuint program;
        std::unordered_map<std::string, int> *uniforms;

        bool readFile(const std::string &filename, std::string &content)const {
#if 1
			FILE* file = fopen(filename.data(), "rt"); //read text
			if (file == nullptr) {
				std::cerr << "File " << filename << " read failed!" << std::endl;
				return false;
			}
			fseek(file, 0, SEEK_END);
			unsigned long length = ftell(file) + 1;
			auto *data = new char[length];
			memset(data, 0, length);
			fseek(file, 0, SEEK_SET);
			fread(data, 1, length, file);
			fclose(file);
			content = std::string(data);
			//std::cout << content << std::endl;
			delete[] data;
			return true;
#else
            std::ifstream file(filename, std::ios::in);
            if(!file.is_open())
            {
                std::cerr<<"File "<<filename<<" read failed!"<<std::endl;
                return false;
            }
            content = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
#endif
        }

        Kint getLocation(const std::string &name)const {
            auto it = uniforms->find(name);
            if(it == uniforms->end()){
				Kint loc = glGetUniformLocation(program, name.c_str());
				if (loc == -1) std::cerr << "Wrong unifrom name: " << name << std::endl;
				uniforms->emplace(name, loc);
				return loc;
            }
			return it->second;
        }

    public:
        Shader():uniforms(new std::unordered_map<std::string, int>()) {
			program = glCreateProgram(); 
			if (program == 0) {
				std::cerr << "Create program failed!" << std::endl;
			}
		}
        Shader(const std::string &vs_filename, const std::string &fs_filename):
        uniforms(new std::unordered_map<std::string, int>()){
            createProgram(vs_filename, fs_filename);
        }
        ~Shader(){
			uniforms->clear();
            delete uniforms;
			if (glIsProgram(program)) glDeleteProgram(program);
        }

        bool createProgram(const std::string &vs_filename, const std::string &fs_filename){
            program = glCreateProgram();
            if(program == 0){
                std::cerr << "Create program failed!" << std::endl;
                return false;
            }

            Kuint vertShader = createShader(GL_VERTEX_SHADER, vs_filename);
            Kuint fragShader = createShader(GL_FRAGMENT_SHADER, fs_filename);
            glAttachShader(program, vertShader);
            glAttachShader(program, fragShader);
            glLinkProgram(program);
            glDeleteShader(vertShader);
            glDeleteShader(fragShader);

            GLint flag;
            auto *error_log = new GLchar[1024];
            glGetProgramiv(program, GL_LINK_STATUS, &flag);
            if(GL_FALSE == flag){
                glGetProgramInfoLog(program, 1024, nullptr, error_log);
                std::cerr << "Error linking program: " << error_log << std::endl;
                glDeleteProgram(program);
                return false;
            }

            glValidateProgram(program);
            glGetProgramiv(program, GL_VALIDATE_STATUS, &flag);
            if(GL_FALSE == flag){
                glGetProgramInfoLog(program, 1024, nullptr, error_log);
                std::cerr << "Program valid: " << error_log << std::endl;
                glDeleteProgram(program);
                return false;
            }
            delete error_log;
            return true;
        }

        Kuint createShader(GLenum type, const std::string &filename)const {
            Kuint shader = glCreateShader(type);
            if(shader == 0){
                std::cerr << "Create shader failed with type: " << type << std::endl;
                return shader;
            }

            std::string content;
            if(!readFile(filename, content)){
                std::cerr << "Read file: " << filename << " failed!" << std::endl;
                glDeleteShader(shader);
                return 0;
            }

            const char *sources = content.c_str();
            glShaderSource(shader, 1, &sources, nullptr);
            glCompileShader(shader);
            //delete[] sources; make sure to do not delete data int content string
            sources = nullptr;

            GLint flag;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &flag);
            if(flag == GL_FALSE){
				Kint length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
                auto *error_log = new GLchar[length+1];
                glGetShaderInfoLog(shader, length, nullptr, error_log);
                std::cerr << "Error compiling " <<
					(type == GL_VERTEX_SHADER ? "vecter" : "fragment")
					<< " shader: \n" << error_log << std::endl;
                glDeleteShader(shader);
                delete error_log;
                return 0;
            }

            return shader;
        }

        bool addShader(Kuint shader)const {
            if(!glIsShader(shader) || !glIsProgram(program)) return false;
            glAttachShader(program, shader);
            glLinkProgram(program);

            GLint flag;
            glGetProgramiv(program, GL_LINK_STATUS, &flag);
            if(GL_FALSE == flag){
				Kint length;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
                auto *error_log = new GLchar[length + 1];
                glGetProgramInfoLog(program, length, nullptr, error_log);
                std::cerr<<"Error linking program: "<<error_log<<std::endl;
                delete error_log;
                return false;
            }
			uniforms->clear();
            return true;
		}

		bool addShader(GLenum type, const std::string &filename)const {
			return addShader(createShader(type, filename));
		}

        bool isValid()const {
            return glIsProgram(program) == GL_TRUE;
        }

        void bind()const {
            if(glIsProgram(program)) glUseProgram(program);
        }

        void unBind()const {
            glUseProgram(0);
        }

		Kint getAttribLocation(const char* name)const {
			if (!isValid()) return -2;
			return glGetAttribLocation(program, name);
		}

        void bindUniform1f(const std::string &name, Kfloat value)const {
            if(!isValid()) return;
            glUniform1f(getLocation(name), value);
        }

        void bindUniform1i(const std::string &name, Kint value)const {
            if(!isValid()) return;
            glUniform1i(getLocation(name), value);
		}

		void bindUniform2f(const std::string &name, const tvec2& v)const {
			if (!isValid()) return;
			glUniform2fv(getLocation(name), 1, v.data());
			//note: if we set count = 2 and with 2 vector data in,
			//then this location will be set with v[0].data()
			//and next location will be set with v[1].data()
		}

		void bindUniform3f(const std::string &name, const tvec3& v)const {
			if (!isValid()) return;
			glUniform3fv(getLocation(name), 1, v.data());
			//note: if we set count = 2 and with 2 vector data in,
			//then this location will be set with v[0].data()
			//and next location will be set with v[1].data()
		}

		void bindUniform4f(const std::string &name, const tvec4& v)const {
			if (!isValid()) return;
			glUniform4fv(getLocation(name), 1, v.data());
			//note: if we set count = 2 and with 2 vector data in,
			//then this location will be set with v[0].data()
			//and next location will be set with v[1].data()
		}

		void bindUniformMat3(const std::string &name, const tmat3 &m)const {
			if (!isValid()) return;
			glUniformMatrix3fv(getLocation(name), 1, GL_TRUE, m.data()); //our matrix is row-first
		}

		void bindUniformMat4(const std::string &name, const tmat4 &m)const {
			if (!isValid()) return;
			glUniformMatrix4fv(getLocation(name), 1, GL_TRUE, m.data()); //our matrix is row-first
		}
    };
}

#endif //SHADER_H
