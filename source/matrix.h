/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_MATRIX_SOURCE
#define MATHG_MATRIX_SOURCE

#include "../include/mathg/function.h"
#include <vector>
#include <stdexcept>
#include <assert.h>
#include <stdarg.h>

std::vector<mathg::Matrix::TextureUnit> mathg::Matrix::_texture_units;

bool mathg::Matrix::_bind(size_t n, Matrix **matrixes, GLint *positions) noexcept
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
			if (positions[j] == -1 && _texture_units[i].texture == MathG::_error)
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
		if (positions[j] == -1 && _texture_units.size() < MathG::_max_binded)
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

void mathg::Matrix::_unbind(Matrix *matrix) noexcept
{
	for (GLenum i = 0; i < _texture_units.size(); i++)
	{
		if (matrix->_texture == _texture_units[i].texture)
		{
			_texture_units[i].texture = MathG::_error;
			_texture_units[i].age = (size_t)-1;
		}
	}
}

mathg::Matrix::Matrix(GLsizei height, GLsizei width, Type typ) noexcept
{
	if (height != 0 && width != 0)
	{
		glGenTextures(1, &_texture);
		if (!MathG::_gl_ok()) { if (_texture != MathG::_error) glDeleteTextures(1, &_texture); _texture = MathG::_error; }

		GLint position;
		Matrix *matrix = this;
		if (!_bind(1, &matrix, &position))
			{ glDeleteTextures(1, &_texture); _texture = MathG::_error; return; }
		glActiveTexture(GL_TEXTURE0 + position);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		switch (typ)
		{
		case Type::int_:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, nullptr);
			break;

		case Type::uint_:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
			break;

		case Type::double_:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, width, height, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, nullptr);
			break;

		default:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
		}

		if (!MathG::_gl_ok()) { glDeleteTextures(1, &_texture); _texture = MathG::_error; }
	}

	_width = width;
	_height = height;
	_typ = typ;
}

GLsizei mathg::Matrix::height() const noexcept
{
	assert(ok());
	return _height;
}

GLsizei mathg::Matrix::width() const noexcept
{
	assert(ok());
	return _width;
}

mathg::Type mathg::Matrix::typ() const noexcept
{
	assert(ok());
	return _typ;
}

bool mathg::Matrix::ok() const noexcept
{
	return _height == 0 || _width == 0 || _texture != MathG::_error;
}

bool mathg::Matrix::load(void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!_bind(1, &matrix, &position)) return false;
	glActiveTexture(GL_TEXTURE0 + position);
	switch (_typ)
	{
	case Type::int_:
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, data);
		break;

	case Type::uint_:
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, data);
		break;

	case Type::double_:
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, data);
		break;

	default:
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, data);
	}
	
	return MathG::_gl_ok();
}

bool mathg::Matrix::store(const void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!_bind(1, &matrix, &position)) return false;
	glActiveTexture(GL_TEXTURE0 + position);
	switch (_typ)
	{
	case Type::int_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED_INTEGER, GL_INT, data);
		break;

	case Type::uint_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED_INTEGER, GL_UNSIGNED_INT, data);
		break;

	case Type::double_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RG_INTEGER, GL_UNSIGNED_INT, data);
		break;

	default:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED, GL_FLOAT, data);
	}

	return MathG::_gl_ok();
}

bool mathg::Matrix::store_row(GLint row, void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!_bind(1, &matrix, &position)) return false;
	glActiveTexture(GL_TEXTURE0 + position);
	switch (_typ)
	{
	case Type::int_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, _width, 1, GL_RED_INTEGER, GL_INT, data);
		break;

	case Type::uint_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, _width, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, data);
		break;

	case Type::double_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, _width, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, data);
		break;

	default:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, _width, 1, GL_RED, GL_FLOAT, data);
	}
	return MathG::_gl_ok();
}

bool mathg::Matrix::store_column(GLint column, void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!_bind(1, &matrix, &position)) return false;
	glActiveTexture(GL_TEXTURE0 + position);
	switch (_typ)
	{
	case Type::int_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, column, 0, 1, _height, GL_RED_INTEGER, GL_INT, data);
		break;

	case Type::uint_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, column, 0, 1, _height, GL_RED_INTEGER, GL_UNSIGNED_INT, data);
		break;

	case Type::double_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, column, 0, 1, _height, GL_RG_INTEGER, GL_UNSIGNED_INT, data);
		break;

	default:
		glTexSubImage2D(GL_TEXTURE_2D, 0, column, 0, 1, _height, GL_RED, GL_FLOAT, data);
	}
	return MathG::_gl_ok();
}

