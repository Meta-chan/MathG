/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef MATHG_SDL2
#define MATHG_SDL2

#include <GL/glew.h>
#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_opengl.h>
#else
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_opengl.h>
#endif

namespace mathg
{
	///SDL2 environment
	class SDL2
	{
	private:
		static SDL_Window *_window;
		static SDL_GLContext _context;

	public:
		///Initializes SDL2 environment
		static bool init()		noexcept;
		///Returns if SDL2 environment is ok
		static bool ok()		noexcept;
		///Finalizes SDL2 environment
		static void finalize()	noexcept;
	};
}

#ifdef MATHG_IMPLEMENT
	#include "../../source/sdl2.h"
#endif

#endif	//#ifndef MATHG_SDL2