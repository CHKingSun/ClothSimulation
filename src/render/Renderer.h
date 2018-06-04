//
// Created by KingSun on 2018/05/28
//

#ifndef RENDERER_H
#define RENDERER_H

#include "../Header.h"
#include "../Window.h"
#include "./Shader.h"
#include "../math/Vec4.h"

namespace KRenderer {
	using tvec2 = KVector::Vec2;
	using tvec3 = KVector::Vec3;

	class Renderer {
		friend void window_size_callback(GLFWwindow* window, int width, int height);
		friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		friend static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		friend void cursor_enter_callback(GLFWwindow* window, int entered);
		friend void window_iconify_callback(GLFWwindow* window, int iconified);

	protected:
		KWindow::Window* window;
		KShader::Shader* shader;
		Kboolean keys[512]; //-1, 32-162, 256-248
		Kboolean mouse[3]; //left, right, wheel

		tvec2 mouse_pos;

		void initAction() {
			glfwSetWindowSizeCallback(window->window, window_size_callback);
			glfwSetKeyCallback(window->window, key_callback);
			glfwSetMouseButtonCallback(window->window, mouse_button_callback);
			glfwSetScrollCallback(window->window, scroll_callback);
			glfwSetCursorPosCallback(window->window, cursor_position_callback);
			glfwSetCursorEnterCallback(window->window, cursor_enter_callback);

			Kdouble mx, my;
			glfwGetCursorPos(window->window, &mx, &my);
			mouse_pos.x = mx, mouse_pos.y = my;
		}

	protected:
		Renderer(const std::string& v_shader, const std::string& f_shader,
			const std::string& title, Ksize swidth = 1000, Ksize sheight = 700) :
			window(nullptr), shader(nullptr) {
			window = new KWindow::Window(title, swidth, sheight);
			shader = new KShader::Shader(v_shader, f_shader);
			if (window->actived()) {
				glfwSetWindowUserPointer(window->window, this);
				initAction();
			}
		}

		virtual void keyEvent(Kint key, Kint action) {
			keys[key] = action != GLFW_RELEASE;
			if (keys[GLFW_KEY_ESCAPE]) {
				window->is_active = !(window->is_active);
				if (window->is_active) {
					glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else {
					glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
			}
		}

		virtual void mouseEvent(Kint button, Kint action) {
			mouse[button] = action == GLFW_PRESS;
		}

		virtual void mouseWheelEvent(Kdouble yoffset) {
			//if(is_focus) std::cout << "Yaxis scroll: " << yoffset << std::endl;
		}

		virtual void cursorEvent(Kdouble xpos, Kdouble ypos) {
			mouse_pos.x = xpos; mouse_pos.y = ypos;
			//if(mouse[GLFW_MOUSE_BUTTON_LEFT]) 
			//	std::cout << "Pointer at: " << mx << ", " << my << std::endl;
		}

		virtual void focusEvent(bool focused) {
			window->is_focus = focused;
		}

		virtual void iconifiedEvent(bool iconified) {
			window->is_active = window->is_active && !iconified;
		}

	public:
		virtual ~Renderer() {
			delete shader;
			delete window;
		}

		virtual void exec() = 0;

		virtual void resize(Kint w, Kint h) {
			window->resize(w, h);
		}

		void hideMouse(Kboolean hide = true)const {
			if (hide) {
				glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				//hide the mouse pointer, and also the mouse pointer will not be limited to window size.
			}
			else {
				glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}

		const tvec2& getMouse()const {
			return mouse_pos;
		}
	};


	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		auto win = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		if (win != nullptr) win->resize(width, height);
		win = nullptr;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		if (renderer != nullptr) renderer->keyEvent(key, action);
		renderer = nullptr;
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		if (renderer != nullptr) renderer->mouseEvent(button, action);
		renderer = nullptr;
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		if (renderer != nullptr) renderer->mouseWheelEvent(yoffset);
		renderer = nullptr;
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
		auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		if (renderer != nullptr) renderer->cursorEvent(xpos, ypos);
		renderer = nullptr;
	}

	void cursor_enter_callback(GLFWwindow* window, int entered) {
		auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		if (renderer != nullptr) renderer->focusEvent(entered != GLFW_FALSE);
		renderer = nullptr;
	}

	void window_iconify_callback(GLFWwindow* window, int iconified) {
		auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
		if (renderer != nullptr) renderer->iconifiedEvent(iconified != GLFW_FALSE);
		renderer = nullptr;
	}
}

#endif // !RENDERER_H
