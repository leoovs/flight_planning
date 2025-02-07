#include <iostream>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <uavpf/uavpf.h>

int main()
{
	uavpf::Version v = uavpf::GetCurrentVersion();
	std::cout << "flight_planning current version is "
		<< v.Major << '.'
		<< v.Minor << '.'
		<< v.Patch << '\n';

	bool sdlInitResult = SDL_Init(SDL_INIT_VIDEO);
	if (!sdlInitResult)
	{
		std::cerr << SDL_GetError() << '\n';
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow("flight_planning", 1280, 720, SDL_WINDOW_OPENGL);
	if (nullptr == window)
	{
		std::cerr << SDL_GetError() << '\n';
		return EXIT_FAILURE;
	}

	SDL_GLContext ctx = SDL_GL_CreateContext(window);
	if (nullptr == ctx)
	{
		std::cerr << SDL_GetError() << '\n';
		return EXIT_FAILURE;
	}

	if (GLenum err = glewInit(); err != GLEW_OK)
	{
		std::cerr << glewGetErrorString(err) << '\n';
		return EXIT_FAILURE;
	}
	glClearColor(0.5f, 0.0f, 0.5f, 1.0f);

	bool running = true;
	while (running)
	{
		for (SDL_Event event{}; SDL_PollEvent(&event);)
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				running = false;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DestroyContext(ctx);
	SDL_DestroyWindow(window);
}

