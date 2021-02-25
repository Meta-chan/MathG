/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_EGL_SOURCE
#define MATHG_EGL_SOURCE

EGLDisplay mathg::EGL::_display	= EGL_NO_DISPLAY;
EGLSurface mathg::EGL::_surface	= EGL_NO_SURFACE;
EGLContext mathg::EGL::_context = EGL_NO_CONTEXT;
bool mathg::EGL::_inintialized	= false;
bool mathg::EGL::_ok			= false;

bool mathg::EGL::init() noexcept
{
	if (_inintialized) return _ok;
	_inintialized = true;

	const EGLint config_attributes[] = {
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
			EGL_NONE };

	const EGLint pbuffer_attributes[] = {
			EGL_WIDTH, 9,
			EGL_HEIGHT, 9,
			EGL_NONE,
	};

	const EGLint context_attributes[] = {
			EGL_CONTEXT_MAJOR_VERSION, 3,
			EGL_CONTEXT_MINOR_VERSION, 3,
			EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
			EGL_NONE,
	};

	//Initializing EGL
	_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (_display == EGL_NO_DISPLAY) return false;
	EGLint major, minor;
	if (eglInitialize(_display, &major, &minor) == EGL_FALSE) return false;
	EGLint config_number;
	EGLConfig config;
	if (!eglChooseConfig(_display, config_attributes, &config, 1, &config_number)) return false;
	_surface = eglCreatePbufferSurface(_display, config, pbuffer_attributes);
	if (_surface == EGL_NO_SURFACE) return false;
	if (!eglBindAPI(EGL_OPENGL_API)) return false;
	_context = eglCreateContext(_display, config, EGL_NO_CONTEXT, context_attributes);
	if (_context == EGL_NO_CONTEXT) return false;
	if (!eglMakeCurrent(_display, _surface, _surface, _context)) return false;

	_ok = true;
	return true;
}

bool mathg::EGL::ok() noexcept
{
	return _ok;
}

void mathg::EGL::finalize() noexcept
{
	if (_context != EGL_NO_CONTEXT)
	{
		eglDestroyContext(_display, _context);
		_context = EGL_NO_CONTEXT;
	}

	if (_surface != EGL_NO_SURFACE)
	{
		eglDestroySurface(_display, _surface);
		_surface = EGL_NO_SURFACE;
	}

	if (_display != EGL_NO_DISPLAY)
	{
		eglTerminate(_display);
		_display = EGL_NO_DISPLAY;
	}

	_inintialized = false;
	_ok = false;
}

#endif	//#ifndef MATHG_EGL_SOURCE