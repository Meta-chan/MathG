/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

HDC mathg::Windows::_dc					= NULL;
HGLRC mathg::Windows::_gc				= NULL;
WNDCLASS mathg::Windows::_window_class	= { 0 };
HWND mathg::Windows::_window			= NULL;
bool mathg::Windows::_initialized		= false;
bool mathg::Windows::_ok				= false;

LRESULT CALLBACK mathg::Windows::_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool mathg::Windows::init() noexcept
{
	if (_initialized) return _ok;
	_initialized = true;

	//Registering class
	_window_class.lpfnWndProc = _window_proc;
	_window_class.style = CS_HREDRAW | CS_VREDRAW;
	_window_class.hInstance = GetModuleHandle(NULL);
	_window_class.lpszClassName = TEXT("MathG");
	_window_class.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	if (RegisterClass(&_window_class) == 0) return false;

	//Create window
	_window = CreateWindow(TEXT("MathG"), TEXT("MathG"), 0,
		0, 0, 100, 100, NULL, NULL, GetModuleHandle(NULL), NULL);
	if (_window == NULL) return false;

	//Setting pixel format
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	if ((_dc = GetDC(_window)) == NULL) return false;
	int pf = ChoosePixelFormat(_dc, &pfd);
	if (pf == 0) return false;
	if (!SetPixelFormat(_dc, pf, &pfd)) return false;
	if (DescribePixelFormat(_dc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0) return false;
	ReleaseDC(_window, _dc);
	_dc = NULL;
	
	//Create OpenGL context
	if ((_dc = GetDC(_window)) == NULL) return false;
	if ((_gc = wglCreateContext(_dc)) == NULL) return false;
	if (!wglMakeCurrent(_dc, _gc)) return false;

	_ok = true;
	return true;
}

bool mathg::Windows::ok() noexcept
{
	return _ok;
}

void mathg::Windows::finalize() noexcept
{
	if (_gc != NULL)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(_gc);
		_gc = NULL;
	}

	if (_dc != NULL)
	{
		ReleaseDC(_window, _dc);
		_dc = NULL;
	}

	UnregisterClass(TEXT("MathG"), GetModuleHandle(NULL));

	if (_window != NULL)
	{
		DestroyWindow(_window);
		_window = NULL;
	}

	_initialized = false;
	_ok = false;
}