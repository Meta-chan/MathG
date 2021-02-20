/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifdef MATHG_FREEGLUT
	#ifdef _WIN32	
		#include <shellapi.h>
	#else
		#include <unistd.h>
	#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const GLuint mathg::System::_error			= (GLuint)-1;
GLuint mathg::System::_vao2d					= mathg::System::_error;
GLuint mathg::System::_vbo2d					= mathg::System::_error;
GLuint mathg::System::_distribute2d			= mathg::System::_error;
GLint mathg::System::_max_binded				= 0;
GLint mathg::System::_max_accessed			= 0;
std::vector<mathg::System::TextureUnit> mathg::System::_texture_units;
bool mathg::System::_ok						= false;
bool mathg::System::_initialized				= false;
#if defined(MATHG_FREEGLUT)
	int mathg::System::_window				= -1;
#elif defined(MATHG_SDL2)
	SDL_Window *mathg::System::_window		= nullptr;
	SDL_GLContext mathg::System::_context	= NULL;
#else
	EGLDisplay mathg::System::_display		= nullptr;
#endif

bool mathg::System::_gl_ok() noexcept
{
	bool ok = true;
	while (true)
	{
		GLint i = glGetError();
		if (i == GL_NO_ERROR) break;
		else ok = false;
	}
	return ok;
}

bool mathg::System::_bind_matrixes(size_t n, Matrix **matrixes, GLint *positions) noexcept
{
	//Dear future me (or anyone who reads this)
	//I have been writing this algorithm all the night. 14th to 15th December. I'm tired
	//My girlfriend sent me a Valentine, but I wish she have just talked to me
	//I wish I were a better person

	//Mark all as unbinded
	for (size_t i = 0; i < n; i++) positions[i] = -1;

	//Find already binded textures
	size_t unbinded = n;
	for (GLenum i = 0; i < _texture_units.size(); i++)
	{
		for (size_t j = 0; j < n; j++)
		{
			if (positions[j] == -1 && _texture_units[i].texture == matrixes[j]->_texture)
			{
				_texture_units[i].age = 0;
				positions[j] = i;
				unbinded--;
				if (unbinded == 0) return true;
				break;
			}
		}
	}
	
	//Bind textures in free units
	for (GLenum i = 0; i < _texture_units.size(); i++)
	{
		for (size_t j = 0; j < n; j++)
		{
			if (positions[j] == -1 && _texture_units[i].texture == _error)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, matrixes[j]->_texture);
				_texture_units[i].age = 0;
				_texture_units[i].texture = matrixes[j]->_texture;
				positions[j] = i;
				unbinded--;
				if (unbinded == 0) return true;
				break;
			}
		}
	}

	//Bind textures in new units
	for (size_t j = 0; j < n; j++)
	{
		if (positions[j] == -1 && _texture_units.size() < _max_binded)
		{
			_texture_units.push_back(TextureUnit());
			glActiveTexture(GL_TEXTURE0 + (GLenum)_texture_units.size() - 1);
			glBindTexture(GL_TEXTURE_2D, matrixes[j]->_texture);
			_texture_units.back().age = 0;
			_texture_units.back().texture = matrixes[j]->_texture;
			positions[j] = (GLenum)_texture_units.size() - 1;
			unbinded--;
			if (unbinded == 0) return true;
		}
	}

	//Update bind count
	for (GLenum i = 0; i < _texture_units.size(); i++)
	{
		if (_texture_units[i].age != (size_t)-1) _texture_units[i].age++;
	}

	//Bind textures in occupied units
	for (size_t j = 0; j < n; j++)
	{
		if (positions[j] == -1)
		{
			size_t oldest_age = 0;
			GLenum oldest = 0;
			for (GLint i = 0; i < _texture_units.size(); i++)
			{
				if (_texture_units[i].age > oldest_age)
				{
					oldest_age = _texture_units[i].age;
					oldest = i;
				}
			}

			glActiveTexture(GL_TEXTURE0 + oldest);
			glBindTexture(GL_TEXTURE_2D, matrixes[j]->_texture);
			_texture_units[oldest].age = 0;
			_texture_units[oldest].texture = matrixes[j]->_texture;
			positions[j] = oldest;
			unbinded--;
			if (unbinded == 0) return true;
		}
	}

	return true;
}
	
