#include <iostream>
#include <windows.h>

namespace render
{
	HHOOK pre_wnd_proc = NULL;

	HDC device_context = NULL;
	HWND game_window = NULL;

	tagRECT window_rect = { 0 };

	POINT recent_position = { 0 };

	COLORREF previous_dc_line_color, previous_dc_text_color = NULL;


	void set_line_color(COLORREF color)
	{
		previous_dc_line_color = SetDCPenColor(device_context, color);
	}

	bool line_to(POINT from, POINT to)
	{
		MoveToEx(device_context, static_cast<int>(from.x), static_cast<int>(from.y), &recent_position);

		return LineTo(device_context, static_cast<int>(to.x), static_cast<int>(to.y));
	}

	void set_text_color(COLORREF color)
	{
		previous_dc_text_color = SetTextColor(device_context, color);
	}

	bool render_text(LPCWSTR text, float fx, float fy)
	{
		return TextOutW(device_context, static_cast<int>(fx), static_cast<int>(fy), text, TA_LEFT);
	}
}

namespace overlay
{
	LRESULT CALLBACK call_wnd_proc(int32_t nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode)
		{
			return CallNextHookEx(render::pre_wnd_proc, nCode, wParam, lParam);
		}

		PCWPSTRUCT hook_parameters = (PCWPSTRUCT)lParam;
		if (hook_parameters->hwnd != render::game_window)
		{
			if ((hook_parameters->message == WM_CLOSE) || (hook_parameters->message == WM_QUIT) || (hook_parameters->message == WM_DESTROY))
			{
				ExitProcess(0);
			}

			GetClientRect(render::game_window, &render::window_rect);

			/*
				Do anything like:
				Call imgui's WndProc
				Cache player list
				etc..

				Get wParam/lParam from hook_parameters, not this function's wParam/lParam..
			*/

			return CallNextHookEx(render::pre_wnd_proc, nCode, wParam, lParam);;
		}
	}

}

int main()
{
	render::game_window = FindWindowW(NULL, L"Fortnite  ");
	if (!render::game_window)
	{
		// FindWindowW failed..
	}

	render::set_text_color(RGB(255, 0, 0));
	render::set_line_color(RGB(255, 0, 0));

	render::pre_wnd_proc = SetWindowsHookExW(WH_CALLWNDPROC, &overlay::call_wnd_proc, NULL, NULL);
	if (!render::pre_wnd_proc)
	{
		// SetWindowsHookExW failed..
	}

	/*
		If we exit the process, our callbacks will be deleted too..
		ExitThread() exits the thread only, not the process.
		On Windows, Microsoft's CRT exits the process once main() returns.
		In other words, if you simply return 0 within main(), you'll exit the process.
	*/

	ExitThread(0);
}
