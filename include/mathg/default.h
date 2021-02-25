/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifdef _WIN32
	#include "windows.h"
	namespace mathg
	{
		///Shortcut for Windows environment
		typedef Windows Default;
	}
#else
	#include "x.h"
	namespace mathg
	{
		///Shortcut for X11 environment
		typedef X Default;
	}
#endif