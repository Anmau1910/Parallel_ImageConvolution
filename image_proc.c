#define image_proc
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#include "image_proc.h"

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

unsigned char kern_apply(unsigned char *col1, unsigned char *col2, unsigned char *col3, float kern[3][3]) {
	int pix;
	pix = col1[0] * kern[0][0] + col1[1] * kern[0][1] + col1[2] * kern[0][2] +
	      col2[0] * kern[1][0] + col2[1] * kern[1][1] + col2[2] * kern[1][2] +
	      col3[0] * kern[2][0] + col3[1] * kern[2][1] + col3[2] * kern[2][2];
	return pix < 0 ? 0 : (pix > 255 ? 255 : pix);
}

unsigned char apply(unsigned char *img, int w, int h, int i, float kern[3][3]) {
	if(i < w || i > w * (h - 1) || !(i % w) || i % w == w - 1) 
		return 0;

	return kern_apply(img + i - w - 1, img + i - 1, img + i + w - 1, kern);
}

void process(const char *filepath, const char *outfile) {
	int w, h, chans, i;
	unsigned char *img, *out;

	if(!(img = stbi_load(filepath, &w, &h, &chans, 1))) {
		fprintf(stderr, "Error cargando imagen\n");
		exit(EXIT_FAILURE);
	}

	out = malloc(w * h * sizeof(unsigned char));

	#pragma omp parallel for
	for(i = 0; i < w * h; i++) {
		out[i] = apply(img, w, h, i, filters[OUTL]);
	}

	stbi_write_bmp(outfile, w, h, 1, out); 
	stbi_image_free(img);
	free(out);
}