void mathg::System::_unbind_matrix(Matrix *matrix) noexcept
{
	for (GLenum i = 0; i < _texture_units.size(); i++)
	{
		if (matrix->_texture == _texture_units[i].texture)
		{
			_texture_units[i].texture = System::_error;
			_texture_units[i].age = (size_t)-1;
		}
	}
}

bool mathg::System::init() noexcept
{
	if (_initialized) return _ok;
	_initialized = true;

	#if defined(MATHG_FREEGLUT)
		//Making argv
		#ifdef _WIN32
			int argc;
			LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
			if (wargv == nullptr) return false;
			char **argv = (char**)calloc(argc, sizeof(char*));
			if (argv == nullptr) return false;
			char undefchar = '?';
			for (int i = 0; i < argc; i++)
			{
				int len = WideCharToMultiByte(CP_ACP, 0, wargv[i], -1, NULL, 0, &undefchar, NULL);
				argv[i] = (char*)calloc(len, sizeof(char));
				if (argv[i] == nullptr) return false;
				WideCharToMultiByte(CP_ACP, 0, wargv[i], -1, argv[i], len, &undefchar, NULL);
			}
			LocalFree(wargv);
		#else
			char arg0[1024];
			char *parg0 = arg0;
			readlink("/proc/self/exe", parg0, 1024);
			char **argv = &parg0;
			int argc = 1;
		#endif
	
		//Initializing Freeglut
		glutInit(&argc, argv);
		glutInitDisplayMode(0);
		glutInitContextVersion(3, 3);
		glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
		glutInitContextProfile(GLUT_CORE_PROFILE);
		glutInitWindowSize(100, 100);
		_window = glutCreateWindow("System");
		if (_window < 1) return false;
		glutHideWindow();
		glutDisplayFunc([](){});
	
	#elif defined(MATHG_SDL2)
		//Initializing SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		_window = SDL_CreateWindow("System", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	100, 100, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (_window == nullptr) return false;
		_context = SDL_GL_CreateContext(_window);
		if (_context == nullptr) return false;
	#else
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
	#endif

	//Disabling multisampling
	glDisable(GL_MULTISAMPLE);

	//Initializing GLEW
	if (glewInit() != GLEW_OK) return false;

	//Initializing table
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_max_accessed);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_max_binded);

	//Creating 2D distribution shader
	_distribute2d = glCreateShader(GL_VERTEX_SHADER);
	if (_distribute2d == _error) return false;
	const char *source = R"(
		#version 330 core
		layout(location = 0) in float vertical;
		layout(location = 1) in float horizontal;
		void main()
		{
			gl_Position = vec4(horizontal, vertical, 0.0f, 1.0f);
		};
	)";
	glShaderSource(_distribute2d, 1, &source, nullptr);
	glCompileShader(_distribute2d);
	GLint success;
	glGetShaderiv(_distribute2d, GL_COMPILE_STATUS, &success);
	if (!success) return false;

	//Creating and binding 2D distribution objects
	GLfloat d2[] = { 1.0f,	1.0f,
					-1.0f,	1.0f,
					-1.0f,	-1.0f,

					-1.0f,	-1.0f,
					1.0f,	-1.0f,
					1.0f,	1.0f,
	};
	glGenVertexArrays(1, &_vao2d);
	if (_vao2d == _error) return false;
	glBindVertexArray(_vao2d);
	glGenBuffers(1, &_vbo2d);
	if (_vbo2d == _error) return false;
	glBindBuffer(GL_ARRAY_BUFFER, _vbo2d);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), d2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)sizeof(GLfloat));
	glEnableVertexAttribArray(1);

	if (!_gl_ok()) return false;
	_ok = true;
	return true;
}

bool mathg::System::ok() noexcept
{
	return _ok;
}

void mathg::System::finalize() noexcept
{
	if (_vbo2d != _error) glDeleteBuffers(1, &_vbo2d);
	if (_vbo2d != _error) glDeleteVertexArrays(1, &_vao2d);
	if (_distribute2d != _error) glDeleteShader(_distribute2d);
	
	#if defined(MATHG_FREEGLUT)
		if (_window) glutDestroyWindow(_window);
	#elif defined(MATHG_SDL2)
		if (_context != nullptr) SDL_GL_DeleteContext(_context);
		if (_window != nullptr) SDL_DestroyWindow(_window);
	#else
		if (_display != EGL_NO_DISPLAY) eglTerminate(_display);
	#endif
	_ok = false;
	_initialized = false;
}