/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//#define NDEBUG

#define MATHG_IMPLEMENT
#define MATHG_FREEGLUT
#include "include/mathg/mathg.h"

#include <random>
#include <time.h>

void test_simple_double()
{
	double a[4] = { 1.0, 2.0, 3.0, 4.0 };
	double b[2] = { 1.0, 1.0 };
	double c[2];
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

	mathg::Matrix ag(2, 2, mathg::Type::double_);
	mathg::Matrix bg(2, 1, mathg::Type::double_);
	mathg::Matrix cg(2, 1, mathg::Type::double_);
	if (!multiply.ok() || !ag.ok() || !bg.ok() || !cg.ok()) return;
	ag.store(a);
	bg.store(b);
	cg.assign(&multiply, &ag, &bg);
	cg.load(c);
	printf("Double test %s\n", (c[0] == 3.0 && c[1] == 7.0) ? "passed" : "failed");
}

void test_simple_float()
{
	float a[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
	float b[2] = { 1.0f, 1.0f };
	float c[2];
	char er[512];
	mathg::Function multiply(R"(
			matrix float a;
			matrix float b;
			out float r;

			void main()
			{
				float sum = 0.0f;
				for (int i = 0; i < a.width; i++) sum += a[row][i] * b[i][0];
				r = sum;
			}
		)", er);

	mathg::Matrix ag(2, 2, mathg::Type::float_);
	mathg::Matrix bg(2, 1, mathg::Type::float_);
	mathg::Matrix cg(2, 1, mathg::Type::float_);
	if (!multiply.ok() || !ag.ok() || !bg.ok() || !cg.ok()) return;
	ag.store(a);
	bg.store(b);
	cg.assign(&multiply, &ag, &bg);
	cg.load(c);
	printf("Float test %s\n", (c[0] == 3.0f && c[1] == 7.0f) ? "passed" : "failed");
}

void test_simple_int()
{
	int a[4] = { 1, 2, 3, 4 };
	int b[2] = { 1, 1 };
	int c[2];
	mathg::Function multiply(R"(
			matrix int a;
			matrix int b;
			out int r;

			void main()
			{
				int sum = 0;
				for (int i = 0; i < a.width; i++) sum += a[row][i] * b[i][0];
				r = sum;
			}
		)", nullptr);

	mathg::Matrix ag(2, 2, mathg::Type::int_);
	mathg::Matrix bg(2, 1, mathg::Type::int_);
	mathg::Matrix cg(2, 1, mathg::Type::int_);
	if (!multiply.ok() || !ag.ok() || !bg.ok() || !cg.ok()) return;
	ag.store(a);
	bg.store(b);
	cg.assign(&multiply, &ag, &bg);
	cg.load(c);
	printf("Int test %s\n", (c[0] == 3 && c[1] == 7) ? "passed" : "failed");
}

void test_cpu(const float * __restrict a, float * __restrict b, float * __restrict c, int size, int n)
{
	clock_t start = clock();

	for (int time = 0; time < n; time++)
	{
		#pragma omp parallel for
		for (int row = 0; row < size; row++)
		{
			float sum = 0.0f;
			for (int i = 0; i < size; i++) sum += a[row * size + i] * b[i];
			c[row] = sum;
		}

		#pragma omp parallel for
		for (int row = 0; row < size; row++)
		{
			float sum = 0.0f;
			for (int i = 0; i < size; i++) sum += a[row * size + i] * c[i];
			b[row] = sum;
		}
	}

	printf("CPU done in %f seconds\n", (float)(clock() - start) / CLOCKS_PER_SEC);
}

void test_gpu(const float * __restrict a, float * __restrict b, float * __restrict c, int size, int n)
{
	mathg::Function multiply(R"(
		matrix float a;
		matrix float b;
		out float r;

		void main()
		{
			float sum = 0.0;
			for (int i = 0; i < a.width; i++) sum += a[row][i] * b[i][0];
			r = sum;
		}
	)", nullptr);

	mathg::Matrix ag(size, size, mathg::Type::float_);
	mathg::Matrix bg(size, 1, mathg::Type::float_);
	mathg::Matrix cg(size, 1, mathg::Type::float_);
	if (!multiply.ok() || !ag.ok() || !bg.ok() || !cg.ok()) return;

	clock_t start = clock();
	for (int time = 0; time < n; time++)
	{
		if (!cg.assign(&multiply, &ag, &bg)) return;
		if (!bg.assign(&multiply, &ag, &cg)) return;
	}
	printf("GPU done in %f seconds\n", (float)(clock() - start) / CLOCKS_PER_SEC);
	cg.load(c);
}

void test_gpu_vectorized(const float * __restrict a, float * __restrict b, float * __restrict c, int size, int n)
{
	mathg::Function multiply(R"(
		matrix float a;
		matrix float b;
		out float r;

		void main()
		{
			vec4 sum = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			int i = 0;
			for (i = 0; i + 3 < a.width; i += 4)
				sum += vec4(a[row][i], a[row][i+1], a[row][i+2], a[row][i+3]) * vec4(b[i][0], b[i+1][0], b[i+2][0], b[i+3][0]);
			float sumsum = dot(sum, vec4(1.0f, 1.0f, 1.0f, 1.0f));
			for (; i < a.width; i++) sumsum += a[row][i] * b[i][0];
			r = sumsum;
		}
	)", nullptr);

	mathg::Matrix ag(size, size, mathg::Type::float_);
	mathg::Matrix bg(size, 1, mathg::Type::float_);
	mathg::Matrix cg(size, 1, mathg::Type::float_);
	if (!multiply.ok() || !ag.ok() || !bg.ok() || !cg.ok()) return;

	clock_t start = clock();
	for (int time = 0; time < n; time++)
	{
		if (!cg.assign(&multiply, &ag, &bg)) return;
		if (!bg.assign(&multiply, &ag, &cg)) return;
	}
	printf("GPU (vectorized) done in %f seconds\n", (float)(clock() - start) / CLOCKS_PER_SEC);
	cg.load(c);
}

void test()
{
	test_simple_double();
	test_simple_float();
	test_simple_int();

	const int size = 1024;
	const int n = 100;
	std::vector<float> a(size * size);
	std::vector<float> b(size);
	std::vector<float> c(size);
	std::default_random_engine engine;
	std::uniform_real_distribution<float> distribution(-0.1f, 0.1f);
	for (int i = 0; i < size * size; i++) a[i] = distribution(engine);
	for (int i = 0; i < size; i++) b[i] = distribution(engine);
	std::vector<float> bb = b;

	test_cpu(a.data(), b.data(), c.data(), size, n);

	b = bb;
	test_gpu(a.data(), b.data(), c.data(), size, n);

	b = bb;
	test_gpu_vectorized(a.data(), b.data(), c.data(), size, n);

	printf("All tests done\n");
}

int main(int argc, char *argv[])
{
	if (mathg::System::init())
	{
		test();
		getchar();
	}
	mathg::System::finalize();
	return 0;
}