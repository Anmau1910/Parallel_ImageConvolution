#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

float filters[9][3][3] = {
	{
		{0.0625, 0.125, 0.0625}, 
		{0.125, 0.25, 0.125},
	 	{0.0625, 0.125, 0.0625}
	},
	{
		{-1, -2, -1},
		{0, 0, 0},
		{1, 2, 1}
	}, 
	{
		{-2, -1, 0}, 
		{-1, 1, 1},
		{0, 1, 2}
	},
	{
		{0, 0, 0},
		{0, 1, 0},
		{0, 0, 0}
	},
	{
		{1, 0, -1},
		{2, 0, -2},
		{1, 0, -1}
	},
	{
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1}
	},
	{
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	},
	{
		{0, -1, 0},
		{-1, 5, -1},
		{0, -1, 0}
	},
	{
		{1, 2, 1},
		{0, 0, 0},
		{-1, -2, -1}
	}
};
enum filter { BLUR, BSOBEL, EMBOSS, IDENT, LSOBEL, OUTL, RSOBEL, SHARP, TSOBEL };

void process(const char *filepath, const char *outfile);
