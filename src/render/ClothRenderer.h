//
// Created by KingSun on 2018/06/04
//

#ifndef CLOTH_RENDERER_H
#define CLOTH_RENDERER_H

#include "./Renderer.h"
#include "../util/Camera.h"
#include "../util/Light.h"
#include "../object/Plane.h"
#include "../object/Sphere.h"
#include "../object/Cloth.h"

namespace KRenderer {
	class ClothRenderer : public Renderer {
	private:
		KObject::Plane* floor;
		KObject::Sphere* sphere;
		KObject::Cloth* cloth;
		
		KCamera::Camera* camera;
		KLight::Light* light;

	public:
		ClothRenderer() : Renderer(RES_PATH + "phong.vert",
			RES_PATH + "phong.frag", "ClothSimulation"),
			floor(nullptr), sphere(nullptr),
			camera(nullptr), light(nullptr) {
			shader->bind();

			floor = new KObject::Plane(80, 80, 40, 40);
			floor->rotate(90, tvec3(-1, 0, 0));

			sphere = new KObject::Sphere(3, 30, 30);
			sphere->translate(tvec3(0, 2, 0));

			Kuint size = 30;
			cloth = new KObject::Cloth(size);

			camera = new KCamera::Camera(tvec3(0, size, size * 2));
			tvec2 wSize = window->getWindowSize();
			camera->setPerspective(60.0f, wSize.x / wSize.y, 0.1f, 1000.0f);
			camera->rotateView(18, tvec3(-1, 0, 0));

			light = new KLight::Light(tvec3(0, size + 2, 0));
			light->factor = size * 0.15;
		}
		~ClothRenderer()override {
			delete floor;
			delete sphere;
			delete cloth;
			delete camera;
			delete light;
		}

		void exec()override {
			glEnable(GL_DEPTH_TEST);

#ifdef IMGUI_ENABLE
			Kboolean light_enable = true;
			Kfloat angle = 0.0;
#endif // IMGUI_ENABLE

			tvec2 wSize;
			tvec2 last_mouse = mouse_pos;
			Kfloat now_time = window->getRunTime();

			camera->bindUniform(shader);
			light->bindUniform(shader);

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

				cloth->drawGui();

				//floor->drawImGui();
				//floor->bindPosition(shader);
				//floor->bindScale(shader);

				//ImGui::DragFloat("angle", &angle, 0.1, 0, 360);
				//camera->setRotation(angle, tvec3(0, 1, 0));
				//camera->bindPosition(shader);

				ImGui::Checkbox("light", &light_enable);
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

				cloth->bindUniform(shader);
				cloth->updatePosition(window->getRunTime() - now_time);
				now_time = window->getRunTime();
				cloth->render();

				floor->bindUniform(shader);
				floor->render();
				floor->unActiveTexture(shader);

				//sphere->bindUniform(shader);
				//sphere->render();
				//sphere->unActiveTexture(shader);

				window->update();
			}
		}

		void resize(Kint w, Kint h)override {
#ifdef IMGUI_ENABLE
			Renderer::resize(w - 300, h);
			camera->setPerspective(60.0f, Kfloat(w - 300) / Kfloat(h), 0.1f, 1000.0f);
#else
			Renderer::resize(w, h);
			camera->setPerspective(60.0f, Kfloat(w) / Kfloat(h), 0.1f, 1000.0f);
#endif
			camera->bindUniform(shader);
		}
	};
}

#endif // !CLOTH_RENDERER_H

