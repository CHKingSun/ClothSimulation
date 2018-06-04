//
// Created by KingSun on 2018/5/4.
//

#ifndef KENGINE_WINDOW_H
#define KENGINE_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "./Header.h"
#include "./math/Vec2.h"
#include "./math/Quaternion.h"

namespace KRenderer { class	Renderer; }

namespace KWindow {
	class Window {
		friend class KRenderer::Renderer;

	private:
		Ksize width, height;
		std::string title;
		GLFWwindow *window;

		Kboolean is_active;
		Kboolean is_focus;
		Kdouble run_time, pause_time;

		Kboolean initGL() {
			if (!glfwInit()) {
				window = nullptr;
				return false;
			}

			window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
			if (!window) {
				glfwTerminate();
				return false;
			}
			glfwSetWindowPos(window, 120, 120);
			glfwMakeContextCurrent(window);

			if (glewInit() != GLEW_OK) {
				std::cerr << "GLEW initial failed!" << std::endl;
				glfwDestroyWindow(window);
				glfwTerminate();
				return false;
			}

			glClearColor(0.17f, 0.17f, 0.17f, 1.0f);
			glfwSwapInterval(1);

#ifdef IMGUI_ENABLE
			// Setup ImGui binding
			ImGui::CreateContext();
			//ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
			ImGui_ImplGlfwGL3_Init(window, true);

			// Setup style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();
			resize(width - 300, height);
#endif

			std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
			is_active = true;
			run_time = 0;
			pause_time = 0;

			return true;
		}

	public:
		Window(const std::string &title, Kint width = 800, Kint height = 800) :
			title(title), width(width), height(height) {
			if (!initGL()) {
				std::cerr << "Create window failed!" << std::endl;
				return;
			}
		}
		~Window() {
#ifdef IMGUI_ENABLE
			ImGui_ImplGlfwGL3_Shutdown();
			ImGui::DestroyContext();
#endif
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		void resize(Kint w, Kint h) {
			width = w;
			height = h;
			//std::cout << "Window resized with " << w << ", " << h << std::endl;
			glViewport(0, 0, width, height);
		}

		bool closed()const {
			return glfwWindowShouldClose(window) == GLFW_TRUE;
		}

		bool actived()const {
			return is_active;
		}

		void closeWindow()const {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		void clear() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void setClearColor(const KVector::Vec4& color = KVector::Vec4(0.17f, 0.17f, 0.17f, 1.0f)) {
			glClearColor(color.r, color.g, color.b, color.a);
		}

		void update() {
			if (is_active) {
				run_time = glfwGetTime() - pause_time;
			}
			else {
				pause_time = glfwGetTime() - run_time;
			}
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		void setTitle(const std::string& title) {
			this->title = title;
			glfwSetWindowTitle(window, this->title.c_str());
		}

		void resetTime() {
			run_time = 0;
			pause_time = 0;
		}

		inline const Kdouble getRunTime()const {
			return run_time;
		}

		inline KVector::Vec2 getWindowSize()const {
			return KVector::Vec2(width, height);
		}
	};
}

#endif //KENGINE_WINDOW_H
