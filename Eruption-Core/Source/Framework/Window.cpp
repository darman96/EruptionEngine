#include "StdAfx.h"
#include "Window.h"

namespace Eruption { namespace Framework {

	Window::Window(const string& title, WindowProperties properties)
		: windowTitle(title), windowProperties(properties), windowHandle(NULL) { }

	void Window::Create(HINSTANCE instance, WNDPROC windowProc) {

		// Create & Register the window class
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.hInstance = instance;
		windowClass.lpszClassName = "EruptionWindowClass";
		windowClass.lpfnWndProc = windowProc;

		RegisterClassEx(&windowClass);

		// Create the window
		windowHandle = CreateWindowEx(	NULL, "EruptionWindowClass", windowTitle.c_str(), 
										WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
										CW_USEDEFAULT, CW_USEDEFAULT, 
										windowProperties.width, windowProperties.height, 
										NULL, NULL, instance, NULL);

		shouldClose = false;
	}

	void Window::ProcessMessages() {
		PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE);

		if (message.message == WM_QUIT)
			shouldClose = true;
		else {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

} }