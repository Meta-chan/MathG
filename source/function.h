/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_FUNCTION_SOURCE
#define MATHG_FUNCTION_SOURCE

#include <stdexcept>
#include <string>
#include <string.h>

bool mathg::Function::_idcmp(const char *str1, const char *str2) noexcept
{
	while (true)
	{
		if (!(str1[0] == '_'
			|| (str1[0] >= '0' && str1[0] <= '9')
			|| (str1[0] >= 'A' && str1[0] <= 'Z')
			|| (str1[0] >= 'a' && str1[0] <= 'z'))
		&& !(str2[0] == '_'
			|| (str2[0] >= '0' && str2[0] <= '9')
			|| (str2[0] >= 'A' && str2[0] <= 'Z')
			|| (str2[0] >= 'a' && str2[0] <= 'z')))
		{
			return true;
		}
		else if (str1[0] != str2[0])
		{
			return false;
		}
		else
		{
			str1++;
			str2++;
		}
	}
}

size_t mathg::Function::_idlen(const char *str1) noexcept
{
	size_t l = 0;
	while (str1[l] == '_'
		|| (str1[l] >= '0' && str1[l] <= '9')
		|| (str1[l] >= 'A' && str1[l] <= 'Z')
		|| (str1[l] >= 'a' && str1[l] <= 'z')) l++;
	return l;
}

size_t mathg::Function::_spacelen(const char *str1) noexcept
{
	size_t l = 0;
	while (str1[l] == ' ' || str1[l] == '\t' || str1[l] == '\n' || str1[l] == '\r') l++;
	return l;
}

mathg::Function::Uniform *mathg::Function::_idmatrix(const char *str1) noexcept
{
	for (size_t j = 0; j < _uniforms.size(); j++)
	{
		if (_uniforms[j].matrix && _idcmp(str1, _uniforms[j].name.c_str())) return &_uniforms[j];
	}
	return nullptr;
}

void mathg::Function::_row(Preprocessor *p)
{
	if (!p->local_row_used)
	{
		p->local_row_used = true;
		p->source.insert(p->local_start, "int row = int(gl_FragCoord.y - 0.5);");
		p->i += strlen("int row = int(gl_FragCoord.y - 0.5);");
	}
	p->i += strlen("row");
}

void mathg::Function::_column(Preprocessor *p)
{
	if (!p->local_column_used)
	{
		p->local_column_used = true;
		p->source.insert(p->local_start, "int column = int(gl_FragCoord.x - 0.5);");
		p->i += strlen("int column = int(gl_FragCoord.x - 0.5);");
	}
	p->i += strlen("column");
}

void mathg::Function::_matrix_name_dot(Preprocessor *p, Uniform *matrix, size_t replace_start)
{
	//Creating width and height
	p->i++;
	p->i += _spacelen(&p->source[p->i]);
	if (_idcmp(&p->source[p->i], "width"))
	{
		if (matrix->width_location != 0)
		{
			matrix->width_location = 0;
			std::string insertion = "uniform int " + matrix->name + "_width;";
			p->source.insert(0, insertion);
			p->i += insertion.size();
			p->local_start += insertion.size(); 
			replace_start += insertion.size();
		}
		p->i += _idlen(&p->source[p->i]);
		p->source.replace(replace_start, p->i - replace_start, matrix->name + "_width");
		p->i = replace_start + matrix->name.size() + strlen("_width");
	}
	else if (_idcmp(&p->source[p->i], "height"))
	{
		if (matrix->height_location != 0)
		{
			matrix->height_location = 0;
			std::string insertion = "uniform int " + matrix->name + "_height;";
			p->source.insert(0, insertion);
			p->i += insertion.size();
			p->local_start += insertion.size();
			replace_start += insertion.size();
		}
		p->i += _idlen(&p->source[p->i]);
		p->source.replace(replace_start, p->i - replace_start, matrix->name + "_height");
		p->i = replace_start + matrix->name.size() + strlen("_height");
	}
	else std::runtime_error("Invalid identifier after dor after matrix");
}

