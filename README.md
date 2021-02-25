# Welcome to MathG!
Light and simple GPU programming library. take a look at the following example:
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
MathG preprocesses source codes into valid `GLSL` shaders. Here is a pair of moments you need to know about:
 - `matrix` macro is used to declare a matrix.
 - You can  use brackets with matrices.
 - You can use `.width` and `.height` expressions with matrices. Appropriate uniform variables are created automatically.
 - You can use `row` and `column` to get what cell we are calculating result to.
 - You can only write to `out` variable.
 
### Platforms
The code can be potentially run at every platform that supports `OpenGL 3.3`. Currently Windows and Linux are supported.

### Structure and dependencies
MathG core depends on [GLEW](http://glew.sourceforge.net) and requires `OpenGL 3.3` context. If you want to provide this context yourself, then include bare `MathG`:
```c++
#define MATHG_IMPLEMENT
#include <mathg/mathg.h>

int main()
{
	mathg::MathG::init();
	//MathG job
	mathg::MathG::finalize();
}
```

But if you don't want to care about `OpenGL` context, use one of environments!
```c++
#define MATHG_IMPLEMENT
#include <mathg/default.h>
#include <mathg/mathg.h>

int main()
{
	mathg::Default::init();
	mathg::MathG::init();
	//MathG job
	mathg::MathG::finalize();
	mathg::Default::finalize();
}
```

The purpose of environment is to open `OpenGL 3.3` context (by opening a window mostly). `MathG` supports following environments:
 - `<mathg/x.h>` \- X11 environment, defines `mathg::X`, depends on X11
 - `<mathg/windows.h>` \- Windows environment, defines `mathg::Windows`
 - `<mathg/default.h>` \- X11 or Windows environment, defines `mathg::Default`
 - `<mathg/sdl2.h>` \- SDL2 environment, defines `mathg::SDL2`, depends on [SDL2](https://www.libsdl.org)
 - `<mathg/freeglut.h>` \- Freeglut environment, defines `mathg::Freeglut`, depends on [Freeglut](http://freeglut.sourceforge.net)
 - `<mathg/egl.h>` \- EGL environment, defines `mathg::EGL`, depends on [EGL](https://www.khronos.org/egl/)

### Documentation
The code contains documentation comments, you may use [Doxygen](https://www.doxygen.nl/manual/starting.html) to create documentation. Also feel free to contact me any time!

###### P.S. My code is not dirty, it is alternatively clean.