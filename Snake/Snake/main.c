///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////SNAKE GAME BY Balázs Oláh////////////////////////////
///////////////////////////////////////////////////////////I know it's shit////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "SDL.h"
#include "sodium.h"


float xBody[211];
float yBody[211];

int processEvents(SDL_Window *window, int size, int *xspeed, int *yspeed, int speed, int def)
{
	SDL_Event event;
	int done = 0;

	while (SDL_PollEvent(&event)) //checks for events
	{
		switch (event.type)
		{
		case SDL_WINDOWEVENT_CLOSE:
		{
			if (window)
			{
				SDL_DestroyWindow(window);
				window = NULL;
				done = 1;
			}
		}
		break;
		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				done = 1;
				break;
			}
		}
		break;
		case SDL_QUIT:
			done = 1;
			break;
		}

		//directions from input
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_UP])
		{
			*xspeed = 0; *yspeed = -(def+speed);
		}
		if (state[SDL_SCANCODE_DOWN]) 
		{
			*xspeed = 0; *yspeed = (def+speed);
		}
		if (state[SDL_SCANCODE_RIGHT])
		{
			*xspeed = (def+speed); *yspeed = 0;
		}
		if (state[SDL_SCANCODE_LEFT]) 
		{
			*xspeed = -(def+speed); *yspeed = 0;
		}
	}
	return done;
}

void doRender(SDL_Renderer* renderer, int length, int xspeed, int yspeed, int size, int *r, int rx, int ry, int *speed, int *fail)
{
	//render display
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //white(background)
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 186, 0, 255); //green(snake)

	//feeding coordinates to the bodyparts
	if (length != 5)
	{
		for (int i = length; i >= 0; i--)
		{
			if (i != 0)
			{
				xBody[i] = xBody[i - 1];
				yBody[i] = yBody[i - 1];
			}
			else
			{
				xBody[0] += xspeed;
				yBody[0] += yspeed;
			}
		}
	}
	else
	{
		xBody[0] += xspeed;
		yBody[0] += yspeed;
	}

	//snake rendering ??
	for (int i = 0; i <= length; i++)
	{
		SDL_Rect rect = { xBody[i], yBody[i], size, size }; //x,y,w,h
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect white = { 0, 0, size, size }; //x,y,w,h
	SDL_RenderFillRect(renderer, &white);

	SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
	SDL_Rect appleRect = { rx, ry, 24, 24 };
	SDL_RenderFillRect(renderer, &appleRect);

	//border collision
	if (xBody[0] < 0)
		xBody[0] = 1280;
	if (xBody[0] > 1280)
		xBody[0] = 0;
	//y-axis
	if (yBody[0] < 0)
		yBody[0] = 720;
	if (yBody[0] > 720)
		yBody[0] = 0;

	//collision with apple
	if ((xBody[0] - rx < 20 && xBody[0] - rx > -20) && (yBody[0] - ry < 20 && yBody[0] - ry > -20))
	{
		*r = 0;
	}

	//collision with itself
	for (int i = 5; i < length; i++)
	{
		if ((xBody[0] - xBody[i] <= 6 && xBody[0] - xBody[i] >= -6) && (yBody[0] - yBody[i] <= 6 && yBody[0] - yBody[i] >= -6))
			*fail = 1;
	}

	//presenting
	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
	////////defaults/////////
	int score = 0; //eaten apples
	int length = 0; //snake's length

	int size = 24;
	int def = 0; //default speed
	int speed = 2; //default speed modifier   |   actual speed is = def + speed

	//directions (snake goes right)
	int xspeed = speed;
	int yspeed = 0;
	/////////////////////////

	//boolean randomizer variable stuff of DOOM
	int r = 0;
	int rx;
	int ry;

	int fail = 0;

	SDL_Renderer* renderer;
	SDL_Window* window;

	//initialization
	SDL_Init(SDL_INIT_VIDEO);

	//window creation
	window = SDL_CreateWindow("Snake",
								SDL_WINDOWPOS_UNDEFINED,
								SDL_WINDOWPOS_UNDEFINED,
								1280,
								720,
								0
								);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	int done = 0;

	while (!done)
	{
		if (fail)
		{
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			printf("You've eaten your tail and lost! You scored %d! \n", score - 1);
			printf("Press any key to escape! \n");
			char lose;
			scanf_s("%c", &lose);
			return 0;
		}
		//random apples
		if (r == 0)
		{
			if (score == 41)
			{
				SDL_DestroyWindow(window);
				SDL_DestroyRenderer(renderer);
				printf("Congratulations! You've won with the score of %d! \n", score - 1);
				printf("Press any key to escape! \n");
				char win;
				scanf_s("%c", &win);
				return 0;
			}
			score++;
			if (length % 10 == 0)
				speed++;
			length += 5;
			rx = randombytes_uniform(1280-24);
			ry = randombytes_uniform(720-24);
			if (rx < 24 && ry < 24) 
				r = 0;
			else
				r = 1;
		}

		//check for events
		done = processEvents(window, size, &xspeed, &yspeed, speed, def, r);

		//rendering the snake, and the apple
		doRender(renderer, length, xspeed, yspeed, size, &r, rx, ry, &speed, &fail);

		SDL_Delay(30);
	}

	//Close and destroy the window
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	printf("You've scored %d points! \n", score-1);
	printf("Press any key to escape! \n");

	char esc;
	scanf_s("%c", &esc);

	//clean up
	SDL_Quit();
	return 0;
}
