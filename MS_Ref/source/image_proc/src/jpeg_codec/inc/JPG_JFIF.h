#ifndef JFIF_H
#define JFIF_H

#include "sci_types.h"

/* JPEG marker codes */
typedef enum {
	M_SOF0  = 0xc0,
		M_SOF1  = 0xc1,
		M_SOF2  = 0xc2,
		M_SOF3  = 0xc3,
		
		M_SOF5  = 0xc5,
		M_SOF6  = 0xc6,
		M_SOF7  = 0xc7,
		
		M_JPG   = 0xc8,
		M_SOF9  = 0xc9,
		M_SOF10 = 0xca,
		M_SOF11 = 0xcb,
		
		M_SOF13 = 0xcd,
		M_SOF14 = 0xce,
		M_SOF15 = 0xcf,
		
		M_DHT   = 0xc4,
		
		M_DAC   = 0xcc,
		
		M_RST0  = 0xd0,
		M_RST1  = 0xd1,
		M_RST2  = 0xd2,
		M_RST3  = 0xd3,
		M_RST4  = 0xd4,
		M_RST5  = 0xd5,
		M_RST6  = 0xd6,
		M_RST7  = 0xd7,
		
		M_SOI   = 0xd8,
		M_EOI   = 0xd9,
		M_SOS   = 0xda,
		M_DQT   = 0xdb,
		M_DNL   = 0xdc,
		M_DRI   = 0xdd,
		M_DHP   = 0xde,
		M_EXP   = 0xdf,
		
		M_APP0  = 0xe0,
		M_APP15 = 0xef,
		
		M_JPG0  = 0xf0,
		M_JPG13 = 0xfd,
		M_COM   = 0xfe,
		
		M_TEM   = 0x01,
		
		M_ERROR = 0x100
} JPEG_MARKER_E;

uint32 ReadJpegHead(void);
uint32 WriteJpegHead(void);
void WriteJpegTail(void);
void PutMarker(JPEG_MARKER_E mark);
 

#endif