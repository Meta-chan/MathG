/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_IMPLEMENTATION
#define MATHG_IMPLEMENTATION

#ifdef IR_MATHG_FREEGLUT
	#ifdef _WIN32	
		#include <shellapi.h>
	#else
		#include <unistd.h>
	#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ir_resource/ir_shader_resource.h>
#include "mathg_source.h"
#include "mathg_variables.h"
#include "mathg_vector.h"
#include "mathg_matrix.h"

bool MathG::_get_status(const char *name, bool link)
{
	GLchar infoLog[512];
	GLint success;
	glGetShaderiv(Shader::_distribute1d, link ? GL_LINK_STATUS : GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(Shader::_distribute1d, 512, nullptr, infoLog);
		printf("Error %s %s %s\n%s",
			link ? "linking" : "compiling",
			link ? "program" : "shader",
			name, infoLog);
		return false;
	}
	return true;
};

bool MathG::_compile_distribute1d()
{
	if (Shader::_distribute1d != GL_ERR) return true;
	 Shader::_distribute1d = glCreateShader(GL_VERTEX_SHADER);
	if (Shader::_distribute1d == GL_ERR) return false;
	glShaderSource(Shader::_distribute1d, 1, &Source::_distribute1d, nullptr);
	glCompileShader(Shader::_distribute1d);
	return _get_status("distribute1d", false);
};

bool MathG::_create_distribution1d()
{
	if (Object::_vbo1d != GL_ERR) return true;
	GLfloat d1[] = { -1.0f, 1.0f };
	glGenVertexArrays(1, &Object::_vao1d);
	if (Object::_vao1d == GL_ERR) return false;
	glBindVertexArray(Object::_vao1d);
	glGenBuffers(1, &Object::_vbo1d);
	if (Object::_vbo1d == GL_ERR) return false;
	glBindBuffer(GL_ARRAY_BUFFER, Object::_vbo1d);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(GLfloat), d1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	return true;
};

bool MathG::_compile_distribute2d()
{
	if (Shader::_distribute2d != GL_ERR) return true;
	Shader::_distribute2d = glCreateShader(GL_VERTEX_SHADER);
	if (Shader::_distribute2d == GL_ERR) return false;
	glShaderSource(Shader::_distribute2d, 1, &Source::_distribute2d, nullptr);
	glCompileShader(Shader::_distribute2d);
	return _get_status("distribute2d", false);
};

bool MathG::_create_distribution2d()
{
	if (Object::_vbo2d != GL_ERR) return true;
	GLfloat d2[] = { 1.0f,	1.0f,
					-1.0f,	1.0f,
					-1.0f,	-1.0f,

					-1.0f,	-1.0f,
					1.0f,	-1.0f,
					1.0f,	1.0f,
	};
	glGenVertexArrays(1, &Object::_vao2d);
	if (Object::_vao2d == GL_ERR) return false;
	glBindVertexArray(Object::_vao2d);
	glGenBuffers(1, &Object::_vbo2d);
	if (Object::_vbo2d == GL_ERR) return false;
	glBindBuffer(GL_ARRAY_BUFFER, Object::_vbo2d);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), d2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)sizeof(GLfloat));
	glEnableVertexAttribArray(1);
	return true;
};

bool MathG::_compile(GLuint *program, const char *source, const char *name, bool d2)
{
	if (*program != GL_ERR) return true;
	if (!_compile_distribute1d()) return false;
	if (!_create_distribution1d()) return false;
	ir::ShaderResource shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (shader == GL_ERR) return false;
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	if (!_get_status(name, false)) return false;
	*program = glCreateProgram();
	if (*program == GL_ERR) return false;
	glAttachShader(*program, Shader::_distribute1d);
	glAttachShader(*program, shader);
	glLinkProgram(*program);
	if (!_get_status(name, true)) return false;
	return true;
};

bool MathG::_compile_add_vvv()
{
	if (!_compile(&Program::AddVVV::_program, Source::_add_vvv, "add_vvv", false)) return false;
	if((Program::AddVVV::_a = glGetUniformLocation(Program::AddVVV::_program, "a")) == -1
	|| (Program::AddVVV::_b = glGetUniformLocation(Program::AddVVV::_program, "b")) == -1) return false;
	return true;
};

bool MathG::_compile_subtract_vvv()
{
	if (!_compile(&Program::SubtractVVV::_program, Source::_subtract_vvv, "subtract_vvv", false)) return false;
	if((Program::SubtractVVV::_a = glGetUniformLocation(Program::SubtractVVV::_program, "a")) == -1
	|| (Program::SubtractVVV::_b = glGetUniformLocation(Program::SubtractVVV::_program, "b")) == -1) return false;
	return true;
};

