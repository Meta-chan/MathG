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

EGLDisplay mathg::EGL::_display = EGL_NO_DISPLAY;

bool mathg::EGL::init() noexcept
{
	const EGLint configAttribs[] = {
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
			EGL_NONE };

	const int pbufferWidth = 9;
	const int pbufferHeight = 9;

	static const EGLint pbufferAttribs[] = {
			EGL_WIDTH, pbufferWidth,
			EGL_HEIGHT, pbufferHeight,
			EGL_NONE,
	};

	//Initializing EGL
	_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (_display == EGL_NO_DISPLAY) return false;
	EGLint major, minor;
	if (eglInitialize(_display, &major, &minor) == EGL_FALSE) return false;
	EGLint numConfigs;
	EGLConfig eglCfg;
	if (!eglChooseConfig(_display, configAttribs, &eglCfg, 1, &numConfigs)) return false;
	EGLSurface eglSurf = eglCreatePbufferSurface(_display, eglCfg, pbufferAttribs);
	if (eglSurf == EGL_NO_SURFACE) return false;
	if (!eglBindAPI(EGL_OPENGL_API)) return false;
	EGLContext eglCtx = eglCreateContext(_display, eglCfg, EGL_NO_CONTEXT, NULL);
	if (eglCtx == EGL_NO_CONTEXT) return false;
	eglMakeCurrent(_display, eglSurf, eglSurf, eglCtx);
	return true;
}

bool mathg::EGL::ok() noexcept
{
	return eglCtx != EGL_NO_CONTEXT;
}

void mathg::EGL::finalize() noexcept
{
	if (_display != EGL_NO_DISPLAY)
	{
		eglTerminate(_display);
		_display = EGL_NO_DISPLAY;
	}
}

#endif	//#ifndef MATHG_SDL2_SOURCE