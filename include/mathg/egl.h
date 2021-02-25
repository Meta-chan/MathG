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
#include <EGL/egl.h>

namespace mathg
{
	///EGL environment
	class EGL
	{
	private:
		static EGLDisplay _display;

	public:
		///Initializes EGL environment
		static bool init()		noexcept;
		///Returns if EGL environment is ok
		static bool ok()		noexcept;
		///Finalizes EGL environment
		static void finalize()	noexcept;
	};
}

#ifdef MATHG_IMPLEMENT
	#include "../../source/egl.h"
#endif

#endif	//#ifndef MATHG_EGL