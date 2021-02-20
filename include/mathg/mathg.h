/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG
#define MATHG

#include <GL/glew.h>
#if defined(MATHG_FREEGLUT)
	#include <GL/freeglut.h>
#elif defined(MATHG_SDL2)
	#ifdef _WIN32
		#include <SDL.h>
		#include <SDL_opengl.h>
	#else
		#include <SDL2/SDL.h>
		#include <SDL2/SDL_opengl.h>
	#endif
#else
	#include <EGL/egl.h>
#endif

#include <vector>
#include <string>

namespace mathg
{
	class Function;
	class Matrix;
	
	///MathG system, contains initialization and finalization functions
	class System
	{
	friend Function;
	friend Matrix;
	private:
		struct TextureUnit
		{
			GLuint texture	= _error;
			size_t age		= (size_t)-1;
		};

		static const GLuint _error;
		static GLuint _vao2d;
		static GLuint _vbo2d;
		static GLuint _distribute2d;
		static GLint _max_binded;
		static GLint _max_accessed;
		static std::vector<TextureUnit> _texture_units;
		static bool _ok;
		static bool _initialized;
		#if defined(MATHG_FREEGLUT)
			static int _window;
		#elif defined(MATHG_SDL2)
			static SDL_Window *_window;
			static SDL_GLContext _context;
		#else
			static EGLDisplay _display;
		#endif

		static bool _gl_ok() noexcept;
		static bool _bind_matrixes(size_t n, Matrix **matrixes, GLint *positions) noexcept;
		static void _unbind_matrix(Matrix *matrix) noexcept;

	public:
		///Initializes System
		static bool init()		noexcept;
		///Returns if System is ok
		static bool ok()		noexcept;
		///Finalizes System
		static void finalize()	noexcept;
	};

	///Type of elements of matrix
	enum class Type
	{
		int_,
		uint_,
		float_,
		double_
	};
	
	///Function to be performed on matrixes
	class Function
	{
	friend System;
	friend Matrix;
	private:
		struct Output
		{
			Type typ				= Type::int_;
			GLuint width_location	= System::_error;
			GLuint height_location	= System::_error;
			std::string name;
		};

		struct Uniform : Output
		{
			GLuint location			= System::_error;
			bool matrix				= false; 
		};

		struct Preprocessor
		{
			size_t i				= 0;
			std::string source;
			size_t local_level		= 0;
			size_t local_start		= 0;
			bool local_column_used	= false;
			bool local_row_used		= false;
			bool uses_double		= false;
		};

		std::vector<Uniform> _uniforms;
		Output _output;
		GLuint _program			= System::_error;
		
		static bool _idcmp(const char *str1, const char *str2) noexcept;
		static size_t _idlen(const char *str1) noexcept;
		static size_t _spacelen(const char *str1) noexcept;
		void _row(Preprocessor *p);
		void _column(Preprocessor *p);
		bool _matrix_name_dot(Preprocessor *p, Uniform *matrix, size_t replace_start);
		bool _matrix_name_bracket(Preprocessor *p, Uniform *matrix, size_t replace_start);
		Uniform *_idmatrix(const Preprocessor *p);
		bool _matrix_name(Preprocessor *p, Uniform *matrix);
		bool _output_name(Preprocessor *p);

		bool _matrix(Preprocessor *p);
		bool _uniform(Preprocessor *p);
		bool _out(Preprocessor *p);

		Function(const Function &function) noexcept;
		
	public:
		///Creates function from source (see examples)
		///@param source Source of function
		///@param error Buffer to print error
		Function(const char *source, char *error) noexcept;
		///Returns if function is ok
		bool ok() const noexcept;
		///Destroys function
		~Function() noexcept;
	};

	///Matrix, two-dimensional array of specified type
	class Matrix
	{
	friend System;
	friend Function;
	private:
		GLsizei _height		= 0;
		GLsizei _width		= 0;
		Type _typ			= Type::int_;
		GLuint _texture		= System::_error;
		GLuint _framebuffer = System::_error;	//Still don't know if I need framebuffer for each texture

		Matrix(const Matrix &matrix)							noexcept;