bool MathG::_compile_multiply_mvv()
{
	if (!_compile(&Program::MultiplyMVV::_program, Source::_multiply_mvv, "multiply_mvv", false)) return false;
	if((Program::MultiplyMVV::_a = glGetUniformLocation(Program::MultiplyMVV::_program, "a")) == -1
	|| (Program::MultiplyMVV::_b = glGetUniformLocation(Program::MultiplyMVV::_program, "b")) == -1
	|| (Program::MultiplyMVV::_awidth = glGetUniformLocation(Program::MultiplyMVV::_program, "awidth")) == -1) return false;
	return true;
};

void MathG::_bind_object(unsigned int count, ObjectG **objects, unsigned int *positions)
{
	for (unsigned int i = 0; i < count; i++) positions[i] = 0xFFFFFFFF;
	bool temporary_used[4] = { false, false, false, false };

	//Finding already binded textures
	for (unsigned int i = 0; i < count; i++)
	{
		//Find permanent textures
		if (objects[i]->_permanent != 0xFFFFFFFF)
		{
			positions[i] = objects[i]->_permanent;
		}
		//Find temporary textures
		else
		{
			for (unsigned int j = 0; j < _ntemporary; j++)
			{
				if (objects[i]->_texture == _temporary[j])
				{
					temporary_used[j] = true;
					positions[i] = _npermanent + j;
					break;
				}
			}
		}
	}

	//Assign position to new textures
	for (unsigned int i = 0; i < count; i++)
	{
		//Try to find place in permanent textures
		if (positions[i] == 0xFFFFFFFF)
		{
			for (unsigned int j = 0; j < _npermanent; j++)
			{
				if (!_permanent[j])
				{
					objects[i]->_permanent = j;
					_permanent[j] = true;
					positions[i] = j;
					glActiveTexture(GL_TEXTURE0 + j);
					glBindTexture(GL_TEXTURE_2D, objects[i]->_texture);
					break;
				}
			}
		}

		//Find place in temporary storage
		if (positions[i] == 0xFFFFFFFF)
		{
			for (unsigned int j = 0; j < _ntemporary; j++)
			{
				if (!temporary_used[j])
				{
					temporary_used[j] = true;
					_temporary[j] = objects[i]->_texture;
					positions[i] = _npermanent + j;
					glActiveTexture(GL_TEXTURE0 + _npermanent + j);
					glBindTexture(GL_TEXTURE_2D, objects[i]->_texture);
					break;
				}
			}
		}
	}
};

void MathG::_unbind_object(ObjectG *object)
{
	if (object->_permanent != 0xFFFFFFFF)
	{
		_permanent[object->_permanent] = false;
	}
	else
	{
		for (unsigned int i = 0; i < _ntemporary; i++)
		{
			if (object->_texture == _temporary[i])
			{
				_temporary[i] = GL_ERR;
				return;
			}
		}
	}
};

bool MathG::init(bool print)
{
#if defined(IR_MATHG_FREEGLUT)
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
	_window = glutCreateWindow("MathG");
	if (_window < 1) return false;
	glutHideWindow();
	glutDisplayFunc([](){});
	glDisable(GLUT_MULTISAMPLE);
#elif defined(IR_MATHG_SDL2)
	//Initializing SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	_window = SDL_CreateWindow("MathG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	100, 100, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

	//Initializing GLEW
	if (glewInit() != GLEW_OK) return false;

	//Initializing tables
	GLint ntextures;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &ntextures);
	if (ntextures < 3) return false;
	_ntemporary = 3;
	_temporary = (GLuint*)malloc(_ntemporary * sizeof(GLuint));
	if (_temporary == nullptr) return false;
	for (unsigned int i = 0; i < _ntemporary; i++) _temporary[i] = GL_ERR;
	_npermanent = ntextures - 3;
	_permanent = (bool*)malloc(_npermanent * sizeof(bool));
	if (_permanent == nullptr) return false;
	for (unsigned int i = 0; i < _npermanent; i++) _permanent[i] = false;

	_ok = true;
	return true;
};

