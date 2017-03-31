#include <stdio.h>
#include "gd.h"

void write_gif (image, nrows, ncols)
{
	FILE *out;
	int white, out;

	/* output image */
	gdImagePtr im_out;

	/* Create output image. */
	im_out = gdImageCreate(nrow, ncols);

	im_out->sx = ncols;
	im_out->sy = nrows;

	for (j=0;j<ncols;j++) {
	for (i=0;i<nrows;i++) {
	im->pixels[j] = (unsigned char) image[i][j];
	}}

	out = fopen("image.gif", "wb");
	/* Write GIF */
	GIFEncode(im_out, ncols, nrows, int GInterlace, int Background, int Transparent,
	16, int *Red, int *Green, int *Blue, gdImagePtr im)

	gdImageGif(im_out, out);
	fclose(out);
	gdImageDestroy(im_out);
	return 0;
}

