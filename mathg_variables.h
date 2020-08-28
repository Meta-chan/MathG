/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_VARIABLES
#define MATHG_VARIABLES

GLuint MathG::Object::_vao1d = MG_UNINITED;
GLuint MathG::Object::_vbo1d = MG_UNINITED;
GLuint MathG::Object::_vao2d = MG_UNINITED;
GLuint MathG::Object::_vbo2d = MG_UNINITED;

GLuint MathG::Shader::_distribute1d = MG_UNINITED;
GLuint MathG::Shader::_distribute2d = MG_UNINITED;

unsigned int MathG::Index::_add_vvv = MG_UNINITED_INDEX;
unsigned int MathG::Index::_subtract_vvv = MG_UNINITED_INDEX;
unsigned int MathG::Index::_multiply_mvv = MG_UNINITED_INDEX;

#if defined(IR_MATHG_FREEGLUT)
	int MathG::_window = 0;
#elif defined(IR_MATHG_SDL2)
	SDL_Window *MathG::_window = nullptr;
	SDL_GLContext MathG::_context = nullptr;
#else
	EGLDisplay MathG::_display = EGL_NO_DISPLAY;
#endif

bool MathG::_ok = false;
bool MathG::_print = false;
bool *MathG::_permanent = nullptr;
GLuint *MathG::_temporary = nullptr;
unsigned int MathG::_npermanent = 0;
unsigned int MathG::_ntemporary = 0;
MathG::SubmittedOperation *MathG::_operations = nullptr;
unsigned int MathG::_noperations = 0;

#endif	//#ifndef MATHG_VARIABLES
