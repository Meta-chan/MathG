/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_SOURCE
#define MATHG_SOURCE

const char *MathG::Source::_distribute1d =
R"(#version 330 core
layout(location = 0) in float position;

void main()
{
	gl_Position = vec4(0.5f, position, 0.0f, 1.0f);
};)";

const char *MathG::Source::_distribute2d =
R"(#version 330 core
layout(location = 0) in float vertical;
layout(location = 1) in float horizontal;

void main()
{
	gl_Position = vec4(horizontal, vertical, 0.0f, 1.0f);
};)";

const char *MathG::Source::_add_vvv = 
R"(#version 330 core

uniform sampler2D a;
uniform sampler2D b;

out float c;

void main()
{
	int position = int(gl_FragCoord.y - 0.5);
	c = texelFetch(a, ivec2(0, position), 0).r + texelFetch(b, ivec2(0, position), 0).r;
};)";

const char *MathG::Source::_subtract_vvv =
R"(#version 330 core

uniform sampler2D a;
uniform sampler2D b;

out float c;

void main()
{
	int position = int(gl_FragCoord.y - 0.5);
	c = texelFetch(a, ivec2(0, position), 0).r - texelFetch(b, ivec2(0, position), 0).r;
};)";

const char *MathG::Source::_multiply_mvv =
R"(#version 330 core

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
};)";


#endif	//#ifndef MATHG_SOURCE