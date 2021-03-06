/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG
#define MATHG

#include <GL/glew.h>

namespace mathg
{
	class Matrix;
	class Function;

	///MathG system, used to initialization and finalization
	class MathG
	{
	friend Matrix;
	friend Function;
	private:
		static const GLuint _error;
		static bool _ok;
		static bool _initialized;
		static GLuint _vao2d;
		static GLuint _vbo2d;
		static GLuint _distribute2d;
		static GLint _max_binded;
		static GLint _max_accessed;
		
		static bool _gl_ok()	noexcept;

	public:
		///Initializes MathG system
		static bool init()		noexcept;
		///Returns if MathG system is ok
		static bool ok()		noexcept;
		///Finalizes MathG system
		static void finalize()	noexcept;
	};
}

#endif	//#ifndef MATHG

#ifdef MATHG_INCLUDE
	#ifndef MATHG_SOURCE
		#define MATHG_SOURCE
		#include "../../source/mathg.h"
	#endif
#endif

/**@mainpage Welcome to MathG!
# Welcome to MathG!
Take a look at the following example:
```
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
	)");

//Multiplication
cg.assign(&multiply, &ag, &bg);

//Result
cg.load(c);
```
And that's it! We performed matrix multiplication in GPU!

### Structure and dependencies
MathG core depends on [GLEW](http://glew.sourceforge.net) and requires `OpenGL 3.3` context. If you want to provide this context yourself, then include bare `MathG`:
```
#define MATHG_INCLUDE
#include <mathg/matrix.h>

int main()
{
	mathg::MathG::init();
	//MathG job
	mathg::MathG::finalize();
}
```

But if you don't want to care about `OpenGL` context, use one of environments!
```
#define MATHG_INCLUDE
#include <mathg/default.h>
#include <mathg/matrix.h>

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
 - `<mathg/egl.h>` \- EGL environment, defines `mathg::EGL`, depends on [EGL](https://www.khronos.org/egl/) (not supported)
 
### Shaders
MathG preprocesses source codes into valid `GLSL` shaders. Here is a pair of moments you need to know about:
 - `matrix` macro is used to declare a matrix.
 - You can  use brackets with matrices.
 - You can use `.width` and `.height` expressions with matrices. Appropriate uniform variables are created automatically.
 - You can use `row` and `column` to get what cell we are calculating result to.
 - You can only write to `out` variable.
 
### Platforms
The code can be potentially run at every platform that supports `OpenGL 3.3`. Currently Windows and Linux are supported.

###### P.S. My code is not dirty, it is alternatively clean.
*/