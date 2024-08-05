// modify by lebao music
 
#include "port_cfg.h"
#include "helper_file.h"

 /*
  *
  * Copyright 2016 - 2017 Marc Volker Dickmann
  * Project: LibBMP
  */

#include "bmp_file.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include "libbmp.h"


// BMP_HEADER
static void       bmp_header_init_df(bmp_header* header, const int width, const int height);
static bmp_error  bmp_header_write(const bmp_header* header, os_FILE img_file);
static bmp_error  bmp_header_read(bmp_header* header, os_FILE img_file);


 // BMP_HEADER
static void
bmp_header_init_df(bmp_header *header,
	const int   width,
	const int   height)
{
	header->bfSize = (sizeof(bmp_pixel) * width + BMP_GET_PADDING(width))
		* abs(height);
	header->bfReserved = 0;
	header->bfOffBits = 54;
	header->biSize = 40;
	header->biWidth = width;
	header->biHeight = height;
	header->biPlanes = 1;
	header->biBitCount = 24;
	header->biCompression = 0;
	header->biSizeImage = 0;
	header->biXPelsPerMeter = 0;
	header->biYPelsPerMeter = 0;
	header->biClrUsed = 0;
	header->biClrImportant = 0;
}

static bmp_error
	bmp_header_write(const bmp_header *header,
		os_FILE img_file)
{
	unsigned short magic = 0;

	if (header == NULL)
	{
		return BMP_HEADER_NOT_INITIALIZED;
	}
	else if (img_file == NULL)
	{
		return BMP_FILE_NOT_OPENED;
	}

	// Since an adress must be passed to fwrite, create a variable!
	magic = BMP_MAGIC;
	os_fwrite(&magic, sizeof(magic), 1, img_file);

	// Use the type instead of the variable because its a pointer!
	os_fwrite(header, sizeof(bmp_header), 1, img_file);
	return BMP_OK;
}

static bmp_error
	bmp_header_read(bmp_header *header,
		os_FILE img_file)
{
	// Since an adress must be passed to fread, create a variable!
	unsigned short magic;

	if (img_file == NULL)
	{
		return BMP_FILE_NOT_OPENED;
	}


	// Check if its an bmp file by comparing the magic nbr:
	if (os_fread(&magic, sizeof(magic), 1, img_file) != 1 ||
		magic != BMP_MAGIC)
	{
		return BMP_INVALID_FILE;
	}

	if (os_fread(header, sizeof(bmp_header), 1, img_file) != 1)
	{
		return BMP_ERROR;
	}

	return BMP_OK;
}

// BMP_PIXEL

void
bmp_pixel_init(bmp_pixel           *pxl,
	const unsigned char  red,
	const unsigned char  green,
	const unsigned char  blue)
{
	pxl->red = red;
	pxl->green = green;
	pxl->blue = blue;
}

// BMP_IMG

void
bmp_img_alloc(bmp_img *img)
{
	const size_t h = abs(img->img_header.biHeight);
	size_t y = 0;

	// Allocate the required memory for the pixels:
	img->img_pixels = os_malloc(sizeof(bmp_pixel*) * h);

	for (y = 0; y < h; y++)
	{
		img->img_pixels[y] = os_malloc(sizeof(bmp_pixel) * img->img_header.biWidth);
	}
}

void
bmp_img_init_df(bmp_img    * img,
	const int  width,
	const int  height)
{
	// INIT the header with default values:
	bmp_header_init_df(&img->img_header, width, height);
	bmp_img_alloc(img);
}

void
bmp_img_free(bmp_img *img)
{
	const size_t h = abs(img->img_header.biHeight);
	size_t y = 0;
	for (y = 0; y < h; y++)
	{
		os_free(img->img_pixels[y]);
	}
	os_free(img->img_pixels);
}

