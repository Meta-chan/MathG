/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_FREEGLUT
#define MATHG_FREEGLUT

#include "mathg.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace mathg
{
	///Freeglut environment
	class Freeglut
	{
	private:
		static int _window;

	public:
		///Initializes Freeglut environment
		static bool init()		noexcept;
		///Returns if Freeglut environment is ok
		static bool ok()		noexcept;
		///Finalizes Freeglut environment
		static void finalize()	noexcept;
	};
}

#endif	//#ifndef MATHG_FREEGLUT

#ifdef MATHG_INCLUDE
	#ifndef MATHG_FREEGLUT_SOURCE
		#define MATHG_FREEGLUT_SOURCE
		#include "../../source/freeglut.h"
	#endif
#endif