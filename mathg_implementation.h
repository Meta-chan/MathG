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
#include <ir_resource/ir_lambda_resource.h>
#include "mathg_variables.h"
#include "mathg_object.h"
#include "mathg_vector.h"
#include "mathg_matrix.h"
#include "mathg_basic.h"

char *MathG::_strdup(const char *s) noexcept
{
	unsigned int len = strlen(s);
	char *d = (char*)malloc(len + 1);
	if (d != nullptr) memcpy(d, s, len + 1);
	return d;
};

bool MathG::_get_compile_status(GLuint shader, const char *name) noexcept
{
	GLchar infoLog[512];
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		if (_print)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			printf("Error compiling shader %s\n%s",
				name != nullptr ? name : "?", infoLog);
		}
		return false;
	}
	return true;
};

bool MathG::_get_link_status(GLuint program, const char *name) noexcept
{
	GLchar infoLog[512];
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		if (_print)
		{
			glGetShaderInfoLog(program, 512, nullptr, infoLog);
			printf("Error linking program %s\n%s",
				name != nullptr ? name : "?", infoLog);
		}
		return false;
	}
	return true;
};

bool MathG::_compile_distribute1d() noexcept
{
	if (Shader::_distribute1d == MG_ERROR) return false;
	if (Shader::_distribute1d != MG_UNINITED) return true;
	
	Shader::_distribute1d = glCreateShader(GL_VERTEX_SHADER);
	if (Shader::_distribute1d == MG_ERROR) return false;
	const char *source =
	R"(
		#version 330 core
		layout(location = 0) in float position;
		void main()
		{
			gl_Position = vec4(0.01f, position, 0.0f, 1.0f);
		};
	)";

	glShaderSource(Shader::_distribute1d, 1, &source, nullptr);
	glCompileShader(Shader::_distribute1d);
	if (_get_compile_status(Shader::_distribute1d, "distribute1d")) return true;
	else { glDeleteShader(Shader::_distribute1d); Shader::_distribute1d = MG_ERROR; return false; }
};

bool MathG::_create_distribution1d() noexcept
{
	if (Object::_vbo1d == MG_ERROR) return false;
	if (Object::_vbo1d != MG_UNINITED) return true;

	GLfloat d1[] = { -1.0f, 1.0f };
	glGenVertexArrays(1, &Object::_vao1d);
	if (Object::_vao1d == MG_ERROR) { Object::_vbo1d = MG_ERROR; return false; };
	glBindVertexArray(Object::_vao1d);
	glGenBuffers(1, &Object::_vbo1d);
	if (Object::_vbo1d == MG_ERROR) return false;
	glBindBuffer(GL_ARRAY_BUFFER, Object::_vbo1d);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(GLfloat), d1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	return true;
};

bool MathG::_compile_distribute2d() noexcept
{
	if (Shader::_distribute2d == MG_ERROR) return false;
	if (Shader::_distribute2d != MG_UNINITED) return true;

	Shader::_distribute2d = glCreateShader(GL_VERTEX_SHADER);
	if (Shader::_distribute2d == MG_ERROR) return false;
	const char *source =
		R"(
		#version 330 core
		layout(location = 0) in float vertical;
		layout(location = 1) in float horizontal;
		void main()
		{
			gl_Position = vec4(horizontal, vertical, 0.0f, 1.0f);
		};
	)";

	glShaderSource(Shader::_distribute2d, 1, &source, nullptr);
	glCompileShader(Shader::_distribute2d);
	if (_get_compile_status(Shader::_distribute2d, "distribute2d")) return true;
	else { glDeleteShader(Shader::_distribute2d); Shader::_distribute2d = MG_ERROR; return false; }
};

bool MathG::_create_distribution2d() noexcept
{
	if (Object::_vbo2d == MG_ERROR) return false;
	if (Object::_vbo2d != MG_UNINITED) return true; 
	
	GLfloat d2[] = { 1.0f,	1.0f,
					-1.0f,	1.0f,
					-1.0f,	-1.0f,

					-1.0f,	-1.0f,
					1.0f,	-1.0f,
					1.0f,	1.0f,
	};
	glGenVertexArrays(1, &Object::_vao2d);
	if (Object::_vao2d == MG_ERROR) { Object::_vbo2d = MG_ERROR; return false; }
	glBindVertexArray(Object::_vao2d);
	glGenBuffers(1, &Object::_vbo2d);
	if (Object::_vbo2d == MG_ERROR) return false;
	glBindBuffer(GL_ARRAY_BUFFER, Object::_vbo2d);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), d2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)sizeof(GLfloat));
	glEnableVertexAttribArray(1);
	return true;
};

