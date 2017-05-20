#include "StdAfx.h"
#include "Application.h"

#include "Window.h"
#include "Graphics\RenderSystemVulkan.h"

namespace Eruption { namespace Framework {
	
	Application::Application(HINSTANCE instance, const string& cmdLine, WNDPROC wndProc)
		: applicationInstance(instance), commandLine(cmdLine), windowProc(wndProc) { }

	Application::~Application() {
		delete(window);
	}

	void Application::Initialize() {
		// Initialize Logging System
		INIT_LOG();	// Creates a Debug Console if Debug mode is enabled
		LOG_SUCCESS("Logging", "Log System initialized!");
		LOG_INFO("Application", "Initializing Game Engine...");

		LOG_INFO("Application", "Creating Engine Window...");
		window = new Window("Eruption Engine", WindowProperties(800, 600, false));
		window->Create(applicationInstance, windowProc);
		LOG_NEW_LINE();
		
		LOG_INFO("Application", "Creating Render System Vulkan...");
		renderSystemVK = new Graphics::RenderSystemVulkan();
		renderSystemVK->Initialize(window->GetHandle());
		LOG_NEW_LINE();

	}

	void Application::Run() {

		while (!window->ShouldClose()) {
			window->ProcessMessages();
		}

	}

} }