//
// Created by KingSun on 2018/05/14
//

#include "Header.h"
#include "Window.h"
#include "./render/Shader.h"
#include "./util/Camera.h"
#include "./util/Light.h"
#include "./object/Plane.h"
#include "./object/Sphere.h"

int main() {
	auto *window = new KWindow::Window("ClothSimulation");
	auto *shader = new KShader::Shader(RES_PATH + "phong.vert", RES_PATH + "phong.frag");
	shader->bind();

	auto floor = new KObject::Plane(40, 40, 40, 40);
	floor->rotate(90, KVector::Vec3(-1, 0, 0));

	auto sphere = new KObject::Sphere(3, 30, 30);
	sphere->translate(KVector::Vec3(0, 2, 0));

	auto camera = new KCamera::Camera(90, 1.0, 0.1, 100);
	camera->setPosition(KVector::Vec3(0, 5, 10));
	camera->rotateView(30, KVector::Vec3(-1, 0, 0));

	auto light = new KLight::Light(KVector::Vec3(0, 10, 0));
	light->factor = 1.5;

	glEnable(GL_DEPTH_TEST);

#ifdef IMGUI_ENABLE
	Kboolean movable = false;
	Kboolean resizable = false;
	Kboolean layout_mode = true;
	Kboolean light_enable = true;
	KVector::Vec2 screenSize;
	KVector::Vec2 mouse_pos;
	Kfloat angle = 0.0;
#endif // IMGUI_ENABLE

	camera->bindUniform(shader);
	light->bindUniform(shader);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!window->closed()) {
		window->clear();

#ifdef IMGUI_ENABLE
		ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;
		if (!movable) flags |= ImGuiWindowFlags_NoMove;
		if (!resizable) flags |= ImGuiWindowFlags_NoResize;
		mouse_pos = window->getMouse();
		screenSize = window->getWindowSize();

		ImGui::Begin("GUI", nullptr, flags);

		ImGui::SetWindowFontScale(1.2);
		if (!movable) {
			if (layout_mode) ImGui::SetWindowPos(ImVec2(screenSize.x - 300, 0));
			else ImGui::SetWindowPos(ImVec2(0, 0));
		}
		if (!resizable) {
			if (layout_mode) ImGui::SetWindowSize(ImVec2(300, screenSize.y));
			else ImGui::SetWindowSize(ImVec2(screenSize.x, 200));
		}
		ImGui::Checkbox("movable", &movable);
		ImGui::SameLine(100);
		ImGui::Checkbox("resizable", &resizable);
		ImGui::SameLine(200);
		ImGui::Checkbox("layout", &layout_mode);

		ImGui::Text("Your screen now is %.2f fps.", ImGui::GetIO().Framerate);
		ImGui::Text("Your mouse pos is %.0f, %.0f", mouse_pos.x, mouse_pos.y);

		//floor->drawImGui();
		//floor->bindPosition(shader);
		//floor->bindScale(shader);
		
		ImGui::DragFloat("angle", &angle, 0.1, 0, 360);
		camera->setRotation(angle, KVector::Vec3(0, 1, 0));
		camera->bindPosition(shader);

		ImGui::Checkbox("light", &light_enable);
		if (light_enable) light->active(shader);
		else light->unActive(shader);

		ImGui::End();

		if (layout_mode && !movable) glViewport(0, 0, screenSize.x - 300, screenSize.y);
		else glViewport(0, 0, screenSize.x, screenSize.y);

#endif // IMGUI_ENABLE

		floor->bindUniform(shader);
		floor->render();

		sphere->bindUniform(shader);
		sphere->render();
		
		window->update();
	}

	delete floor;
	delete camera;
	delete light;
	delete shader;
	delete window;
}