/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <assert.h>
#include <stdarg.h>

mathg::Matrix::Matrix(GLsizei height, GLsizei width, Type typ) noexcept
{
	if (height != 0 && width != 0)
	{
		glGenTextures(1, &_texture);
		if (!System::_gl_ok()) { if (_texture != System::_error) glDeleteTextures(1, &_texture); _texture = System::_error; }

		GLint position;
		Matrix *matrix = this;
		if (!System::_bind_matrixes(1, &matrix, &position))
			{ glDeleteTextures(1, &_texture); _texture = System::_error; return; }
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

		if (!System::_gl_ok()) { glDeleteTextures(1, &_texture); _texture = System::_error; }
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
	return _height == 0 || _width == 0 || _texture != System::_error;
}

bool mathg::Matrix::load(void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!System::_bind_matrixes(1, &matrix, &position)) return false;
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
	
	return System::_gl_ok();
}

bool mathg::Matrix::store(const void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!System::_bind_matrixes(1, &matrix, &position)) return false;
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

	return System::_gl_ok();
}

bool mathg::Matrix::store_row(GLint row, void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!System::_bind_matrixes(1, &matrix, &position)) return false;
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
	return System::_gl_ok();
}

bool mathg::Matrix::store_column(GLint column, void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!System::_bind_matrixes(1, &matrix, &position)) return false;
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
	return System::_gl_ok();
}

bool mathg::Matrix::store_element(GLint row, GLint column, void *data) noexcept
{
	assert(ok());
	GLint position;
	Matrix *matrix = this;
	if (!System::_bind_matrixes(1, &matrix, &position)) return false;
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
	return System::_gl_ok();
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
				va_arg(arguments, float);
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
	if (!System::_bind_matrixes(matrixes.size(), matrixes.data(), positions.data())) return false;

	//Setting uniforms
	va_start(arguments, function);
	for (size_t i = 0, nmatrixes = 0; i < function->_uniforms.size(); i++)
	{
		if (function->_uniforms[i].matrix)
		{
			glUniform1i(function->_uniforms[i].location, positions[nmatrixes]);
			if (function->_uniforms[i].height_location != System::_error)
				glUniform1i(function->_uniforms[i].height_location, matrixes[nmatrixes]->_height);
			if (function->_uniforms[i].width_location != System::_error)
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
			glUniform1f(function->_uniforms[i].location, va_arg(arguments, float));
		}
	}
	va_end(arguments);

	//Creating framebuffer
	if (_framebuffer == System::_error)
	{
		glGenFramebuffers(1, &_framebuffer);
		if (_framebuffer == System::_error) return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

	//Setting view and executing
	glViewport(0, 0, _width, _height);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glFlush();

	return System::_gl_ok();
}

mathg::Matrix::~Matrix() noexcept
{
	if (_texture != System::_error)
	{
		System::_unbind_matrix(this);
		glDeleteTextures(1, &_texture);
	}

	if (_framebuffer != System::_error)
	{
		glDeleteFramebuffers(1, &_framebuffer);
	}
}