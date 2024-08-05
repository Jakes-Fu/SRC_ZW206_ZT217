/**********************
 *      INCLUDES
 *********************/
#include "port_cfg.h"

#include "qrcodegen.h"
#include "bmp_file.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int _generate_qr_code_image(const char* uri, const char* filename, char** buffer, int* bufSize, int* width);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int helper_save_qr_code_bmp(const char* uri, const char* filename) 
{
	return _generate_qr_code_image(uri, filename, NULL, NULL, NULL);
}

int helper_get_qr_code_bmp(const char* uri, char** buffer, int* size) 
{
	return _generate_qr_code_image(uri, NULL, buffer, size, NULL);
}

int helper_get_qr_code_bmp_size(const char* uri, char** buffer, int* size, int* width) 
{
	return _generate_qr_code_image(uri, NULL, buffer, size, width);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
static int _generate_qr_code_image(const char* uri, const char* filename, char** buffer, int* bufSize, int* width)
{
	enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_LOW;
	int size = 0;
	bmp_img img = { 0 };
	int x = 0, y = 0;

	// Make and print the QR Code symbol
	unsigned char qrcode[qrcodegen_BUFFER_LEN_MAX];
	unsigned char tempBuffer[qrcodegen_BUFFER_LEN_MAX];
	int ok = qrcodegen_encodeText(uri, tempBuffer, qrcode, errCorLvl,
		12, // image size
		qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, 1);

	if(width != NULL) *width = 0;
	if (!ok) return -1;

	size = qrcodegen_getSize(qrcode);
	bmp_img_init_df(&img, size * 2, size * 2);

	if(width != NULL) *width = size * 2;

	// Draw a checkerboard pattern:
	for (y = 0; y < size; y++) {
		for (x = 0; x < size; x++) {
			if (qrcodegen_getModule(qrcode, x, y)) {
				bmp_pixel_init(&img.img_pixels[2 * y][2 * x], 0, 0, 0);
				bmp_pixel_init(&img.img_pixels[2 * y][2 * x + 1], 0, 0, 0);
				bmp_pixel_init(&img.img_pixels[2 * y + 1][2 * x], 0, 0, 0);
				bmp_pixel_init(&img.img_pixels[2 * y + 1][2 * x + 1], 0, 0, 0);

			}
			else {
				bmp_pixel_init(&img.img_pixels[2 * y][2 * x], 255, 255, 255);
				bmp_pixel_init(&img.img_pixels[2 * y][2 * x + 1], 255, 255, 255);
				bmp_pixel_init(&img.img_pixels[2 * y + 1][2 * x], 255, 255, 255);
				bmp_pixel_init(&img.img_pixels[2 * y + 1][2 * x + 1], 255, 255, 255);
			}
		}
	}

	if (filename != NULL)
		bmp_img_write(&img, filename);
	else if (buffer != NULL && bufSize != NULL)
		bmp_img_write_buffer(&img, buffer, bufSize);

	bmp_img_free(&img);
	return 0;
}
