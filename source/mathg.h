/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

const GLuint mathg::MathG::_error	= (GLuint)-1;
bool mathg::MathG::_ok				= false;
bool mathg::MathG::_initialized		= false;
GLuint mathg::MathG::_vao2d			= mathg::MathG::_error;
GLuint mathg::MathG::_vbo2d			= mathg::MathG::_error;
GLuint mathg::MathG::_distribute2d	= mathg::MathG::_error;
GLint mathg::MathG::_max_binded		= 0;
GLint mathg::MathG::_max_accessed	= 0;

bool mathg::MathG::_gl_ok() noexcept
{
	bool ok = true;
	while (true)
	{
		GLint i = glGetError();
		if (i == GL_NO_ERROR) break;
		else ok = false;
	}
	return ok;
}
bool mathg::MathG::init() noexcept
{
	if (_initialized) return _ok;
	_initialized = true;
	
	//Disabling multisampling
	glDisable(GL_MULTISAMPLE);

	//Initializing GLEW
	if (glewInit() != GLEW_OK) return false;

	//Initializing table
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_max_accessed);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_max_binded);

	//Creating 2D distribution shader
	_distribute2d = glCreateShader(GL_VERTEX_SHADER);
	if (_distribute2d == _error) return false;
	const char *source = R"(
		#version 330 core
		layout(location = 0) in float vertical;
		layout(location = 1) in float horizontal;
		void main()
		{
			gl_Position = vec4(horizontal, vertical, 0.0f, 1.0f);
		};
	)";
	glShaderSource(_distribute2d, 1, &source, nullptr);
	glCompileShader(_distribute2d);
	GLint success;
	glGetShaderiv(_distribute2d, GL_COMPILE_STATUS, &success);
	if (!success) return false;

	//Creating and binding 2D distribution objects
	GLfloat d2[] = { 1.0f,	1.0f,
					-1.0f,	1.0f,
					-1.0f,	-1.0f,

					-1.0f,	-1.0f,
					1.0f,	-1.0f,
					1.0f,	1.0f,
	};
	glGenVertexArrays(1, &_vao2d);
	if (_vao2d == _error) return false;
	glBindVertexArray(_vao2d);
	glGenBuffers(1, &_vbo2d);
	if (_vbo2d == _error) return false;
	glBindBuffer(GL_ARRAY_BUFFER, _vbo2d);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), d2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)sizeof(GLfloat));
	glEnableVertexAttribArray(1);

	if (!_gl_ok()) return false;
	_ok = true;
	return true;
}

bool mathg::MathG::ok() noexcept
{
	return _ok;
}

void mathg::MathG::finalize() noexcept
{
	if (_vbo2d != _error) glDeleteBuffers(1, &_vbo2d);
	if (_vbo2d != _error) glDeleteVertexArrays(1, &_vao2d);
	if (_distribute2d != _error) glDeleteShader(_distribute2d);
	_ok = false;
	_initialized = false;
}