void mathg::Function::_matrix_name_bracket(Preprocessor *p, Uniform *matrix, size_t replace_start)
{
	//Extract row expresion
	p->i++;
	size_t row_expression_start = p->i;
	size_t index_level = 1;
	while (index_level != 0)
	{
		if (p->i == p->source.size()) throw std::runtime_error("Missing bracket after opened bracket after matrix");
		else if (p->source[p->i] == '[') index_level++;
		else if (p->source[p->i] == ']') index_level--;
		p->i++;
	}
	std::string row_expression(&p->source[row_expression_start], p->i - row_expression_start - 1);

	//Extract column expression
	p->i += _spacelen(&p->source[p->i]);
	if (p->source[p->i] != '[') throw std::runtime_error("Missing second bracket expression after matrix");
	p->i++;
	size_t column_expression_start = p->i;
	index_level = 1;
	while (index_level != 0)
	{
		if (p->i == p->source.size()) throw std::runtime_error("Missing bracket after opened bracket after matrix");
		else if (p->source[p->i] == '[') index_level++;
		else if (p->source[p->i] == ']') index_level--;
		p->i++;
	}
	std::string column_expression(&p->source[column_expression_start], p->i - column_expression_start - 1);

	//Replace
	std::string replacement = "texelFetch(" + matrix->name + ", ivec2(" + column_expression + ", " + row_expression + "), 0)";
	if (matrix->typ == Type::double_) replacement = "packDouble2x32(" + replacement + ".rg)";
	else replacement = replacement + ".r";
	p->source.replace(replace_start, p->i - replace_start, replacement);
	if (matrix->typ == Type::double_) p->i = replace_start + strlen("packDouble2x32(texelFetch(") + matrix->name.size();
	else p->i = replace_start + strlen("texelFetch(") + matrix->name.size();
}

void mathg::Function::_matrix_name(Preprocessor *p, Uniform *matrix)
{
	size_t replace_start = p->i;
	p->i += _idlen(&p->source[p->i]);
	p->i += _spacelen(&p->source[p->i]);
	if (p->source[p->i] == '[') _matrix_name_bracket(p, matrix, replace_start);
	else if (p->source[p->i] == '.') _matrix_name_dot(p, matrix, replace_start);
	else throw std::runtime_error("Invalid matrix usage");
}

void mathg::Function::_output_name(Preprocessor *p)
{
	p->i += _idlen(&p->source[p->i]);
	p->i += _spacelen(&p->source[p->i]);
	if (p->source[p->i] != '=') throw std::runtime_error("Invalid operation with output variable");
	p->i++;
	if (_output.typ == Type::double_) p->source.insert(p->i, "unpackDouble2x32(");
	else p->source.insert(p->i, "(");

	size_t start = p->i;
	while (true)
	{
		if (p->i == p->source.size()) throw std::runtime_error("Invalid operation with output variable");
		else if (p->source[p->i] == ';')
		{
			p->source.insert(p->i, ")");
			p->i = start;
			return;
		}
		else p->i++;
	}
}