bool mathg::Matrix::store_element(GLint row, GLint column, void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!_bind(1, &matrix, &position)) return false;
	glActiveTexture(GL_TEXTURE0 + position);
	switch (_typ)
	{
	case Type::int_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, column, row, 1, 1, GL_RED_INTEGER, GL_INT, data);
		break;

	case Type::uint_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, column, row, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, data);
		break;

	case Type::double_:
		glTexSubImage2D(GL_TEXTURE_2D, 0, column, row, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, data);
		break;

	default:
		glTexSubImage2D(GL_TEXTURE_2D, 0, column, row, 1, 1, GL_RED, GL_FLOAT, data);
	}
	return MathG::_gl_ok();
}

bool mathg::Matrix::assign(const Function *function, ...) noexcept
{
	assert(ok());
	assert(function->ok());

	//Finding matrixes
	va_list arguments;
	va_start(arguments, function);
	std::vector<Matrix*> matrixes;
	std::vector<GLint> positions;
	try
	{
		for (size_t i = 0; i < function->_uniforms.size(); i++)
		{
			if (function->_uniforms[i].matrix)
			{
				matrixes.push_back(va_arg(arguments, Matrix*));
				positions.push_back(0);
				assert(matrixes.back()->ok());
				assert(matrixes.back()->_typ == function->_uniforms[i].typ);
			}
			else switch (function->_uniforms[i].typ)
			{
			case Type::int_:
				va_arg(arguments, int);
				break;

			case Type::uint_:
				va_arg(arguments, unsigned int);
				break;
			
			case Type::double_:
				va_arg(arguments, double);
				break;

			default:
				va_arg(arguments, double);
			}
		}
	}
	catch (std::exception e)
	{
		va_end(arguments);
		return false;
	}
	va_end(arguments);

	//Using program
	glUseProgram(function->_program);
	//ACHTUNG 1: Vertex array is now always binded!
	//ACHTUNG 2: I sould consider downgrading to OpenGL 2.0. I need to test it on Raspberry PI.

	//Binding objects
	if (!_bind(matrixes.size(), matrixes.data(), positions.data())) return false;

	//Setting uniforms
	va_start(arguments, function);
	for (size_t i = 0, nmatrixes = 0; i < function->_uniforms.size(); i++)
	{
		if (function->_uniforms[i].matrix)
		{
			glUniform1i(function->_uniforms[i].location, positions[nmatrixes]);
			if (function->_uniforms[i].height_location != MathG::_error)
				glUniform1i(function->_uniforms[i].height_location, matrixes[nmatrixes]->_height);
			if (function->_uniforms[i].width_location != MathG::_error)
				glUniform1i(function->_uniforms[i].width_location, matrixes[nmatrixes]->_width);
			nmatrixes++;
		}
		else switch (function->_uniforms[i].typ)
		{
		case Type::int_:
			glUniform1i(function->_uniforms[i].location, va_arg(arguments, int));
			break;

		case Type::uint_:
			glUniform1ui(function->_uniforms[i].location, va_arg(arguments, unsigned int));
			break;

		case Type::double_:
			glUniform2ui(function->_uniforms[i].location, va_arg(arguments, unsigned int), va_arg(arguments, unsigned int));
			break;

		default:
			glUniform1f(function->_uniforms[i].location, (GLfloat)va_arg(arguments, double));
		}
	}
	va_end(arguments);

	//Creating framebuffer
	if (_framebuffer == MathG::_error)
	{
		glGenFramebuffers(1, &_framebuffer);
		if (_framebuffer == MathG::_error) return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

	//Setting view and executing
	glViewport(0, 0, _width, _height);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glFlush();

	return MathG::_gl_ok();
}

mathg::Matrix::~Matrix() noexcept
{
	if (_texture != MathG::_error)
	{
		_unbind(this);
		glDeleteTextures(1, &_texture);
	}

	if (_framebuffer != MathG::_error)
	{
		glDeleteFramebuffers(1, &_framebuffer);
	}
}

#endif	//#ifndef MATHG_MATRIX_SOURCE