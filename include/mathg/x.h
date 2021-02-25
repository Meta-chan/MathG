/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_X
#define MATHG_X

#include <GL/glew.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

namespace mathg
{
	///X11 environment
	class X
	{
	private:
		typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
		
		static Display *_display;
		static GLXContext _context;
		static Window _window;
		static Colormap _colormap;
		static bool _ok;
		
		static int _handler(Display *display, XErrorEvent *event)						noexcept;
		static bool _extension_supported(const char *extList, const char *extension)	noexcept;
	public:
		///Initializes X environment
		static bool init()		noexcept;
		///Returns if X environment is ok
		static bool ok()		noexcept;
		///Finalizes X environment
		static void finalize()	noexcept;
	};
}

#ifdef MATHG_IMPLEMENT
	#include "../../source/x.h"
#endif

#endif	//#ifndef MATHG_X
