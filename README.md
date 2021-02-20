# Welcome to MathG!
Ultra-light and simple GPU programming library. Just look at an example:
```c++
//Creating matrices
mathg::Matrix ag(2, 2, mathg::Type::double_);
mathg::Matrix bg(2, 1, mathg::Type::double_);
mathg::Matrix cg(2, 1, mathg::Type::double_);

//Storing values into matrices
double a[4] = { 1.0, 2.0, 3.0, 4.0 };
double b[2] = { 1.0, 1.0 };
double c[2];
ag.store(a);
bg.store(b);

//Matrix multiplication function
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

//Multiplication
cg.assign(&multiply, &ag, &bg);

//Result
cg.load(c);
```
And that's it! We performed matrix multiplication in GPU!

### Shaders
MathG preprocesses source codes into valid `GLSL` shaders. Here are a pair of moments you need to know about:
 - `matrix` macro is used to declare a matrix.
 - You can  use brackets with matrices.
 - You can use `.width` and `.height` expressions with matrices. Appropriate uniform variables are created automatically.
 - You can use `row` and `column` to get what cell we are calculating result to.
 - You can only write to `out` variable.
 
### Platforms
The code can be potentially run at every platform that supports `OpenGL 3.3`. Currently Windows and Linux are supported.

### Installation
After installing dependencies, you can do just this:
```c++
#define MATHG_IMPLEMENT
#include <mathg.h>
```

### Dependencies
MathG depends on:
 - [GLEW](http://glew.sourceforge.net)
 - Libraries to create OpenGL environment, which can be:
   - [SDL](https://www.libsdl.org) (`MATHG_SDL2` macro needs to be defined)
   - [FreeGlut](http://freeglut.sourceforge.net) (`MATHG_FREEGLUT` macro needs to be defined)
   - [EGL](https://www.khronos.org/egl/) (default option)

### Documentation
The code contains documentation comments, you may use [Doxygen](https://www.doxygen.nl/manual/starting.html) to create documentation. Also feel free to contact me any time!

###### P.S. My code is not dirty, it is alternatively clean.