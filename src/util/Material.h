 //
// Created by KingSun on 2018/5/3.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#define STB_IMAGE_IMPLEMENTATION

#include <string>
#include <vector>
#include <GL/glew.h>
#include <stb_image.h>
#include "../Header.h"
#include "../render/Shader.h"
#include "../math/Vec4.h"

namespace KMaterial {
	using tcolor = KVector::Vec4;

    class Material{
	public:
		static const tcolor BLACK;
		static const tcolor RED;
		static const tcolor BLUE;
		static const tcolor GREEN;
		static const tcolor ORANGE;
		static const tcolor YELLOW;
		static const tcolor PINK;
		static const tcolor WHITE;
		static const tcolor GREY;

        tcolor ambient;
        tcolor diffuse;
        tcolor specular;
        Kfloat shininess;

	private:
		const static std::string AMBIENT; //u_ambient
		const static std::string DIFFUSE; //u_diffuse
		const static std::string SPECULAR; //u_specular
		const static std::string SHININESS; //u_shininess
		const static std::string TEXTURE; //u_texture.tex
		const static std::string TEX_ENABLE; //u_texture.enable

		Kuint tex_id;

    public:

        Material(const tcolor &ka = WHITE, const tcolor &kd = GREY,
                    const tcolor &ks = GREY, Kfloat shininess = 1.0f):
			shininess(shininess), ambient(ka), diffuse(kd), specular(ks), tex_id(0) {}

        Material(const std::string &path, const tcolor &ka = WHITE, const tcolor &kd = GREY, const tcolor &ks = GREY,
                 Kfloat shininess = 1.0f):
			ambient(ka), diffuse(kd), specular(ks), shininess(shininess) {
			setTexture(path);
		}

        ~Material(){
			if (glIsTexture(tex_id)) glDeleteTextures(1, &tex_id);
        }

        void setTexture(const std::string &path) {
			if (glIsTexture(tex_id)) glDeleteTextures(1, &tex_id);
			glGenTextures(1, &tex_id);

			int width, height, component;
			GLubyte *data = stbi_load(path.data(), &width, &height, &component, 0);
			if (data != nullptr) {
				GLenum format;
				switch (component) {
				case 1:
					format = GL_RED;
					break;
				case 3:
					format = GL_RGB;
					break;
				case 4:
					format = GL_RGBA;
					break;
				default:
					format = GL_RGB;
				}

				glBindTexture(GL_TEXTURE_2D, tex_id);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 6);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			} else {
				std::cerr << "Load texture file: " << path << " failed!" << std::endl;
				glDeleteTextures(1, &tex_id);
				tex_id = 0;
			}
        }

		void bindUniform(const KShader::Shader *shader)const {
			shader->bindUniform4f(AMBIENT, ambient);
			shader->bindUniform4f(DIFFUSE, diffuse);
			shader->bindUniform4f(SPECULAR, specular);
			shader->bindUniform1f(SHININESS, shininess);
			if (glIsTexture(tex_id)) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex_id);
				shader->bindUniform1i(TEXTURE, GL_TEXTURE0);
				shader->bindUniform1i(TEX_ENABLE, true);
			}
		}

		void activeTexture(const KShader::Shader *shader)const {
			shader->bindUniform1i(TEX_ENABLE, true);
		}

		void unactiveTexture(const KShader::Shader *shader)const {
			shader->bindUniform1i(TEX_ENABLE, false);
		}
    };

	const std::string Material::AMBIENT("u_ambient");
	const std::string Material::DIFFUSE("u_diffuse");
	const std::string Material::SPECULAR("u_specular");
	const std::string Material::SHININESS("u_shininess");
	const std::string Material::TEXTURE("u_texture.tex");
	const std::string Material::TEX_ENABLE("u_texture.enable");

	const tcolor Material::BLACK(0, 0, 0, 1.0f);
	const tcolor Material::RED(1.0f, 0, 0, 1.0f);
	const tcolor Material::BLUE(0, 1.0f, 0, 1.0f);
	const tcolor Material::GREEN(0, 0, 1.0f, 1.0f);
	const tcolor Material::ORANGE(1.0f, 1.0f, 0, 1.0f);
	const tcolor Material::YELLOW(0, 1.0f, 1.0f, 1.0f);
	const tcolor Material::PINK(1.0f, 0, 1.0f, 1.0f);
	const tcolor Material::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
	const tcolor Material::GREY(0.8, 0.8, 0.8, 1.0);
}

#endif //MATERIAL_H