void MathG::_bind_object(unsigned int count, ObjectG **objects, unsigned int *positions) noexcept
{
	for (unsigned int i = 0; i < count; i++) positions[i] = MG_INVALID_POSITION;
	bool temporary_used[4] = { false, false, false, false };

	//Finding already binded textures
	for (unsigned int i = 0; i < count; i++)
	{
		//Find permanent textures
		if (objects[i]->_permanent != MG_INVALID_POSITION)
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
		if (positions[i] == MG_INVALID_POSITION)
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
		if (positions[i] == MG_INVALID_POSITION)
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

void MathG::_unbind_object(ObjectG *object) noexcept
{
	if (object->_permanent != MG_NOT_PERMANENT)
	{
		_permanent[object->_permanent] = false;
	}
	else
	{
		for (unsigned int i = 0; i < _ntemporary; i++)
		{
			if (object->_texture == _temporary[i])
			{
				_temporary[i] = MG_ERROR;
				return;
			}
		}
	}
};

bool MathG::init(bool print) noexcept
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
	glDisable(GL_MULTISAMPLE);
	
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
	for (unsigned int i = 0; i < _ntemporary; i++) _temporary[i] = MG_ERROR;
	_npermanent = ntextures - 3;
	_permanent = (bool*)malloc(_npermanent * sizeof(bool));
	if (_permanent == nullptr) return false;
	for (unsigned int i = 0; i < _npermanent; i++) _permanent[i] = false;

	_print = print;
	_ok = true;
	return true;
};

unsigned int MathG::submit(const Operation &operation) noexcept
{
	//Compile vertex shader
	if (operation.result_type == ArgumentType::vector)
	{
		if (!_compile_distribute1d()) return MG_ERROR_INDEX;
	}
	else if (operation.result_type == ArgumentType::matrix)
	{
		if (!_compile_distribute2d()) return MG_ERROR_INDEX;
	}
	else return MG_ERROR_INDEX;
	
	//Compiling fragment shader
	ir::LambdaResource<GLuint> shader([](GLuint* s)
	{
		if (*s != MG_ERROR)
		{
			glDeleteShader(*s);
			*s = MG_ERROR;
		}
	});
	shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (shader == MG_ERROR) return false;
	glShaderSource(shader, 1, &operation.source, nullptr);
	glCompileShader(shader);
	if (!_get_compile_status(shader, operation.name)) return false;
	
	//Linking program
	ir::LambdaResource<GLuint> program([](GLuint* p)
	{
		if (*p != MG_ERROR)
		{
			glDeleteProgram(*p);
			*p = MG_ERROR;
		}
	});
	program = glCreateProgram();
	if (program == MG_ERROR) return false;
	glAttachShader(program, Shader::_distribute1d);
	glAttachShader(program, shader);
	glLinkProgram(program);
	if (!_get_link_status(program, operation.name)) return false;

	//Checking uniforms
	for (unsigned int i = 0; i < operation.argument_number; i++)
	{
		if (glGetUniformLocation(program, operation.argument_names[i]) == MG_ERROR)
			return MG_ERROR_INDEX;
	}

	//Creating operation
	if (_operations == nullptr) _operations = (SubmittedOperation*)malloc(sizeof(SubmittedOperation));
	else _operations = (SubmittedOperation*)realloc(_operations, (_noperations + 1) * sizeof(SubmittedOperation));
	if (_operations == nullptr) return MG_ERROR_INDEX;
	_noperations++;
	SubmittedOperation *so = &_operations[_noperations - 1];
	//name
	so->name = operation.name != nullptr ? _strdup(operation.name) : nullptr;
	//source
	so->source = _strdup(operation.source);
	if (so->source == nullptr) return MG_ERROR_INDEX;
	//result_type
	so->result_type = operation.result_type;
	//argument_number
	so->argument_number = operation.argument_number;
	//argument_names
	so->argument_names = (char**)malloc(operation.argument_number * sizeof(char*));
	if (so->argument_names == nullptr) return MG_ERROR_INDEX;
	memset(so->argument_names, 0, operation.argument_number * sizeof(char*));
	for (unsigned int i = 0; i < operation.argument_number; i++)
	{
		so->argument_names[i] = _strdup(operation.argument_names[i]);
		if (so->argument_names[i] == nullptr) return MG_ERROR_INDEX;
	}
	//argument_types
	so->argument_types = (ArgumentType*)malloc(operation.argument_number * sizeof(ArgumentType));
	if (so->argument_types == nullptr) return MG_ERROR_INDEX;
	memcpy(so->argument_types, operation.argument_types, operation.argument_number * sizeof(ArgumentType));
	//argument_locations
	so->argument_locations = (GLuint*)malloc(operation.argument_number * sizeof(GLuint));
	for (unsigned int i = 0; i < operation.argument_number; i++)
		so->argument_locations[i] = glGetUniformLocation(program, operation.argument_names[i]);
	//check
	so->check = operation.check;
	//program
	so->program = program;
	program = MG_ERROR;

	return _noperations - 1;
};

bool MathG::perform(unsigned int operation, Argument *args, ObjectG *r) noexcept
{
	//Checking operation
	if (operation >= _noperations) return false;
	SubmittedOperation *op = &_operations[operation];

	//Finding all object arguments, checking their type and if thay are ok
	ObjectG *objects[3];
	unsigned int positions[3];
	unsigned int nobjects = 0;
	for (unsigned int i = 0; i < op->argument_number; i++)
	{
		ArgumentType typ = op->argument_types[i];
		if (typ == ArgumentType::vector)
		{
			if (!args[i].o->ok() || args[i].o->matrix()) return false;
			objects[nobjects++] = args[i].o;
		}
		else if (typ == ArgumentType::matrix)
		{
			if (!args[i].o->ok() || !args[i].o->matrix()) return false;
			objects[nobjects++] = args[i].o;
		}
	}

	//Compiling distributions, checking result type and if it is ok
	if (!r->ok()) return false;
	if (op->result_type == ArgumentType::vector)
	{
		if (!_create_distribution1d() || !_compile_distribute1d() || r->matrix()) return false;
	}
	else
	{
		if (!_create_distribution1d() || !_compile_distribute2d() || !r->matrix()) return false;
	}

	//User check
	if (op->check != nullptr)
	{
		if (!op->check(args, r)) return false;
	}
	
	//Using program
	glUseProgram(_operations[operation].program);
	glBindVertexArray(op->result_type == ArgumentType::vector ? Object::_vao1d : Object::_vao2d);

	//Binding objects
	_bind_object(nobjects, objects, positions);

	//Setting uniforms
	nobjects = 0;
	for (unsigned int i = 0; i < op->argument_number; i++)
	{
		ArgumentType typ = op->argument_types[i];
		if (typ == ArgumentType::float_) glUniform1f(op->argument_locations[i], args[i].f);
		else if (typ == ArgumentType::int_) glUniform1i(op->argument_locations[i], args[i].i);
		else if (typ == ArgumentType::unsigned_) glUniform1ui(op->argument_locations[i], args[i].u);
		else glUniform1i(op->argument_locations[i], positions[nobjects++]);
	}

	//Creating framebuffer
	if (r->_framebuffer == MG_ERROR)
	{
		glGenFramebuffers(1, &r->_framebuffer);
		if (r->_framebuffer == MG_ERROR) return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, r->_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r->_texture, 0);
	
	//Setting view and executing
	if (op->result_type == ArgumentType::vector)
	{
		glViewport(0, 0, 1, ((VectorG*)r)->height());
		glDrawArrays(GL_LINES, 0, 2);
	}
	else
	{
		glViewport(0, 0, ((MatrixG*)r)->width(), ((MatrixG*)r)->width());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	//Flushing
	glFlush();

	return true;
};

void MathG::free() noexcept
{
	if (Object::_vbo1d != MG_ERROR && Object::_vbo1d != MG_UNINITED) glDeleteBuffers(1, &Object::_vbo1d);
	if (Object::_vbo1d != MG_ERROR && Object::_vbo1d != MG_UNINITED) glDeleteVertexArrays(1, &Object::_vao1d);
	if (Object::_vbo2d != MG_ERROR && Object::_vbo2d != MG_UNINITED) glDeleteBuffers(1, &Object::_vbo2d);
	if (Object::_vbo2d != MG_ERROR && Object::_vbo2d != MG_UNINITED) glDeleteVertexArrays(1, &Object::_vao2d);
	if (Shader::_distribute1d != MG_ERROR && Shader::_distribute1d != MG_UNINITED) glDeleteShader(Shader::_distribute1d);
	if (Shader::_distribute2d != MG_ERROR && Shader::_distribute2d != MG_UNINITED) glDeleteShader(Shader::_distribute2d);
	for (unsigned int i = 0; i < _noperations; i++)
	{
		if (_operations[i].name != nullptr) ::free(_operations[i].name);
		if (_operations[i].source != nullptr) ::free(_operations[i].source);
		if (_operations[i].argument_names != nullptr)
		{
			for (unsigned int j = 0; j < _operations[i].argument_number; j++)
				::free(_operations[i].argument_names[j]);
			::free(_operations[i].argument_names);
		}
		if (_operations[i].argument_types != nullptr) ::free(_operations[i].argument_types);
		if (_operations[i].argument_locations != nullptr) ::free(_operations[i].argument_locations);
		glDeleteProgram(_operations[i].program);
	}
	
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