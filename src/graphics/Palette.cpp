#include "Palette.h"

const SDL_Color Palette::light = { 238, 238, 210, 255 };
const SDL_Color Palette::dark = { 118, 150, 86, 255 };
const SDL_Color Palette::hover = { 255, 255, 255,  166 };
const SDL_Color Palette::hint = { 0, 0, 0,  26 };
const SDL_Color Palette::text[PALETTE_TEXT_SIZE] = {
	{ 152, 151, 149, 255 },
	Palette::light,
	Palette::dark,
	{ 255, 0, 255, 255 }
};
const SDL_Color Palette::highlight[PALETTE_HIGHLIGHT_SIZE] = {
	{ 255, 255, 0, 127 },	// Last Move
	{ 255, 255, 0, 127 },	// Selected
	{ 235, 97, 80, 204 },	// None
	{ 255, 170, 0, 204 },	// CTRL
	{ 172, 206, 89, 204 },	// SHIFT
	{ 82, 176, 220, 204 }	// ALT
};
const SDL_Color Palette::debug[PALETTE_DEBUG_SIZE] = {
	{ 0, 125, 255, 204 },
	{ 170, 0, 255, 204 },
	{ 235, 97, 80, 204 },
	{ 255, 170, 0, 204 }
};