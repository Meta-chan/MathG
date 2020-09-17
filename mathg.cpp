/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#define IR_IMPLEMENT
//#define IR_MATHC_OPENMP
#define IR_MATHG_FREEGLUT

#include "mathg.h"
#include <ir_math/ir_mathc.h>
#include <random>
#include <time.h>

void test_performance()
{
	const unsigned int SIZE = 2000;
	const unsigned int N = 1000;
	const unsigned int ALIGN = 8;

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	bool ok;
	ir::MatrixC<float, ALIGN> ac(SIZE, SIZE, &ok); if (!ok) return;
	for (unsigned int i = 0; i < SIZE; i++)
	{
		for (unsigned int j = 0; j < SIZE; j++)	ac.data(i)[j] = distribution(generator);
	}
	MatrixG ag(SIZE, SIZE, &ok); if (!ok) return;
	for (unsigned int i = 0; i < SIZE; i++)
	{
		if (!ag.store_line(i, ac.data(i))) return;
	}

	ir::VectorC<float, ALIGN> bc(SIZE, &ok); if (!ok) return;
	for (unsigned int i = 0; i < SIZE; i++)	bc.data()[i] = distribution(generator);
	VectorG bg(SIZE, &ok); if (!ok) return;
	if (!bg.store(bc.data())) return;

	ir::VectorC<float, ALIGN> cc(SIZE, &ok); if (!ok) return;
	VectorG cg(SIZE, &ok); if (!ok) return;
	
	ir::MathC<float, ALIGN>::multiply_mvv(&ac, &bc, &cc);
	clock_t cl = clock();
	for (unsigned int i = 0; i < N; i++)
	{
		ir::MathC<float, ALIGN>::multiply_mvv(&ac, &bc, &cc);
	}
	printf("CPU done in %f seconds\n", (float)(clock() - cl)/CLOCKS_PER_SEC);

	MathG::multiply_mvv(&ag, &bg, &cg);
	cl = clock();
	for (unsigned int i = 0; i < N; i++)
	{
		MathG::multiply_mvv(&ag, &bg, &cg);
	}
	printf("GPU done in %f seconds\n", (float)(clock() - cl) / CLOCKS_PER_SEC);

	cg.load(cc.data());
	return;
};

void test_accurance()
{
	const unsigned int SIZE = 2;

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	
	float a[SIZE * SIZE], b[SIZE], c[SIZE];
	for (unsigned int i = 0; i < SIZE * SIZE; i++) a[i] = distribution(generator);
	for (unsigned int i = 0; i < SIZE; i++) b[i] = distribution(generator);

	//CPU
	for (unsigned int i = 0; i < SIZE; i++)
	{
		float sum = 0.0f;
		for (unsigned int j = 0; j < SIZE; j++)
		{
			sum += a[SIZE * i + j] * b[j];
		}
		c[i] = sum;
	}

	//GPU
	bool ok;
	MatrixG ag(SIZE, SIZE, &ok);
	if (!ag.ok() || !ag.store(a)) return;
	VectorG bg(SIZE, &ok);
	if (!bg.ok() || !bg.store(b)) return;
	VectorG cg(SIZE, &ok);
	if (!cg.ok()) return;
	if (!MathG::multiply_mvv(&ag, &bg, &cg)) return;
	if (!cg.load(b)) return;

	//Compare
	bool match = true;
	for (unsigned int i = 0; i < SIZE; i++)
	{
		if (c[i] != b[i])
		{
			match = false;
		}
	}
	printf(match ? "match\n" : "no match\n");

	return;
};

void test_1d()
{
	MathG::Operation op;
	op.name = "r";
	op.argument_number = 1;
	const char *names[1] = { "r" };
	op.argument_names = names;
	MathG::ArgumentType types[1] = { MathG::ArgumentType::float_ };
	op.argument_types = types;
	op.result_type = MathG::ArgumentType::vector; 
	op.check = nullptr;
	op.source =
		R"(
		#version 330 core
		uniform float r;
		out float o;

		void main()
		{
			o = r;
		};
	)";
	unsigned int i = MathG::submit(&op);

	MathG::Argument arg[1];
	arg[0].f = 0.42f;
	VectorG v(1, nullptr);
	MathG::perform(i, arg, &v);
	float f = 0;
	v.load(&f);
	GLenum e = glGetError();
};

void test_2d()
{
	MathG::Operation op;
	op.name = "r";
	op.argument_number = 1;
	const char *names[1] = { "r" };
	op.argument_names = names;
	MathG::ArgumentType types[1] = { MathG::ArgumentType::float_ };
	op.argument_types = types;
	op.check = nullptr;
	op.result_type = MathG::ArgumentType::matrix;
	op.source = 
	R"(
		#version 330 core
		uniform float r;
		out float o;

		void main()
		{
			o = r;
		};
	)";
	unsigned int i = MathG::submit(&op);

	MathG::Argument arg[1];
	arg[0].f = 0.42f;
	MatrixG m(1, 1, nullptr);
	MathG::perform(i, arg, &m);
	float f = 0;
	m.load(&f);
	f = 0.0f;
	GLenum e = glGetError();
};

int main(int argc, char *argv[])
{
	if (MathG::init(true))
	{
		test_accurance();
		MathG::free();
	}
	getchar();
	return 0;
};