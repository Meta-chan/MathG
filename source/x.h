/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_X_SOURCE
#define MATHG_X_SOURCE

#include <string.h>

Display *mathg::X::_display		= nullptr;
GLXContext mathg::X::_context	= 0;
Window mathg::X::_window		= 0;
Colormap mathg::X::_colormap	= 0;
bool mathg::X::_initialized		= false;
bool mathg::X::_ok				= false;

bool mathg::X::_extension_supported(const char *extList, const char *extension) noexcept
{
	const char *start  = extList;
	const char *where = strchr(extension, ' ');
	if (where || *extension == '\0') return false;
	while (true)
	{
		where = strstr(start, extension);
		if (!where) return false;
		const char *terminator = where + strlen(extension);
		if ( where == start || *(where - 1) == ' ' )
		{
			if ( *terminator == ' ' || *terminator == '\0' ) return true;
		}
		start = terminator;
	}
}

int mathg::X::_handler(Display *display, XErrorEvent *event) noexcept
{
	_ok = false;
	return 0;
}


bool mathg::X::init() noexcept
{
	//khronos.org/opengl/wiki/Tutorial:_OpenGL_3.0_Context_Creation_(GLX)
	if (_initialized) return _ok;
	_initialized = true;

	//Open display
	_display = XOpenDisplay(NULL);
	if (_display == nullptr) return false;
	
	static int visual_attributes[] =
	{
		GLX_X_RENDERABLE,	True,
		GLX_DRAWABLE_TYPE,	GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,	GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE,	GLX_TRUE_COLOR,
		GLX_RED_SIZE,		8,
		GLX_GREEN_SIZE,		8,
		GLX_BLUE_SIZE,		8,
		GLX_ALPHA_SIZE,		8,
		GLX_DEPTH_SIZE,		24,
		GLX_STENCIL_SIZE,	8,
		GLX_DOUBLEBUFFER,	False,
		None
	};

 
	//Check OpenGL version is at least 1.3
	int major, minor;
	if (!glXQueryVersion(_display, &major, &minor ) || (major < 1) || (major == 1 && minor < 3)) return false;
	
	//Getting matching framebuffer configs
	int fbcount;
	GLXFBConfig* fbconfig = glXChooseFBConfig(_display, DefaultScreen(_display), visual_attributes, &fbcount);
	if (!fbconfig) return false;
	
	// Pick the FB config/visual with the most samples per pixel
	int max_samples_index	= -1;
	int min_samples_index	= -1;
	int max_samples			= -1;
	int min_samples			= 999;
	for (int i = 0; i < fbcount; i++)
	{
		XVisualInfo *visual_info = glXGetVisualFromFBConfig(_display, fbconfig[i]);
		if (visual_info != nullptr)
		{
			int sample_buffer;
			int samples;
			glXGetFBConfigAttrib(_display, fbconfig[i], GLX_SAMPLE_BUFFERS, &sample_buffer);
			glXGetFBConfigAttrib(_display, fbconfig[i], GLX_SAMPLES, &samples);

			if (max_samples_index < 0 || sample_buffer && samples > max_samples)
			{
				max_samples_index = i;
				max_samples = samples;
			}

			if (min_samples_index < 0 || !sample_buffer || samples < min_samples)
			{
				min_samples_index = i;
				min_samples = samples;
			}

			XFree(visual_info);
		}
	}
	GLXFBConfig best_fbconfig = fbconfig[max_samples_index];
	XFree(fbconfig);

	// Get a visual
	XVisualInfo *visual_info = glXGetVisualFromFBConfig(_display, best_fbconfig);

	//Create colormap
	XSetWindowAttributes window_attributes;
	_colormap = XCreateColormap(_display, RootWindow(_display, visual_info->screen ), visual_info->visual, AllocNone);
	window_attributes.colormap			= _colormap;
	window_attributes.background_pixmap = None;
	window_attributes.border_pixel      = 0;
	window_attributes.event_mask        = StructureNotifyMask;

	//Creating window
	_window = XCreateWindow(_display, RootWindow(_display, visual_info->screen), 0, 0, 100, 100, 0, visual_info->depth, InputOutput, visual_info->visual, CWBorderPixel|CWColormap|CWEventMask, &window_attributes);
	XFree(visual_info);
	if (_window == 0) return false;	
	XStoreName(_display, _window, "MathG" );

	//Mapping window
	XMapWindow(_display, _window);

	// Get the default screen's GLX extension list
	const char *glx_extensions = glXQueryExtensionsString(_display, DefaultScreen(_display));

	// NOTE: It is not necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

	// Install an X error handler so the application won't exit if GL 3.0 context allocation fails.
	int (*old_handler)(Display*, XErrorEvent*) = XSetErrorHandler(&_handler);

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, exit
	if (!_extension_supported(glx_extensions, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) return false;

	// Try to get a GL 3.3 context!
	int context_attributes[] =
	{
		0x2091,	3,	//GLX_CONTEXT_MAJOR_VERSION_ARB
		0x2092,	3,	//GLX_CONTEXT_MINOR_VERSION_ARB
		//GLX_CONTEXT_FLAGS_ARB,	GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None
	};
	_context = glXCreateContextAttribsARB(_display, best_fbconfig, 0, True, context_attributes);
	if (_context == 0)
	{
		XSetErrorHandler(old_handler);
		return false;
	}

	// Sync to ensure any errors generated are processed.
	//If thare are, we have 2.0 context - I'm still considering it
	_ok = true;
	XSync(_display, False);
	XSetErrorHandler(old_handler);
	if (!_ok) return false;
	_ok = false;

	//Making context current
	if (!glXMakeCurrent(_display, _window, _context)) return false;

	_ok = true;
	return true;
}

bool mathg::X::ok() noexcept
{
	return _ok;
}

void mathg::X::finalize() noexcept
{
	//Make context not current
	if (_display != nullptr) glXMakeCurrent(_display, 0, 0 );
	
	//Destroy context
	if (_display != nullptr && _context != 0)
	{
		glXDestroyContext(_display, _context);
		_context = 0;
	}
	
	//Destroy window
	if (_window != 0)
	{
		XDestroyWindow(_display, _window);
		_window = 0;
	}

	//Destroy colormap
	if (_colormap != 0)
	{
		XFreeColormap(_display, _colormap);
		_colormap = 0;
	}

	//Destroy display
	if (_display != nullptr)
	{
		XCloseDisplay(_display);
		_display = nullptr;
	}

	_initialized = false;
	_ok = false;
}

#endif	//#ifndef MATHG_X_SOURCE