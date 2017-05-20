#include "StdAfx.h"

#include <Windows.h>
#include <Framework\Application.h>


LRESULT CALLBACK WindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{

	Eruption::Framework::Application* app = new Eruption::Framework::Application(instance, string(cmdLine), WindowProc);
	app->Initialize();
	app->Run();

}