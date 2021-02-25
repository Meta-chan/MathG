/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_EGL
#define MATHG_EGL

#include <GL/glew.h>
#include <Windows.h>

namespace mathg
{
	///Window environment
	class Windows
	{
	private:
		static HDC _dc;
		static HGLRC _gc;
		static WNDCLASS _window_class;
		static HWND _window;
		static bool _initialized;
		static bool _ok;

		static LRESULT CALLBACK _window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	public:
		///Initializes Windows environment
		static bool init()		noexcept;
		///Returns if Windows environment is ok
		static bool ok()		noexcept;
		///Finalizes Windows environment
		static void finalize()	noexcept;
	};
}

#ifdef MATHG_IMPLEMENT
	#include "../../source/windows.h"
#endif

#endif	//#ifndef MATHG_EGL