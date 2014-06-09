#include "SDL.h"

SDL_Surface *init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Cannot initialize SDL\n");
		return NULL;
	}

	SDL_Rect **modes;

	/* Get available fullscreen/hardware modes */
	modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

	/* Check is there are any modes available */
	if(modes == (SDL_Rect **)0) {
		fprintf(stderr, "No modes available!\n");
		return NULL;
	}

	/* Check if our resolution is restricted */
	if(modes == (SDL_Rect **)-1) {
		fprintf(stderr, "Can't choose a resolution.\n");
		return NULL;
	}

	/* Take the first mode, typically the best one. */
	/* TODO allow to select another mode as well. */

	SDL_Surface *screen = SDL_SetVideoMode(modes[0]->w, modes[0]->h, 0,
		SDL_FULLSCREEN|SDL_HWSURFACE);
	if (!screen) {
		fprintf(stderr, "Cannot set the video mode to %dx%d\n",
			modes[0]->w, modes[0]->h);
		return NULL;
	}

	SDL_ShowCursor(0);

	return screen;
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	SDL_Surface *screen = init_sdl();
	if (!screen)
		return 1;

	int done = 0;
	SDL_Event event;

	while(done <20) {
		if ( SDL_PollEvent(&event) ) {
			switch (event.type) {
			case SDL_MOUSEBUTTONUP:
				fprintf(stderr, "Button up\n");
				done++;
				break;
			case SDL_MOUSEBUTTONDOWN:
				fprintf(stderr, "Button down\n");
				break;
			case SDL_QUIT:
				done = 1999;
				break;
			default:
				break;
			}
		} else {
			SDL_Delay(10);
		}
	}
	SDL_Quit();

	return 0;
}
