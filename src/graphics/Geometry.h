#pragma once

#define CIRCLE_STEP (float)M_PI / 16.0

#include <SDL.h>
#include <math.h>
#include <iostream>
#include <vector>

#include "Palette.h"

using namespace std;

class Geometry
{
public:
	static void SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int r, SDL_Color c);
	static void SDL_RenderHollowCircle(SDL_Renderer* renderer, int x, int y, int innerRadius, int outerRadius, SDL_Color c);
};

