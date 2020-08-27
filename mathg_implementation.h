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

bool MathG::_get_status(const char *name, bool link) noexcept
{
	GLchar infoLog[512];
	GLint success;
	glGetShaderiv(Shader::_distribute1d, link ? GL_LINK_STATUS : GL_COMPILE_STATUS, &success);
	if (!success)
	{
		if (_print)
		{
			glGetShaderInfoLog(Shader::_distribute1d, 512, nullptr, infoLog);
			printf("Error %s %s %s\n%s",
				link ? "linking" : "compiling",
				link ? "program" : "shader",
				name, infoLog);
		}
		return false;
	}
	return true;
};

bool MathG::_compile_distribute1d() noexcept
{
	if (Shader::_distribute1d != GL_ERR) return true;
	 Shader::_distribute1d = glCreateShader(GL_VERTEX_SHADER);
	if (Shader::_distribute1d == GL_ERR) return false;
	const char *source =
	R"(
		#version 330 core
		layout(location = 0) in float position;
		void main()
		{
			gl_Position = vec4(0.5f, position, 0.0f, 1.0f);
		};
	)";

	glShaderSource(Shader::_distribute1d, 1, &source, nullptr);
	glCompileShader(Shader::_distribute1d);
	return _get_status("distribute1d", false);
};

bool MathG::_create_distribution1d() noexcept
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

bool MathG::_compile_distribute2d() noexcept
{
	if (Shader::_distribute2d != GL_ERR) return true;
	Shader::_distribute2d = glCreateShader(GL_VERTEX_SHADER);
	if (Shader::_distribute2d == GL_ERR) return false;
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
	return _get_status("distribute2d", false);
};

bool MathG::_create_distribution2d() noexcept
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

void MathG::_bind_object(unsigned int count, ObjectG **objects, unsigned int *positions) noexcept
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

void MathG::_unbind_object(ObjectG *object) noexcept
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

unsigned int MathG::submit(const Operation &operation) noexcept
{
	//Compile vertex shader
	if (operation.result_type == ArgumentType::vector)
	{
		if (!_compile_distribute1d()) return 0xFFFFFFFF;
	}
	else if (operation.result_type == ArgumentType::matrix)
	{
		if (!_compile_distribute2d()) return 0xFFFFFFFF;
	}
	else return 0xFFFFFFFF;
	
	//Compiling fragment shader
	ir::LambdaResource<GLuint> shader([](GLuint* s)
	{
		if (*s != GL_ERR)
		{
			glDeleteShader(*s);
			*s = GL_ERR;
		}
	});
	shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (shader == GL_ERR) return false;
	glShaderSource(shader, 1, &operation.source, nullptr);
	glCompileShader(shader);
	if (!_get_status(operation.name, false)) return false;
	
	//Linking program
	ir::LambdaResource<GLuint> program([](GLuint* p)
	{
		if (*p != GL_ERR)
		{
			glDeleteProgram(*p);
			*p = GL_ERR;
		}
	});
	program = glCreateProgram();
	if (program == GL_ERR) return false;
	glAttachShader(program, Shader::_distribute1d);
	glAttachShader(program, shader);
	glLinkProgram(program);
	if (!_get_status(operation.name, true)) return false;

	//Finding uniforms
	for (unsigned int i = 0; i < operation.argument_number; i++)
	{
		if (glGetUniformLocation(program, operation.argument_names[i]) == GL_ERR)
			return 0xFFFFFFFF;
	}

	//Creating operation
	if (_operations == nullptr) _operations = (SubmittedOperation*)malloc(sizeof(SubmittedOperation));
	else _operations = (SubmittedOperation*)realloc(_operations, (_noperations + 1) * sizeof(SubmittedOperation));
	if (_operations == nullptr) return 0xFFFFFFFF;
	_noperations++;
	SubmittedOperation *so = &_operations[_noperations - 1];
	*((Operation*)so) = operation;
	if(((so->argument_names = (const char**)malloc(operation.argument_number * sizeof(char*))) == nullptr)
	|| ((so->argument_types = (ArgumentType*)malloc(operation.argument_number * sizeof(ArgumentType))) == nullptr)
	|| ((so->argument_locations = (GLuint*)malloc(operation.argument_number * sizeof(GLuint))) == nullptr)) return 0xFFFFFFFF;
	memcpy(so->argument_names, operation.argument_names, operation.argument_number * sizeof(char*));
	memcpy(so->argument_types, operation.argument_types, operation.argument_number * sizeof(ArgumentType));
	for (unsigned int i = 0; i < operation.argument_number; i++)
		so->argument_locations[i] = glGetUniformLocation(program, operation.argument_names[i]);
	so->program = program;
	
	program = GL_ERR;
	return _noperations - 1;
};

bool MathG::perform(unsigned int operation, Argument *args, ObjectG *r) noexcept
{
	if (operation > _noperations) return false;
	SubmittedOperation *op = &_operations[operation];
	if (op->result_type == ArgumentType::vector) _compile_distribute1d();
	else _compile_distribute2d;

	ObjectG *objects[3];
	unsigned int positions[3];
	unsigned int nobjects = 0;
	for (unsigned int i = 0; i < op->argument_number; i++)
	{
		ArgumentType typ = op->argument_types[i];
		if (typ == ArgumentType::vector || typ == ArgumentType::matrix)
		{
			if (!args[i].o->ok() || (typ != ArgumentType::matrix ^ args[i].o->matrix())) return false;
			objects[nobjects++] = args[i].v;
		}
	}
	glUseProgram(_operations[operation].program);
	glBindVertexArray(op->result_type == ArgumentType::vector ? Object::_vao1d : Object::_vao2d);
	_bind_object(nobjects, objects, positions);
	nobjects = 0;
	for (unsigned int i = 0; i < op->argument_number; i++)
	{
		ArgumentType typ = op->argument_types[i];
		if (typ == ArgumentType::float_) glUniform1f(op->argument_locations[i], args[i].f);
		else if (typ == ArgumentType::int_) glUniform1i(op->argument_locations[i], args[i].i);
		else if (typ == ArgumentType::unsigned_) glUniform1i(op->argument_locations[i], args[i].u);
		else glUniform1i(op->argument_locations[i], positions[nobjects++]);
	}
	if (r->_framebuffer == GL_ERR)
	{
		glGenFramebuffers(1, &r->_framebuffer);
		if (r->_framebuffer == GL_ERR) return false;
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r->_texture, 0);
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
	glFlush();
};