bool MathG::add_vvv(VectorG *a, VectorG *b, VectorG *r)
{
	if (!_ok) return false;
	if (!a->_ok || !b->_ok || !r->_ok) return false;
	if (a == b || a == r || b == r) return false;
	if (a->_height != b->_height || a->_height != r->_height) return false;
	if (!_compile_add_vvv()) return false;
	glUseProgram(Program::AddVVV::_program);
	glBindVertexArray(MathG::Object::_vao1d);
	ObjectG *objects[2] = { a, b };
	unsigned int positions[2];
	_bind_object(2, objects, positions);
	glUniform1i(Program::AddVVV::_a, positions[0]);
	glUniform1i(Program::AddVVV::_b, positions[1]);
	if (r->_framebuffer == GL_ERR)
	{
		glGenFramebuffers(1, &r->_framebuffer);
		if (r->_framebuffer == GL_ERR) return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, r->_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r->_texture, 0);
	glViewport(0, 0, 1, r->_height);
	glDrawArrays(GL_LINES, 0, 2);
	glFinish();
	return false;
};

bool MathG::subtract_vvv(VectorG *a, VectorG *b, VectorG *r)
{
	if (!_ok) return false;
	if (!a->_ok || !b->_ok || !r->_ok) return false;
	if (a == b || a == r || b == r) return false;
	if (a->_height != b->_height || a->_height != r->_height) return false;
	if (!_compile_subtract_vvv()) return false;
	glUseProgram(Program::SubtractVVV::_program);
	glBindVertexArray(MathG::Object::_vao1d);
	ObjectG *objects[2] = { a, b };
	unsigned int positions[2];
	_bind_object(2, objects, positions);
	glUniform1i(Program::SubtractVVV::_a, positions[0]);
	glUniform1i(Program::SubtractVVV::_b, positions[1]);
	if (r->_framebuffer == GL_ERR)
	{
		glGenFramebuffers(1, &r->_framebuffer);
		if (r->_framebuffer == GL_ERR) return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, r->_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r->_texture, 0);
	glViewport(0, 0, 1, r->_height);
	glDrawArrays(GL_LINES, 0, 2);
	glFinish();
	return false;
};

bool MathG::multiply_mvv(MatrixG *a, VectorG *b, VectorG *r)
{
	if (!_ok) return false;
	if (!a->_ok || !b->_ok || !r->_ok) return false;
	if (b == r) return false;
	if (a->_width != b->_height || a->_height != r->_height) return false;
	if (!_compile_multiply_mvv()) return false;
	glUseProgram(Program::MultiplyMVV::_program);
	glBindVertexArray(MathG::Object::_vao1d);
	ObjectG *objects[2] = { a, b };
	unsigned int positions[2];
	_bind_object(2, objects, positions);
	glUniform1i(Program::MultiplyMVV::_a, positions[0]);
	glUniform1i(Program::MultiplyMVV::_b, positions[1]);
	glUniform1i(Program::MultiplyMVV::_awidth, a->_width);
	if (r->_framebuffer == GL_ERR)
	{
		glGenFramebuffers(1, &r->_framebuffer);
		if (r->_framebuffer == GL_ERR) return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, r->_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r->_texture, 0);
	glViewport(0, 0, 1, r->_height);
	glDrawArrays(GL_LINES, 0, 2);
	glFinish();
	return false;
};

void MathG::free()
{
	if (Object::_vbo1d != GL_ERR) glDeleteBuffers(1, &Object::_vbo1d);
	if (Object::_vao1d != GL_ERR) glDeleteVertexArrays(1, &Object::_vao1d);
	if (Object::_vbo2d != GL_ERR) glDeleteBuffers(1, &Object::_vbo2d);
	if (Object::_vao2d != GL_ERR) glDeleteVertexArrays(1, &Object::_vao2d);
	if (Shader::_distribute1d != GL_ERR) glDeleteShader(Shader::_distribute1d);
	if (Shader::_distribute2d != GL_ERR) glDeleteShader(Shader::_distribute2d);
	if (Program::AddVVV::_program != GL_ERR) glDeleteProgram(Program::AddVVV::_program);
	if (Program::SubtractVVV::_program != GL_ERR) glDeleteProgram(Program::SubtractVVV::_program);
	if (Program::MultiplyMVV::_program != GL_ERR) glDeleteProgram(Program::MultiplyMVV::_program);
	#if defined(IR_MATHG_FREEGLUT)
		if (_window) glutDestroyWindow(_window);
	#elif defined(IR_MATHG_SDL2)
		if (_context != nullptr) SDL_GL_DeleteContext(_context);
		if (_window != nullptr) SDL_DestroyWindow(_window);
	#else
		if (_display != EGL_NO_DISPLAY) eglTerminate(_display);
	#endif
	_ok = false;
};

#endif	//#ifndef MATHG_IMPLEMENTATION
