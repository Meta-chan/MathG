/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_MATRIX
#define MATHG_MATRIX

#include "type.h"
#include "mathg.h"
#include "function.h"
#include <vector>
#include <GL/glew.h>

namespace mathg
{	
	///Matrix, two-dimensional array of specified type
	class Matrix
	{
	private:
		struct TextureUnit
		{
			GLuint texture = MathG::_error;
			size_t age = (size_t)-1;
		};

		static std::vector<TextureUnit> _texture_units;
		GLsizei _height		= 0;
		GLsizei _width		= 0;
		Type _typ			= Type::int_;
		GLuint _texture		= MathG::_error;
		GLuint _framebuffer = MathG::_error;	//Still don't know if I need framebuffer for each texture

		static bool _bind(size_t n, Matrix **matrixes, GLint *positions)	noexcept;
		static void _unbind(Matrix *matrix)									noexcept;
		Matrix(const Matrix &matrix)										noexcept;

	public:
		///Creates empty matrix
		Matrix()												noexcept;
		///Creates matrix
		///@param width Width of the matrix
		///@param height Height of the matrix
		///@param typ Type of elements in matrix
		Matrix(GLsizei height, GLsizei width, Type typ)			noexcept;
		///Initializes existing matrix
		///@param width Width of the matrix
		///@param height Height of the matrix
		///@param typ Type of elements in matrix
		bool init(GLsizei height, GLsizei width, Type typ)		noexcept;
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
		///Finalizes matrix
		void finalize()											noexcept;
		///Destroys matrix
		~Matrix()												noexcept;
	};
}

#ifdef MATHG_IMPLEMENT
	#include "../../source/matrix.h"
#endif

#endif	//#ifndef MATHG