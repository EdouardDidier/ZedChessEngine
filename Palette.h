#pragma once

#define PALETTE_TEXT_SIZE 4
#define PALETTE_HIGHLIGHT_SIZE 6
#define PALETTE_DEBUG_SIZE 2

#include <SDL.h>

class Palette
{
public:
	static const SDL_Color light;
	static const SDL_Color dark;
	static const SDL_Color hover;
	static const SDL_Color hint;
	static const SDL_Color text[PALETTE_TEXT_SIZE];
	static const SDL_Color highlight[PALETTE_HIGHLIGHT_SIZE];
	static const SDL_Color debug[PALETTE_DEBUG_SIZE];
};

