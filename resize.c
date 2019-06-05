#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

#define DEBUG

int main(int argc, char **argv)
{
#ifdef DEBUG
	argc = 4;
	char* arr[4];
	argv = arr;
	argv[0] = "./resize";
	argv[1] = "2";
	argv[2] = "small.bmp";
	argv[3] = "small_2.bmp";
	
#endif // DEBUG
	
	// ensure proper usage
	if (argc != 4)
	{
		printf("Usage: ./resize n infile outfile\n");
		return 1;
	}

#ifdef DEBUG
	printf("DEBUG INFORMATION\n");
	printf("_______________________\n");
	for (int i = 0; i != argc; ++i)
	{
		printf("argv[%d] -- %s\n", i, argv[i]);
	}
	printf("_______________________\n");
	printf("DEBUG INFORMATION\n\n\n");
#endif //DEBUG

	int n = atoi(argv[1]);
	 if (n < 1 || n > 100)
	{
		printf("Usage:  n from 1 to 100\n");
		return 2;
	}

	// remember filenames
	char *infile = argv[2];
	char *outfile = argv[3];

	// open input file 
	FILE *inptr = fopen(infile, "r");
	if (inptr == NULL)
	{
		printf("Could not open %s.\n", infile);
		return 3;
	}

	// open output file
	FILE *outptr = fopen(outfile, "w");
	if (outptr == NULL)
	{
		fclose(inptr);
		fprintf(stderr, "Could not create %s.\n", outfile);
		return 4;
	}

	// read infile's BITMAPFILEHEADER
	BITMAPFILEHEADER bf;
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

#ifdef DEBUG
	printf("DEBUG INFORMATION\n");
	printf("_______________________\n");
    printf("bfType = %d\n", bf.bfType);
    printf("bfSize = %d\n", bf.bfSize);
    printf("bfReserved1 = %d\n", bf.bfReserved1);
    printf("bfReserved2 = %d\n", bf.bfReserved2);
    printf("bfOffBits = %d\n", bf.bfOffBits);
    printf("_______________________\n");
	printf("DEBUG INFORMATION\n\n\n");
#endif //DEBUG

	// read infile's BITMAPINFOHEADER
	BITMAPINFOHEADER bi;
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

#ifdef DEBUG
	printf("DEBUG INFORMATION\n");
	printf("_______________________\n");
	printf("biSize = %d\n", bi.biSize);
    printf("biWidth = %d\n", bi.biWidth);
    printf("biHeight = %d\n", bi.biHeight);
    printf("biPlanes = %d\n", bi.biPlanes);
    printf("biBitCount = %d\n", bi.biBitCount);
    printf("biCompression = %d\n", bi.biCompression);
    printf("biSizeImage = %d\n", bi.biSizeImage);
    printf("biXPelsPerMeter = %d\n", bi.biXPelsPerMeter);
    printf("biYPelsPerMeter = %d\n", bi.biYPelsPerMeter);
    printf("biClrUsed = %d\n", bi.biClrUsed);
    printf("biClrImportant = %d\n", bi.biClrImportant);
    printf("_______________________\n");
	printf("DEBUG INFORMATION\n\n\n");
#endif //DEBUG

	// ensure infile is (likely) a 24-bit uncompressed BMP 4.0
	if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
		bi.biBitCount != 24 || bi.biCompression != 0)
	{
		fclose(outptr);
		fclose(inptr);
		fprintf(stderr, "Unsupported file format.\n");
		return 5;
	}

	//
	int old_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

	int old_width = bi.biWidth;
	int old_height = abs(bi.biHeight);

	bi.biWidth = n * bi.biWidth;
	bi.biHeight = n * bi.biHeight;

	int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;


	bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight);

	bf.bfSize = bi.biSizeImage + bf.bfOffBits;

	// write outfile's BITMAPFILEHEADER
	fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

	// write outfile's BITMAPINFOHEADER
	fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

	// temporary storage
	RGBTRIPLE* arrey_triple = malloc(sizeof(RGBTRIPLE) * old_width);

	// iterate over infile's scanlines
	for (int i = 0; i < old_height; i++)
	{
		// iterate over pixels in scanline
		for (int j = 0; j < old_width; j++)
		{
			// read RGB triple from infile
			fread(arrey_triple + j, sizeof(RGBTRIPLE), 1, inptr);
		}
#ifdef DEBUG
		printf("DEBUG INFORMATION\n");
		printf("_______________________\n");
		for (int z = 0; z != old_width; ++z)
		{
			printf("tripl[%d] -- %X\n", z, arrey_triple + z);
		}
		printf("_______________________\n");
		printf("DEBUG INFORMATION\n\n\n");
#endif //DEBUG
		// skip over padding, if any
		fseek(inptr, old_padding, SEEK_CUR);

		for (int j = 0; j < n; j++)
		{
			for (int jj = 0; jj < old_width; jj++)
			{
				// write RGB triple to outfile
				fwrite(arrey_triple + jj, sizeof(RGBTRIPLE), n, outptr);
			}
			// then add it back (to demonstrate how)
			for (int k = 0; k < padding; k++)
			{
				fputc(0x00, outptr);
			}
		}
	}

	free(arrey_triple);

	// close infile
	fclose(inptr);

	// close outfile
	fclose(outptr);

	// that's all folks
	return 0;
}
