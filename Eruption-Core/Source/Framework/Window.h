#pragma once

namespace Eruption { namespace Framework {
	
	struct WindowProperties {

		uint width;
		uint height;
		bool fullscreen;

		WindowProperties(uint width, uint height, bool fullscreen) 
			: width(width), height(height), fullscreen(fullscreen)
		{
		}

		WindowProperties() : width(0), height(0), fullscreen(false) {}

	};

	class Window {

	public:

	private:

		string windowTitle;
		WindowProperties windowProperties;

		HWND windowHandle;
		MSG message;

		bool shouldClose;

	public:

		Window(const string& title, WindowProperties properties);

		void Create(HINSTANCE instance, WNDPROC windowProc);
		void ProcessMessages();

		// Getter & Setters
		inline const string& GetTitle() { return windowTitle; }
		inline const WindowProperties& GetProperties() { return windowProperties; }
		inline const HWND GetHandle() { return windowHandle; }
		inline const bool ShouldClose() { return shouldClose; }

		inline void SetTitle(const string& title) { windowTitle = title; }
		inline void SetWindowProperties(WindowProperties properties) { windowProperties = properties; }

	private:

	};

} }