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
	
	#endif
	
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
#endif

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
#endif

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
#endif

	// ensure infile is (likely) a 24-bit uncompressed BMP 4.0
	if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
		bi.biBitCount != 24 || bi.biCompression != 0)
	{
		fclose(outptr);
		fclose(inptr);
		fprintf(stderr, "Unsupported file format.\n");
		return 5;
	}

	// write outfile's BITMAPFILEHEADER
	fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

	// write outfile's BITMAPINFOHEADER
	fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

	// determine padding for scanlines
	int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

	// iterate over infile's scanlines
	for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
	{
		// iterate over pixels in scanline
		for (int j = 0; j < bi.biWidth; j++)
		{
			// temporary storage
			RGBTRIPLE triple;

			// read RGB triple from infile
			fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

			// write RGB triple to outfile
			fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
		}

		// skip over padding, if any
		fseek(inptr, padding, SEEK_CUR);

		// then add it back (to demonstrate how)
		for (int k = 0; k < padding; k++)
		{
			fputc(0x00, outptr);
		}
	}

	// close infile
	fclose(inptr);

	// close outfile
	fclose(outptr);

	// that's all folks
	return 0;
}
