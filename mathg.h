/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG
#define MATHG

#define FREEGLUT_STATIC
#include <GL/freeglut.h>

#define GL_ERR ((GLuint)-1)

class MathG;

#ifndef DOXYGEN
	class ObjectG
	{
		friend MathG;
		
	protected:
		unsigned int _permanent = 0xFFFFFFFF;
		GLuint _texture			= GL_ERR;
		GLuint _framebuffer		= GL_ERR;
		bool _ok				= false;
	};
#endif

///Vector, one-dimensional float array
class VectorG : public ObjectG
{
	friend MathG;

private:
	unsigned int _height	= 0;
	VectorG(VectorG &vector);
	
public:
	///Creates vector and allocates graphical memory for it
	///@param height Dimension (height) of the vector
	///@param ok If not @c nullptr, receives @c true or @c false dependent on success or fail
	VectorG(unsigned int height, bool *ok);
	///Returns dimension (height) of vector
	unsigned int height() const;
	///Loads vector from graphical memory to processor memory
	///@return @c true or @c false dependent on success or fail
	bool load(float *data);
	///Stores vector from processor memory to graphical memory
	///@return @c true or @c false dependent on success or fail
	bool store(const float *data);
	///Destroys vector
	~VectorG();
};

///Matrix, two-dimensional float array
class MatrixG : public ObjectG
{
	friend MathG;

private:
	unsigned int _width		= 0;
	unsigned int _height	= 0;
	MatrixG(MatrixG &matrix);

public:
	///Creates matrix and allocates graphical memory for it
	///@param width Width of the matrix
	///@param height Height of the matrix
	///@param ok If not @c nullptr, receives @c true or @c false dependent on success or fail
	MatrixG(unsigned int width, unsigned int height, bool *ok);
	///Returns width of matrix
	unsigned int width() const;
	///Returns height of matrix
	unsigned int height() const;
	///Loads matrix from graphical memory to processor memory
	///@param data Pointer to buffer to receive values in line-major order. The buffer needs to be at least <tt>width() * height() * sizeof(float)</tt> size
	///@return @c true or @c false dependent on success or fail
	bool load(float *data);
	///Stores matrix from processor memory to graphical memory
	///@param data Pointer to buffer to take values from in line-major order. The buffer needs to be at least <tt>width() * height() * sizeof(float)</tt> size
	///@return @c true or @c false dependent on success or fail
	bool store(const float *data);
	///Stores one line of matrix from processor memory to graphical memory
	///@param line Line to store
	///@param data Pointer to buffer to take line values from. The buffer needs to be at least <tt>width() * sizeof(float)</tt> size
	///@return @c true or @c false dependent on success or fail
	bool store_line(unsigned int line, const float *data);
	///Stores one column of matrix from processor memory to graphical memory
	///@param column Column to store
	///@param data Pointer to buffer to take column values from. The buffer needs to be at least <tt>height() * sizeof(float)</tt> size
	///@return @c true or @c false dependent on success or fail
	bool store_column(unsigned int column, const float *data);
	///Destroys matrix
	~MatrixG();
};

///@brief MathG system, manipulates objects and performs mathematical operations on them
///@remark All vectors and matrices passed, both arguments and results, need to be @b different! Otherwise functions will fail.
class MathG
{
private:
	static bool _ok;
	static int _window;
	static bool *_permanent;
	static GLuint *_temporary;
	static unsigned int _npermanent;
	static unsigned int _ntemporary;

	struct Object
	{
		static GLuint _vao1d;
		static GLuint _vbo1d;
		static GLuint _vao2d;
		static GLuint _vbo2d;
	};

	struct Shader
	{
		static GLuint _distribute1d;
		static GLuint _distribute2d;
	};

	struct Program
	{
		struct AddVVV { static GLuint _program; static GLint _a; static GLint _b; };
		struct SubtractVVV { static GLuint _program; static GLint _a; static GLint _b; };
		struct MultiplyMVV { static GLuint _program; static GLint _a; static GLint _b; static GLint _awidth; };
	};

	struct Source
	{
		static const char *_distribute1d;
		static const char *_distribute2d;
		static const char *_add_vvv;
		static const char *_subtract_vvv;
		static const char *_multiply_mvv;
	};

	static bool _get_status(const char *name, bool link);
	static bool _compile_distribute1d();
	static bool _create_distribution1d();
	static bool _compile_distribute2d();
	static bool _create_distribution2d();

	static bool _compile(GLuint *program, const char *source, const char *name, bool d2);
	static bool _compile_add_vvv();
	static bool _compile_subtract_vvv();
	static bool _compile_multiply_mvv();

	static void _bind_object(unsigned int count, ObjectG **matrixes, unsigned int *positions); friend VectorG; friend MatrixG;
	static void _unbind_object(ObjectG *matrix); friend VectorG; friend MatrixG;

public:
	///Initializes MathG system
	///@param print Print compilation and linking errors to @c stdout
	static bool init(bool print);

	///Performs add operation on vectors <tt>r = a + b</tt>
	///@return @c true or @c false dependent on success or fail
	static bool add_vvv(VectorG *a, VectorG *b, VectorG *r);
	///Performs subtract operation <tt>r = a - b</tt>
	///@return @c true or @c false dependent on success or fail
	static bool subtract_vvv(VectorG *a, VectorG *b, VectorG *r);
	///Performs multiply operation <tt>r = A * b</tt>
	///@return @c true or @c false dependent on success or fail
	static bool multiply_mvv(MatrixG *a, VectorG *b, VectorG *r);
	
	///Finalizes MathG system
	///@return @c true or @c false dependent on success or fail
	static void free();
};

/**@mainpage Welcome to MathG!
*Ultra-light GPU-accelerated mathematics library. Developed mostly for [NeuroG](https://github.com/Meta-chan/NeuroG).
*
*### Dependencies
*At the moment MathG depends on [FreeGlut](http://freeglut.sourceforge.net) and [Ironic Library](https://github.com/Meta-chan/ironic_library). FreeGlut can be easily replaced with any equivalent library.
*
*### Platforms
*Currently Windows x86 only, but in perspective can be ported on every platform that provides OpenGL API. In nearest future I plan to port it on Linux x86.
*
*### Installation
*The installation process is integrated with [Ironic Library](https://github.com/Meta-chan/ironic_library), follow the link for more. But in a nutshell, you can do just this:
*@code{.cpp}
*#define IR_IMPLEMENT
*#include <mathg.h>
*@endcode
*
*### Documentation
*The code is pretty self-documented. But more importantly, I provide [Doxygen](https://www.doxygen.nl/manual/starting.html) documentation! It does not look too pretty since I am not an expert, but it is still quite informative. And of course, feel free to contact me!
*
*###### P.S. My code is not dirty, it is alternatively clean.
**/

#if (defined(IR_IMPLEMENT) || defined(IR_MATHG_IMPLEMENT)) && !defined(IR_MATHG_NOT_IMPLEMENT)
	#include "mathg_implementation.h"
#endif

#endif	//#ifndef MATHG