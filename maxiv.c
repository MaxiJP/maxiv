#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

int get_p3_colour(FILE *in) {
	Uint8 byteone = (char)fgetc(in);
	Uint8 bytetwo = (char)fgetc(in);
	Uint8 bytethree = (char)fgetc(in);
	// if the first and/or second digit is a zero, its instead saved as a whitespace for readability. terrible, but so is P3 soooo meh.
	if (byteone==' ') {
		byteone=='0';
	}
	if (bytetwo==' ') {
		bytetwo=='0';
	}
	char text_color[3] = "   ";
	text_color[0]=byteone;
	text_color[1]=bytetwo;
	text_color[2]=bytethree;
	Uint8 output = 0;
	sscanf(text_color, "%d", &output);
	return output;
}

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
	
	// Read third line (w+h)
	char *pdimensions = calloc(1000, sizeof(char));
	fgets(pdimensions, 1000, in);
	while (pdimensions[0]=='#') {
		// just skip the comments, since the spec does say to allow for any amount of comments.
		fgets(pdimensions, 1000, in);
	}
	printf(pdimensions);
	
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

	int counter = 0;

	if (magicnumber[1]=='3') {
		// P6 indicates that we are reading the files as if encoded in bytes.
		for (y=0;y<height;y++){
			for(x=0;x<width;x++){
				r=get_p3_colour(in);
				fgetc(in); // getting rid of the whitespace inbetween each value
				g=get_p3_colour(in);
				fgetc(in); // getting rid of the whitespace inbetween each value
				b=get_p3_colour(in);
				// bodge, as sometimes pixels are separated by three whitespaces, and sometimes its a \n. This renderer processes a new pixel and new line as the same, so we just ensure it is handled the same.
				if (fgetc(in)!='\n') {
					fgetc(in);
					fgetc(in);
				} 
				// back to it
				Uint32 color = SDL_MapRGB(psurface->format, r, g, b);
				pixel.x=x;
				pixel.y=y;
				SDL_FillRect(psurface, &pixel, color);
			}
		}
		printf("%d", counter);
	} else if (magicnumber[1]=='6') {
		//FORMAT:
		//RRR GGG BBB   #
		//Both 0x20 and 0x30 both represent '0'. don't question it, its stupid i know
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
	} else {
		printf("Unsupported PPM file type supplied. P3 and P6 are the only supported file types currently.");
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
