//
// Created by KingSun on 2018/4/26.
//

#ifndef LIGHT_H
#define LIGHT_H

#include <ostream>
#include <string>
#include "../Header.h"
#include "../render/Shader.h"
#include "../math/Vec3.h"
#include "../math/Vec4.h"
#include "./Material.h"

namespace KLight {
	using namespace KMaterial;

    class Light{
	private:
		const static std::string U_ENABLE; //enable
		const static std::string U_POSITION; //position
		const static std::string U_FACTOR; //factor
		const static std::string U_AMBIENT; //ambient
		const static std::string U_DIFFUSE; //diffuse
		const static std::string U_SPECULAR; //specular
		const static std::string U_KC; //kc
		const static std::string U_KL; //kl
		const static std::string U_KQ; //kq

    public:
        using tcolor = KVector::Vec4;
		using tvec3 = KVector::Vec3;

        Kfloat factor; //Light intensity

        tvec3 position;

        tcolor ambient;
        tcolor diffuse;
		tcolor specular;

		Kfloat kc, kl, kq; //attenuation factor

		Light(): factor(1.0), position(tvec3(0, 0, 0)), ambient(Material::GREY),
			diffuse(Material::GREY), specular(Material::GREY),
			kc(1.0), kl(0.045), kq(0.0075) {}

		Light(const tvec3 &pos): factor(1.0), position(pos),
			diffuse(Material::GREY), specular(Material::GREY),
			kc(1.0), kl(0.045), kq(0.0075) {}

		Light(const tvec3 &pos, const tcolor &ambient,
			const tcolor &diffuse, const tcolor &specular):
			factor(1.0), position(pos), diffuse(diffuse), specular(specular),
			kc(1.0), kl(0.045), kq(0.0075) {}

		Light(const tvec3 &pos, const tcolor &ambient, const tcolor &diffuse,
			const tcolor &specular, const Kfloat &factor):
			factor(1.0), position(pos), diffuse(diffuse), specular(specular),
			kc(1.0), kl(0.045), kq(0.0075) {}

		Light(const tvec3 &pos, const tcolor &ambient, const tcolor &diffuse,
			const tcolor &specular, const Kfloat &factor,
			const Kfloat &kc, const Kfloat &kl, const Kfloat &kq):
			factor(1.0), position(pos), diffuse(diffuse), specular(specular),
			kc(kc), kl(kl), kq(kq) {}

		void bindUniform(const KShader::Shader* shader)const {
			shader->bindUniform1i(U_ENABLE, true);
			shader->bindUniform1f(U_FACTOR, factor);
			shader->bindUniform3f(U_POSITION, position);
			shader->bindUniform4f(U_AMBIENT, ambient);
			shader->bindUniform4f(U_DIFFUSE, diffuse);
			shader->bindUniform4f(U_SPECULAR, specular);
			shader->bindUniform1f(U_KC, kc);
			shader->bindUniform1f(U_KL, kl);
			shader->bindUniform1f(U_KQ, kq);
		}

		void bindPosition(const const KShader::Shader* shader)const {
			shader->bindUniform3f(U_POSITION, position);
		}

		void active(const KShader::Shader* shader)const {
			shader->bindUniform1i(U_ENABLE, true);
		}

		void unActive(const KShader::Shader* shader)const {
			shader->bindUniform1i(U_ENABLE, false);
		}
    };

	const std::string Light::U_ENABLE = "u_light.enable";
	const std::string Light::U_POSITION = "u_light.position";
	const std::string Light::U_FACTOR = "u_light.factor";
	const std::string Light::U_AMBIENT = "u_light.ambient";
	const std::string Light::U_DIFFUSE = "u_light.diffuse";
	const std::string Light::U_SPECULAR = "u_light.specular";
	const std::string Light::U_KC = "u_light.kc";
	const std::string Light::U_KL = "u_light.kl";
	const std::string Light::U_KQ = "u_light.kq";
}

#endif //LIGHT_H