bmp_error
	bmp_img_write(const bmp_img *img,
		const char    *filename)
{
	bmp_error err;
	size_t h;
	size_t offset;
	// Create the padding:
	const unsigned char padding[3] = { '\0', '\0', '\0' };
	size_t y = 0;

	os_FILE img_file = os_fopen(filename, "wb");
	if (img_file == NULL)
	{
		return BMP_FILE_NOT_OPENED;
	}

	// NOTE: This way the correct error code could be returned.
	err = bmp_header_write(&img->img_header, img_file);
	if (err != BMP_OK)
	{
		// ERROR: Could'nt write the header!
		os_fclose(img_file);
		return err;
	}

	// Select the mode (bottom-up or top-down):
	h = abs(img->img_header.biHeight);
	offset = (img->img_header.biHeight > 0 ? h - 1 : 0);

	// Write the content:
	for (y = 0; y < h; y++)
	{
		// Write a whole row of pixels to the file:
		os_fwrite(img->img_pixels[abs(offset - y)], sizeof(bmp_pixel), img->img_header.biWidth, img_file);

		// Write the padding for the row!
		os_fwrite(padding, sizeof(unsigned char), BMP_GET_PADDING(img->img_header.biWidth), img_file);
	}

	// NOTE: All good!
	os_fclose(img_file);
	return BMP_OK;
}

bmp_error  
	bmp_img_write_buffer(const bmp_img* img, 
		char ** buffer, 
		int* size)
{
	unsigned short magic = BMP_MAGIC;
	char* p = NULL;
	size_t h = 0, offset = 0, y = 0;
	const unsigned char padding[3] = { '\0', '\0', '\0' };

	if (img == NULL || img->img_header.biWidth <= 0 || img->img_header.biHeight <= 0)
		return BMP_HEADER_NOT_INITIALIZED;

	*size = 3 * img->img_header.biWidth * img->img_header.biHeight + 54;

	if (BMP_GET_PADDING(img->img_header.biWidth) > 0) {
		*size += sizeof(unsigned char) * BMP_GET_PADDING(img->img_header.biWidth) * img->img_header.biHeight;
	}

	*buffer = os_malloc(*size);
	p = *buffer;

	os_memcpy(p, &magic, sizeof(magic));
	p += sizeof(magic);

	os_memcpy(p, &img->img_header, sizeof(bmp_header));
	p += sizeof(bmp_header);
	
	h = abs(img->img_header.biHeight);
	offset = (img->img_header.biHeight > 0 ? h - 1 : 0);

	for (y = 0; y < h; y++)
	{
		os_memcpy(p, img->img_pixels[abs(offset - y)], sizeof(bmp_pixel) * img->img_header.biWidth);
		p += sizeof(bmp_pixel) * img->img_header.biWidth;

		if (BMP_GET_PADDING(img->img_header.biWidth) > 0) {
			os_memcpy(p, padding, sizeof(unsigned char) * BMP_GET_PADDING(img->img_header.biWidth));
			p += sizeof(unsigned char) * BMP_GET_PADDING(img->img_header.biWidth);
		}
	}

	return BMP_OK;
}

bmp_error
	bmp_img_read(bmp_img    *img,
		const char *filename)
{
	bmp_error err;

	size_t h = 0;
	size_t offset = 0;
	size_t padding = 0;

	size_t items = 0;

	size_t y = 0;

	os_FILE img_file = os_fopen(filename, "rb");
	if (img_file == NULL)
	{
		return BMP_FILE_NOT_OPENED;
	}

	// NOTE: This way the correct error code can be returned.
	err = bmp_header_read(&img->img_header, img_file);

	if (err != BMP_OK)
	{
		// ERROR: Could'nt read the image header!
		os_fclose(img_file);
		return err;
	}

	bmp_img_alloc(img);

	// Select the mode (bottom-up or top-down):
	h = abs(img->img_header.biHeight);
	offset = (img->img_header.biHeight > 0 ? h - 1 : 0);
	padding = BMP_GET_PADDING(img->img_header.biWidth);

	// Needed to compare the return value of fread
	items = img->img_header.biWidth;

	// Read the content:
	for (y = 0; y < h; y++)
	{
		// Read a whole row of pixels from the file:
		if (os_fread(img->img_pixels[abs(offset - y)], sizeof(bmp_pixel), items, img_file) != items)
		{
			os_fclose(img_file);
			return BMP_ERROR;
		}

		// Skip the padding:
		os_fseek(img_file, padding, SEEK_CUR);
	}

	// NOTE: All good!
	os_fclose(img_file);
	return BMP_OK;
}
