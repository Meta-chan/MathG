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
	#include <shellapi.h>
#else
	#include <unistd.h>
#endif

int mathg::Freeglut::_window = -1;

bool mathg::Freeglut::init() noexcept
{
	#ifdef _WIN32
		int argc;
		LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
		if (wargv == nullptr) return false;
		char **argv = (char**)calloc(argc, sizeof(char*));
		if (argv == nullptr) return false;
		char undefchar = '?';
		for (int i = 0; i < argc; i++)
		{
			int len = WideCharToMultiByte(CP_ACP, 0, wargv[i], -1, NULL, 0, &undefchar, NULL);
			argv[i] = (char*)calloc(len, sizeof(char));
			if (argv[i] == nullptr) return false;
			WideCharToMultiByte(CP_ACP, 0, wargv[i], -1, argv[i], len, &undefchar, NULL);
		}
		LocalFree(wargv);
	#else
		char arg0[1024];
		char *parg0 = arg0;
		readlink("/proc/self/exe", parg0, 1024);
		char **argv = &parg0;
		int argc = 1;
	#endif

	//Initializing Freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(0);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(100, 100);
	_window = glutCreateWindow("MathG");
	if (_window < 0) return false;
	glutDisplayFunc([]() {});
	glutHideWindow();
	glutMainLoopEvent();
	return true;
}

bool mathg::Freeglut::ok() noexcept
{
	return _window >= 0;
}

void mathg::Freeglut::finalize() noexcept
{
	if (_window >= 0)
	{
		glutDestroyWindow(_window);
		_window = -1;
	}
}