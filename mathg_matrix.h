/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_MATRIX
#define MATHG_MATRIX

MatrixG::MatrixG(unsigned int width, unsigned int height, bool *ok)
{
	_matrix = true;
	glGenTextures(1, &_texture);
	if (_texture != MG_ERROR)
	{
		unsigned int position;
		ObjectG *object = this;
		MathG::_bind_object(1, &object, &position);
		glActiveTexture(GL_TEXTURE0 + position);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
		_width = width;
		_height = height;
		_ok = true;
	}
	if (ok != nullptr) *ok = _ok;
};

unsigned int MatrixG::width() const
{
	if (!_ok) return 0;
	return _width;
};

unsigned int MatrixG::height() const
{
	if (!_ok) return 0;
	return _height;
};

bool MatrixG::load(float *data)
{
	if (!_ok) return false;
	unsigned int position;
	ObjectG *object = this;
	MathG::_bind_object(1, &object, &position);
	glActiveTexture(GL_TEXTURE0 + position);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, data);
	return true;
};

bool MatrixG::store(const float *data)
{
	if (!_ok) return false;
	unsigned int position;
	ObjectG *object = this;
	MathG::_bind_object(1, &object, &position);
	glActiveTexture(GL_TEXTURE0 + position);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED, GL_FLOAT, data);
	return true;
};

bool MatrixG::store_line(unsigned int line, const float *data)
{
	if (!_ok) return false;
	unsigned int position;
	ObjectG *object = this;
	MathG::_bind_object(1, &object, &position);
	glActiveTexture(GL_TEXTURE0 + position);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, line, _width, 1, GL_RED, GL_FLOAT, data);
	return true;
};

bool MatrixG::store_column(unsigned int column, const float *data)
{
	if (!_ok) return false;
	unsigned int position;
	ObjectG *object = this;
	MathG::_bind_object(1, &object, &position);
	glActiveTexture(GL_TEXTURE0 + position);
	glTexSubImage2D(GL_TEXTURE_2D, 0, column, 0, 1, _height, GL_RED, GL_FLOAT, data);
	return true;
};

MatrixG::~MatrixG()
{
	if (_texture != MG_ERROR)
	{
		MathG::_unbind_object(this);
		glDeleteTextures(1, &_texture);
	}
	if (_framebuffer != MG_ERROR) glDeleteFramebuffers(1, &_framebuffer);
	_ok = false;
};

#endif	//#ifndef MATHG_MATRIX