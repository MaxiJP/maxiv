#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

int main(int argc, char **argv) 
{
	printf("max's image viewer. (c) 2026 max & co.\n\n");

	const int width;
	const int height;
	
	char magicnumber[3];
	char comment[1000];
	int colordepth;
	
	if (argc!=2){
		printf("Usage: maxiv <pathfile>\n");
		return 0;
	}
	
	FILE *in = fopen(argv[1], "r");
	if (in == NULL) {
		printf("file \"%s\" doesn't exist.\n", argv[1]);
		return 1;
	}

	char *pthroway = calloc(1000, sizeof(char));
	// Read first line (Magic number)
	fgets(pthroway, 1000, in);
	sscanf(pthroway, "%s\n", &magicnumber);
	printf("Magic Number: %s\n", magicnumber);
	
	// Read second line (comment)
	fgets(pthroway, 1000, in);
	printf("Comment: %s", pthroway);
	
	// Read third line (w+h)
	char *pdimensions = calloc(1000, sizeof(char));
	fgets(pdimensions, 1000, in);
	
	sscanf(pdimensions, "%d %d\n", &width, &height);
	printf("Dimensions: %d, %d\n", width, height);
	
	free(pdimensions);
	
	// Read fourth line (should be 255, but we don't sanity check ts)
	fgets(pthroway, 1000, in);
	sscanf(pthroway, "%d\n", &colordepth);
	printf("Color depth: %d\n", colordepth);
	if (colordepth!=255){
		printf("maxiv only supports images with a color depth of 255.\n");
		return 1;
	}
	free(pthroway);
	
	char title[100] = "max's image viewer. (";
	strcat(title, argv[1]);
	strcat(title, ")");

	SDL_Window *pwindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0x00000000);
	SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);

	Uint8 r,g,b;
	int x, y;

	SDL_Rect pixel = (SDL_Rect){x,y,1,1};

	for (y=0;y<height;y++){
		for(x=0;x<width;x++){
			r=(char)fgetc(in);
			g=(char)fgetc(in);
			b=(char)fgetc(in);
			Uint32 color = SDL_MapRGB(psurface->format, r, g, b);
			pixel.x=x;
			pixel.y=y;
			SDL_FillRect(psurface, &pixel, color);
		}
	}
	
	SDL_UpdateWindowSurface(pwindow);

	int running = 1;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				running = 0;
			}
		}
		SDL_Delay(25);
	}
}
