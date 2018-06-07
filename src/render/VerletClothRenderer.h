//
// Created by KingSun on 2018/06/07
//

#ifndef VERLET_CLOTH_RENDERER_H
#define VERLET_CLOTH_RENDERER_H

#include "./Renderer.h"
#include "../util/Camera.h"
#include "../util/Light.h"
#include "../object/Plane.h"
#include "../object/Sphere.h"
#include "../object/VerletCloth.h"

namespace KRenderer {
	class VerletClothRenderer : public Renderer {
	private:
		KShader::Shader* back_shader;

		KObject::Plane* floor;
		KObject::Sphere* sphere;
		KObject::VerletCloth* cloth;

		KCamera::Camera* camera;
		KLight::Light* light;

	public:
		VerletClothRenderer(): Renderer(RES_PATH + "phong.vert",
			RES_PATH + "phong.frag", "ClothSimulation"),
			back_shader(nullptr), cloth(nullptr),
			floor(nullptr), sphere(nullptr),
			camera(nullptr), light(nullptr) {
			back_shader = new KShader::Shader();
			back_shader->addShader(GL_VERTEX_SHADER, RES_PATH + "verlet.vert");

			floor = new KObject::Plane(80, 80, 40, 40);
			floor->rotate(90, tvec3(-1, 0, 0));

			sphere = new KObject::Sphere(3, 30, 30);
			sphere->translate(tvec3(0, 2, 0));

			Kuint size_x = 20, size_y = 20;
			cloth = new KObject::VerletCloth(size_x, size_y);
			cloth->setPosition(tvec3(0.f, 3.f, 0.f));

			camera = new KCamera::Camera(tvec3(0, 12, 15));
			tvec2 wSize = window->getWindowSize();
			camera->setPerspective(60.0f, wSize.x / wSize.y, 0.1f, 1000.0f);
			camera->rotateView(24, tvec3(-1, 0, 0));

			light = new KLight::Light(tvec3(0, size_y + 2, 0));
			light->factor = size_y * 0.15;
		}
		~VerletClothRenderer()override {
			delete floor;
			delete sphere;
			delete cloth;
			delete camera;
			delete light;
			delete back_shader;
		}

		void exec()override {
			glEnable(GL_DEPTH_TEST);

#ifdef IMGUI_ENABLE
			Kboolean light_enable = true;
			Kboolean sphere_enable = true;
			Kfloat angle = 0.0;
#endif // IMGUI_ENABLE

			tvec2 wSize;
			tvec2 last_mouse = mouse_pos;
			Kfloat now_time = window->getRunTime();

			back_shader->bind();
			cloth->initBackBuffer(back_shader);
			cloth->bindBackUniform(back_shader);

			shader->bind();
			camera->bindUniform(shader);
			light->bindUniform(shader);

			glCall(;)

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			while (!window->closed()) {
				window->clear();

				wSize = window->getWindowSize();

#ifdef IMGUI_ENABLE
				ImGui_ImplGlfwGL3_NewFrame();
				ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar
					| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

				ImGui::Begin("GUI", nullptr, flags);
				ImGui::SetWindowPos(ImVec2(wSize.x, 0));
				ImGui::SetWindowSize(ImVec2(300, wSize.y));

				ImGui::SetWindowFontScale(1.2);
				ImGui::Text("Your screen now is %.2f fps.", ImGui::GetIO().Framerate);
				ImGui::Text("Your mouse pos is %.0f, %.0f", mouse_pos.x, mouse_pos.y);
				ImGui::Text("Your last mouse pos is %.0f, %.0f", last_mouse.x, last_mouse.y);

				ImGui::Checkbox("light", &light_enable);
				ImGui::SameLine(150);
				ImGui::Checkbox("sphere", &sphere_enable);
				if (light_enable) light->active(shader);
				else light->unActive(shader);

				ImGui::End();
				ImGui::Render();
				ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
#endif // IMGUI_ENABLE

				if (mouse[GLFW_MOUSE_BUTTON_LEFT] &&
					last_mouse.x != mouse_pos.x) {
					static const tvec3 center(0.0f, 1.0f, 0.0f);
					camera->rotateCamera(-atan((mouse_pos.x - last_mouse.x) / 2.0) * 3.0f, center);
					camera->bindPosition(shader);
				}
				last_mouse = mouse_pos;

				//now_time = window->getRunTime() - now_time;
				//if (!KFunction::isZero(now_time)) {
				//	back_shader->bind();
				//	back_shader->bindUniform1f("last_dt", 1.f / 60.f);
				//	back_shader->bindUniform1f("delta_time", 1.f / 60.f);
				//	cloth->renderBack();
				//}
				//now_time = window->getRunTime();

				back_shader->bind();
				cloth->renderBack();

				shader->bind();
				cloth->bindUniform(shader);
				cloth->render();

				floor->bindUniform(shader);
				floor->render();
				floor->unActiveTexture(shader);

				if (sphere_enable) {
					sphere->bindUniform(shader);
					sphere->render();
					sphere->unActiveTexture(shader);
				}

				window->update();
			}
		}

		void resize(Kint w, Kint h)override {
#ifdef IMGUI_ENABLE
			Renderer::resize(w - 300, h);
			if (w > 300 && h > 0) camera->setPerspective(60.0f, Kfloat(w - 300) / Kfloat(h), 0.1f, 1000.0f);
#else
			Renderer::resize(w, h);
			if (w > 0 && h > 0) camera->setPerspective(60.0f, Kfloat(w) / Kfloat(h), 0.1f, 1000.0f);
#endif
			camera->bindUniform(shader);
		}
	};
}

#endif // !VERLET_CLOTH_RENDERER_H