void mathg::Function::_matrix(Preprocessor *p)
{
	//Identify type
	size_t replace_start = p->i;
	const char *replace = nullptr;
	p->i += _idlen(&p->source[p->i]);
	p->i += _spacelen(&p->source[p->i]);
	Uniform uniform;
	uniform.matrix = true;
	if (_idcmp(&p->source[p->i], "int"))
	{
		uniform.typ = Type::int_;
		replace = "uniform isampler2D";
	}
	else if (_idcmp(&p->source[p->i], "uint"))
	{
		uniform.typ = Type::uint_;
		replace = "uniform usampler2D";
	}
	else if (_idcmp(&p->source[p->i], "float"))
	{
		uniform.typ = Type::float_;
		replace = "uniform sampler2D";
	}
	else if (_idcmp(&p->source[p->i], "double"))
	{
		uniform.typ = Type::double_;
		replace = "uniform usampler2D";
		p->uses_double = true;
	}
	else throw std::runtime_error("Invalid matrix type");
	p->i += _idlen(&p->source[p->i]);

	//Replace
	p->source.replace(replace_start, p->i - replace_start, replace);
	p->i = replace_start + strlen(replace);

	//Register matrixes
	while (true)
	{
		p->i += _spacelen(&p->source[p->i]);
		if (_idlen(&p->source[p->i]) == 0) throw std::runtime_error("Invalid matrix declaration");
		uniform.name = std::string(&p->source[p->i], _idlen(&p->source[p->i]));
		p->i += _idlen(&p->source[p->i]);
		_uniforms.push_back(uniform);
		if (p->source[p->i] == ',') p->i++;
		else if (p->source[p->i] == ';') return;
		else throw std::runtime_error("Invalid matrix declaration");
	}
}

void mathg::Function::_uniform(Preprocessor *p)
{
	//Identify type
	p->i += _idlen(&p->source[p->i]);
	p->i += _spacelen(&p->source[p->i]);
	Uniform uniform;
	if (_idcmp(&p->source[p->i], "int")) uniform.typ = Type::int_;
	else if (_idcmp(&p->source[p->i], "uint")) uniform.typ = Type::uint_;
	else if (_idcmp(&p->source[p->i], "float")) uniform.typ = Type::float_;
	else if (_idcmp(&p->source[p->i], "double"))
	{
		uniform.typ = Type::double_;
		p->source.replace(p->i, strlen("double"), "ivec2");
		p->uses_double = true;
	}
	else throw std::runtime_error("Invalid uniform type");
	p->i += _idlen(&p->source[p->i]);

	//Register uniforms
	while (true)
	{
		p->i += _spacelen(&p->source[p->i]);
		if (_idlen(&p->source[p->i]) == 0) throw std::runtime_error("Invalid uniform declaration");
		uniform.name = std::string(&p->source[p->i], _idlen(&p->source[p->i]));
		p->i += _idlen(&p->source[p->i]);
		_uniforms.push_back(uniform);
		if (p->source[p->i] == ',') p->i++;
		else if (p->source[p->i] == ';') return;
		else throw std::runtime_error("Invalid uniform declaration");
	}
}

void mathg::Function::_out(Preprocessor *p)
{	
	//Identify type
	if (!_output.name.empty()) throw std::runtime_error("Output already declared");
	p->i += _idlen(&p->source[p->i]);
	p->i += _spacelen(&p->source[p->i]);
	if (_idcmp(&p->source[p->i], "int")) _output.typ = Type::int_;
	else if (_idcmp(&p->source[p->i], "uint")) _output.typ = Type::uint_;
	else if (_idcmp(&p->source[p->i], "float")) _output.typ = Type::float_;
	else if (_idcmp(&p->source[p->i], "double"))
	{
		_output.typ = Type::double_;
		p->source.replace(p->i, strlen("double"), "uvec2");
		p->uses_double = true;
	}
	p->i += _idlen(&p->source[p->i]);

	//Register output
	p->i += _spacelen(&p->source[p->i]);
	if (_idlen(&p->source[p->i]) == 0) throw std::runtime_error("Invalid output declaration");
	_output.name = std::string(&p->source[p->i], _idlen(&p->source[p->i]));
	p->i += _idlen(&p->source[p->i]);
}

mathg::Function::Function() noexcept
{
}

mathg::Function::Function(const char *source, char *error, GLsizei error_size) noexcept
{
	init(source, error, error_size);
}

