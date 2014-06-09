#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_image.h>

SDL_Surface *init_sdl()
{
	printf("Initializing SDL...\n");
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Cannot initialize SDL\n");
		return NULL;
	}

	SDL_Rect **modes;

	printf("Finding video modes...\n");
	/* Get available fullscreen/hardware modes */
	modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE|SDL_DOUBLEBUF);

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

	printf("Setting up video mode to %dx%d...\n", modes[0]->w, modes[0]->h);
	SDL_Surface *screen = SDL_SetVideoMode(modes[0]->w, modes[0]->h, 0,
		SDL_FULLSCREEN|SDL_HWSURFACE);
	if (!screen) {
		fprintf(stderr, "Cannot set the video mode to %dx%d\n",
			modes[0]->w, modes[0]->h);
		return NULL;
	}

	printf("Hiding cursor...\n");
	SDL_ShowCursor(0);

	return screen;
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	int log_file;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	log_file = open("/var/log/desco.log", O_WRONLY | O_CREAT, mode);

	if (dup2(log_file, fileno(stderr)) != fileno(stderr) ||
		dup2(log_file, fileno(stdout)) != fileno(stdout)) {
		perror("Unable to redirect output");
	}

	SDL_Surface *screen = init_sdl();
	if (!screen)
		return 1;

	printf("Opening background image...\n");
	SDL_Surface *background = IMG_Load("/root/desco/desco.png");
	if (background) {
		printf("Showing background image...\n");
		SDL_BlitSurface(background, NULL, screen, NULL);
		SDL_Flip(screen);
	}

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
	printf("Terminating.\n");
	SDL_Quit();

	return 0;
}
