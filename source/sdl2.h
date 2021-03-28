/*
	Part of the MathG Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

SDL_Window *mathg::SDL2::_window	= nullptr;
SDL_GLContext mathg::SDL2::_context	= NULL;

bool mathg::SDL2::init() noexcept
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	_window = SDL_CreateWindow("MathG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 100, 100, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	if (_window == nullptr) return false;
	_context = SDL_GL_CreateContext(_window);
	if (_context == nullptr) return false;
	return true;
}

bool mathg::SDL2::ok() noexcept
{
	return _context != nullptr;
}

void mathg::SDL2::finalize() noexcept
{
	if (_context != nullptr)
	{
		SDL_GL_DeleteContext(_context);
		_context = nullptr;
	}

	if (_window != nullptr)
	{
		SDL_DestroyWindow(_window);
		_window = nullptr;
	}
}