bool mathg::Function::init(const char *source, char *error, GLsizei error_size) noexcept
{
	finalize();
	Preprocessor p;
	p.source = source;

	try
	{	
		while (p.source[p.i] != '\0')
		{
			if (p.local_level > 0)
			{
				if (_idcmp(&p.source[p.i], "row"))
					_row(&p);
				else if (_idcmp(&p.source[p.i], "column"))
					_column(&p);
				else if (p.source[p.i] == '{')
					{ p.local_level++; p.i++; }
				else if (p.source[p.i] == '}')
					{ p.local_level--; p.i++; }
				else if (_idcmp(&p.source[p.i], "double"))
					{ p.uses_double = true; p.i += _idlen(&p.source[p.i]); }
				else if (p.source[p.i - 1] != '.' && _idmatrix(&p.source[p.i]) != nullptr)
					{ _matrix_name(&p, _idmatrix(&p.source[p.i])); }
				else if (p.source[p.i - 1] != '.' && _idcmp(&p.source[p.i], _output.name.c_str()))
					{ _output_name(&p); }
				else if (_idlen(&p.source[p.i]) > 0)
					p.i += _idlen(&p.source[p.i]);
				else
					p.i++;
			}
			else if (_idcmp(&p.source[p.i], "matrix")) _matrix(&p);
			else if (_idcmp(&p.source[p.i], "uniform")) _uniform(&p);
			else if (_idcmp(&p.source[p.i], "out")) _out(&p);
			else if (p.source[p.i] == '{')
			{
				p.i++;
				p.local_level = 1;
				p.local_start = p.i;
				p.local_row_used = false;
				p.local_column_used = false;
			}
			else p.i++;
		}
		if (p.uses_double) p.source.insert(0, "#extension GL_ARB_gpu_shader_fp64 : enable\n");
		p.source.insert(0, "#version 330 core\n");
	}
	catch (std::exception &e)
	{
		if (error != nullptr)
		{
			#ifdef _WIN32
				strcpy_s(error, error_size, e.what());
			#else
				if (strlen(e.what()) < error_size) memcpy(error, e.what(), strlen(e.what()));
				else memcpy(error, e.what(), error_size);
				if (error_size != 0) error[error_size - 1] = '\0';
			#endif
		}
		return false;
	}

	//Compile fragment shader
	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (shader == MathG::_error) return false;
	const char *processed_cstr = p.source.c_str();
	glShaderSource(shader, 1, &processed_cstr, nullptr);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, error_size, nullptr, error);
		glDeleteShader(shader);
		return false;
	}

	//Linking program
	_program = glCreateProgram();
	if (_program == MathG::_error) return false;
	glAttachShader(_program, MathG::_distribute2d);
	glAttachShader(_program, shader);
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, error_size, nullptr, error);
		glDeleteShader(shader);
		glDeleteProgram(_program);
		_program = MathG::_error;
		return false;
	}

	//Finding uniforms
	for (size_t i = 0; i < _uniforms.size(); i++)
	{
		if (_uniforms[i].matrix)
		{
			if (_uniforms[i].width_location == 0)
				_uniforms[i].width_location = glGetUniformLocation(_program, std::string(_uniforms[i].name + "_width").c_str());
			if (_uniforms[i].height_location == 0)
				_uniforms[i].height_location = glGetUniformLocation(_program, std::string(_uniforms[i].name + "_height").c_str());
		}
		_uniforms[i].location = glGetUniformLocation(_program, _uniforms[i].name.c_str());
	}
	if (_output.width_location == 0)
		_output.width_location = glGetUniformLocation(_program, std::string(_output.name + "_width").c_str());
	if (_output.height_location == 0)
		_output.height_location = glGetUniformLocation(_program, std::string(_output.name + "_height").c_str());

	//Final check
	if (!MathG::_gl_ok()) { glDeleteProgram(_program); _program = MathG::_error; return false; }

	return true;
}

bool mathg::Function::ok() const noexcept
{
	return _program != MathG::_error;
}

void mathg::Function::finalize() noexcept
{
	if (_program != MathG::_error)
	{
		glDeleteProgram(_program);
		_program = MathG::_error;
	}
	_uniforms.clear();
	_output = Output();
}

mathg::Function::~Function() noexcept
{
	finalize();
}

#endif	//#ifndef MATHG_FUNCTION_SOURCE