bool MathG::add_vvv(VectorG *a, VectorG *b, VectorG *r) noexcept
{
	if (!_ok) return false;
	if (Index::_add_vvv == 0xFFFFFFFE)
	{
		const char *names[2] = { "a", "b" };
		ArgumentType types[2] = { ArgumentType::vector, ArgumentType::vector };
		Operation op;
		op.argument_number = 2;
		op.argument_names = names;
		op.argument_types = types;
		op.result_type = ArgumentType::vector;
		op.check = [](Argument* arguments, ObjectG *result) -> bool
		{
			return(arguments[0].v->height() == arguments[1].v->height()
				&& arguments[0].v->height() == ((VectorG*)result)->height());
		};
		op.source =
		R"(
			#version 330 core
			uniform sampler2D a;
			uniform sampler2D b;
			out float c;
			void main()
			{
				int position = int(gl_FragCoord.y - 0.5);
				c = texelFetch(a, ivec2(0, position), 0).r + texelFetch(b, ivec2(0, position), 0).r;
			};
		)";
		Index::_add_vvv = submit(op);
	}
	if (Index::_add_vvv == 0xFFFFFFFF) return false;
	Argument arguments[2];
	arguments[0].v = a;
	arguments[1].v = b;
	return perform(Index::_add_vvv, arguments, r);
};

bool MathG::subtract_vvv(VectorG *a, VectorG *b, VectorG *r) noexcept
{
	if (!_ok) return false;
	if (Index::_subtract_vvv == 0xFFFFFFFE)
	{
		const char *names[2] = { "a", "b" };
		ArgumentType types[2] = { ArgumentType::vector, ArgumentType::vector };
		Operation op;
		op.argument_number = 2;
		op.argument_names = names;
		op.argument_types = types;
		op.result_type = ArgumentType::vector;
		op.check = [](Argument* arguments, ObjectG *result) -> bool
		{
			return(arguments[0].v->height() == arguments[1].v->height()
				&& arguments[0].v->height() == ((VectorG*)result)->height());
		};
		op.source =
		R"(
			#version 330 core
			uniform sampler2D a;
			uniform sampler2D b;
			out float c;
			void main()
			{
				int position = int(gl_FragCoord.y - 0.5);
				c = texelFetch(a, ivec2(0, position), 0).r - texelFetch(b, ivec2(0, position), 0).r;
			};
		)";
		Index::_subtract_vvv = submit(op);
	}
	if (Index::_subtract_vvv == 0xFFFFFFFF) return false;
	Argument arguments[2];
	arguments[0].v = a;
	arguments[1].v = b;
	return perform(Index::_subtract_vvv, arguments, r);
};

bool MathG::multiply_mvv(MatrixG *a, VectorG *b, VectorG *r) noexcept
{
	if (!_ok) return false;
	if (Index::_multiply_mvv == 0xFFFFFFFE)
	{
		const char *names[3] = { "a", "b", "awidth" };
		ArgumentType types[3] = { ArgumentType::vector, ArgumentType::vector, ArgumentType::int_ };
		Operation op;
		op.argument_number = 3;
		op.argument_names = names;
		op.argument_types = types;
		op.result_type = ArgumentType::vector;
		op.check = [](Argument* arguments, ObjectG *result) -> bool
		{
			return(arguments[0].v->height() == arguments[1].v->height()
				&& arguments[0].v->height() == ((VectorG*)result)->height());
		};
		op.source =
		R"(
			#version 330 core

			uniform sampler2D a;
			uniform sampler2D b;
			uniform int awidth;

			out float c;

			void main()
			{
				int position = int(gl_FragCoord.y - 0.5);
				float sum = 0.0;
				for (int i = 0; i < awidth; i++)
				{
					sum += texelFetch(a, ivec2(i, position), 0).r * texelFetch(b, ivec2(0, position), 0).r;
				}
				c = sum;
			};
		)";
		Index::_multiply_mvv = submit(op);
	}
	if (Index::_multiply_mvv == 0xFFFFFFFF) return false;
	Argument arguments[2];
	arguments[0].m = a;
	arguments[1].v = b;
	return perform(Index::_multiply_mvv, arguments, r);
};

void MathG::free() noexcept
{
	if (Object::_vbo1d != GL_ERR) glDeleteBuffers(1, &Object::_vbo1d);
	if (Object::_vao1d != GL_ERR) glDeleteVertexArrays(1, &Object::_vao1d);
	if (Object::_vbo2d != GL_ERR) glDeleteBuffers(1, &Object::_vbo2d);
	if (Object::_vao2d != GL_ERR) glDeleteVertexArrays(1, &Object::_vao2d);
	if (Shader::_distribute1d != GL_ERR) glDeleteShader(Shader::_distribute1d);
	if (Shader::_distribute2d != GL_ERR) glDeleteShader(Shader::_distribute2d);
	for (unsigned int i = 0; i < _noperations; i++)
	{
		::free(_operations[i].argument_names);
		::free(_operations[i].argument_types);
		::free(_operations[i].argument_locations);
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