//
// Created by KingSun on 2018/5/14.
//

#ifndef WINDOW_H
#define WINDOW_H

#include "Header.h"
#include "./math/Vec2.h"

namespace KWindow {
	class Window {
		friend void window_size_callback(GLFWwindow* window, int width, int height);
		friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		friend static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		friend void cursor_enter_callback(GLFWwindow* window, int entered);
		friend void window_iconify_callback(GLFWwindow* window, int iconified);
		friend void error_callback(int error, const char* description);

	private:
		Kdouble run_time, pause_time;
		Kuint width, height;
		Kdouble mx, my;
		std::string title;
		Kboolean keys[512]; //-1, 32-162, 256-248
		Kboolean mouse[3]; //left, right, wheel
		Kboolean is_active;
		Kboolean is_focus;
		GLFWwindow *window;

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
			glfwSetWindowUserPointer(window, this);

			if (glewInit() != GLEW_OK) {
				std::cerr << "GLEW initial failed!" << std::endl;
				glfwDestroyWindow(window);
				glfwTerminate();
				return false;
			}

			glClearColor(0.17, 0.17, 0.17, 1.0);

#ifdef IMGUI_ENABLE
			// Setup ImGui binding
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
			ImGui_ImplGlfwGL3_Init(window, true);

			// Setup style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();
#endif // IMGUI_ENABLE

			std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

			return true;
		}

		void initAction() {
			glfwSetWindowSizeCallback(window, window_size_callback);
			glfwSetKeyCallback(window, key_callback);
			glfwSetMouseButtonCallback(window, mouse_button_callback);
			glfwSetScrollCallback(window, scroll_callback);
			glfwSetCursorPosCallback(window, cursor_position_callback);
			glfwSetCursorEnterCallback(window, cursor_enter_callback);
			glfwSetErrorCallback(error_callback);

			glfwGetCursorPos(window, &mx, &my);
			glfwSwapInterval(1);
			//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //hide the mouse pointer
			is_active = true;
			run_time = 0;
			pause_time = 0;
		}

		void resize(Kint w, Kint h) {
			width = w;
			height = h;
			std::cout << "Window resized with " << w << ", " << h << std::endl;
			glViewport(0, 0, width, height);
		}

		void keyEvent(Kint key, Kint action) {
			keys[key] = action == GLFW_PRESS;
			if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		void mouseEvent(Kint button, Kint action) {
			mouse[button] = action == GLFW_PRESS;
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				is_active = !is_active;
				//std::cout << "Pointer at: " << mx << ", " << my << std::endl;
				//std::cout << "Run times: " << run_time << std::endl;
				//std::cout << "Pause times: " << pause_time << std::endl;
			}
		}

		void mouseWheelEvent(Kdouble yoffset) {
			//if (is_focus) std::cout << "Yaxis scroll: " << yoffset << std::endl;
		}

		void cursorEvent(Kdouble xpos, Kdouble ypos) {
			if (is_focus) {
				mx = xpos; my = ypos;
				//if (mouse[GLFW_MOUSE_BUTTON_LEFT])
					//std::cout << "Pointer at: " << mx << ", " << my << std::endl;
			}
		}

		void focusEvent(bool focused) {
			is_focus = focused;
		}

		void iconifiedEvent(bool iconified) {
			is_active = is_active && !iconified;
		}

	public:
		Window(const std::string &title, Kint width = 1000, Kint height = 700) :
			title(title), width(width), height(height) {
			if (!initGL()) {
				std::cerr << "Create window failed!" << std::endl;
				is_active = false;
				return;
			}

			initAction();
		}
		~Window() {
			ImGui_ImplGlfwGL3_Shutdown();
			ImGui::DestroyContext();
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		bool closed()const {
			return glfwWindowShouldClose(window) == GLFW_TRUE;
		}

		void clear() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef IMGUI_ENABLE
			ImGui_ImplGlfwGL3_NewFrame();
#endif // IMGUI_ENABLE

		}

		void update() {
			if (is_active) {
				run_time = glfwGetTime() - pause_time;
			}
			else {
				pause_time = glfwGetTime() - run_time;
			}
#ifdef IMGUI_ENABLE
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
#endif // IMGUI_ENABLE
			glfwPollEvents();
			glfwSwapBuffers(window);
		}

		void setTitle(const std::string& title) {
			this->title = title;
			glfwSetWindowTitle(window, this->title.c_str());
		}

		inline Kdouble getRunTime()const {
			return run_time;
		}

		inline KVector::Vec2 getMouse()const {
			return KVector::Vec2(mx, my);
		}

		inline KVector::Vec2 getWindowSize()const {
			return KVector::Vec2(width, height);
		}
	};

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win != nullptr) win->resize(width, height);
		win = nullptr;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win != nullptr) win->keyEvent(key, action);
		win = nullptr;
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win != nullptr) win->mouseEvent(button, action);
		win = nullptr;
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win != nullptr) win->mouseWheelEvent(yoffset);
		win = nullptr;
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win != nullptr) win->cursorEvent(xpos, ypos);
		win = nullptr;
	}

	void cursor_enter_callback(GLFWwindow* window, int entered) {
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win != nullptr) win->focusEvent(entered != GLFW_FALSE);
		win = nullptr;
	}

	void window_iconify_callback(GLFWwindow* window, int iconified) {
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win != nullptr) win->iconifiedEvent(iconified != GLFW_FALSE);
		win = nullptr;
	}

	void error_callback(int error, const char* description) {
		std::cerr << "An error occured with an error code: " << error << " and discription: "
			<< description << std::endl;
	}
}

#endif //WINDOW_H