	public:
		///Creates matrix and allocates graphical memory for it
		///@param width Width of the matrix
		///@param height Height of the matrix
		///@param typ Type of elements in matrix
		Matrix(GLsizei height, GLsizei width, Type typ)		noexcept;
		///Returns width of matrix
		GLsizei height()										const noexcept;
		///Returns height of matrix
		GLsizei width()											const noexcept;
		///Returns elements' type
		Type typ()												const noexcept;
		///Returns if matrix is ok
		bool ok()												const noexcept;
		///Loads matrix from graphical memory to processor memory
		///@param data Pointer to buffer to receive values in row-major order
		///@return `true` n success, `false` on fail
		bool load(void *data)									noexcept;
		///Stores matrix from processor memory to graphical memory
		///@param data Pointer to buffer to take values from in row-major order
		///@return `true` n success, `false` on fail
		bool store(const void *data)							noexcept;
		///Stores one row of matrix from processor memory to graphical memory
		///@param row Row to store
		///@param data Pointer to buffer to take row values from
		///@return `true` n success, `false` on fail
		bool store_row(GLint row, void *data)					noexcept;
		///Stores one column of matrix from processor memory to graphical memory
		///@param column Column to store
		///@param data Pointer to buffer to take column values from
		///@return `true` n success, `false` on fail
		bool store_column(GLint column, void *data)				noexcept;
		///Stores one element of matrix from processor memory to graphical memory
		///@param row Row of element to store
		///@param column Column of element to store
		///@param data Pointer to buffer to take value from
		///@return `true` n success, `false` on fail
		bool store_element(GLint row, GLint column, void *data)	noexcept;
		///Assigns matrix to result of function
		///@param function Function to perform
		///@return `true` n success, `false` on fail
		bool assign(const Function *function, ...)				noexcept;
		///Destroys matrix
		~Matrix()												noexcept;
	};
}

/**@mainpage Welcome to MathG!
Just look at the following example:
```
//Creating matrices
mathg::Matrix ag(2, 2, mathg::Type::double_);
mathg::Matrix bg(2, 1, mathg::Type::double_);
mathg::Matrix cg(2, 1, mathg::Type::double_);

//Storing values into matrices
double a[4] = { 1.0, 2.0, 3.0, 4.0 };
double b[2] = { 1.0, 1.0 };
double c[2];
ag.store(a);
bg.store(b);

//Matrix multiplication function
mathg::Function multiply(R"(
		matrix double a;
		matrix double b;
		out double r;

		void main()
		{
			double sum = 0.0;
			for (int i = 0; i < a.width; i++) sum += a[row][i] * b[i][0];
			r = sum;
		}
	)", nullptr);

//Multiplication
cg.assign(&multiply, &ag, &bg);

//Result
cg.load(c);
```
And that's it! We performed matrix multiplication in GPU!

### Shaders
MathG preprocesses source codes into valid `GLSL` shaders. Here is a pair of moments you need to know about:
 - `matrix` macro is used to declare a matrix.
 - You can  use brackets with matrices.
 - You can use `.width` and `.height` expressions with matrices. Appropriate uniform variables are created automatically.
 - You can use `row` and `column` to get what cell we are calculating result to.
 - You can only write to `out` variable.
 
### Platforms
The code can be potentially run at every platform that supports `OpenGL 3.3`. Currently Windows and Linux are supported.

### Installation
After installing dependencies, you can do just this:
```
#define MATHG_IMPLEMENT
#include <mathg.h>
```

### Dependencies
MathG depends on:
 - [GLEW](http://glew.sourceforge.net)
 - Libraries to create OpenGL environment, which can be:
   - [SDL](https://www.libsdl.org) (`MATHG_SDL2` macro needs to be defined)
   - [FreeGlut](http://freeglut.sourceforge.net) (`MATHG_FREEGLUT` macro needs to be defined)
   - [EGL](https://www.khronos.org/egl/) (default option)

###### P.S. My code is not dirty, it is alternatively clean.
*/

#ifdef MATHG_IMPLEMENT
	#ifndef MATHG_SOURCE
	#define MATHG_SOURCE
		#include "../../source/mathg.h"
		#include "../../source/function.h"
		#include "../../source/matrix.h"
	#endif
#endif

#endif	//#ifndef MATHG