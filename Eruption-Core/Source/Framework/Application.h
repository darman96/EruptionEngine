#pragma once

namespace Eruption { namespace Graphics {
	class RenderSystemVulkan;
} }

namespace Eruption { namespace Framework {

	struct WindowProperties;
	class Window;


	class Application {

	public:

	private:

		HINSTANCE applicationInstance;
		WNDPROC windowProc;
		const string& commandLine;

		Window* window;

		Graphics::RenderSystemVulkan* renderSystemVK;
		

	public:

		Application(HINSTANCE instance, const string& cmdLine, WNDPROC wndProc);
		virtual ~Application();

		void Initialize();

		void Run();

	private:

	};

} }