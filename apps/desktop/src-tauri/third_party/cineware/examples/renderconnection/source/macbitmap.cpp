//
//  MacBitmap.h
//  RenderConnection
//
//  Created by Sven on 05.08.09.
//  Copyright 2009 Maxon Computer GmbH. All rights reserved.
//

#include "macbitmap.h"
#include "c4d_system.h"
#include "b3d_file.h"

using namespace cineware;

static CGColorSpaceRef GetSystemColorSpace(void);
static CGImageRef CreateCGImage(Int32 bw, Int32 bh, void *data);

MacBitmap::MacBitmap()
{
	Bits = NULL;
}

MacBitmap::~MacBitmap()
{
	Free();
}

// free the content of the bitmap
void MacBitmap::Free()
{
	MemFree(Bits);
	xres = yres = 0;
	Bits = NULL;
}

// initialize the bitmap with a given x/y resolution
bool MacBitmap::Init(long x_resolution, long y_resolution)
{
	Free();
	xres = x_resolution;
	yres = y_resolution;

	Bits = MemAlloc(xres*yres*3);
	return Bits != NULL;
}

void	MacBitmap::Clear(void)
{
	if (Bits)
		memset(Bits, 0, xres * yres * 3);
}

static long ConvFtoL(float f)
{
	f *= 256.0;
	if (f < 0.0)
		return 0;
	else if (f > 255.0)
		return 255;

	return (int)f;
}

// update the image with RGB data
void	MacBitmap::Update(void *userdata, long xpos, long ypos, long xcnt, long bpp, unsigned char *rgba_data)
{
	// a few safety checks
	if (!Bits || ypos<0 || ypos >= yres)
		return;

	UChar *adr = (UChar*)Bits + ypos * xres * 3;

	if (xpos+xcnt > xres)
		xcnt = xres-xpos;

	UChar *dst = adr+xpos*3;

	if (bpp == 4) // floating point data is streamed
	{
		float *src = (float*)rgba_data;
		while (xcnt-->0)
		{
			dst[0] = ConvFtoL(src[0]);
			dst[1] = ConvFtoL(src[1]);
			dst[2] = ConvFtoL(src[2]);
			src += 4;
			dst += 3;
		}
	}
	else
	{
		UChar *src = rgba_data;
		while (xcnt-->0)
		{
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			src += 4;
			dst += 3;
		}
	}
}

void	MacBitmap::DrawImage(CGContextRef gc, const CGRect &bounds)
{
	CGImageRef	image;
	CGRect	dr;

	if (Bits == NULL)
		return;

	dr.origin.x = 0;
	dr.origin.y = 0;
	dr.size.width = xres;
	dr.size.height = yres;

	image = (CGImageRef) CreateCGImage((Int32)xres, (Int32)yres, Bits);
	if (!image)
		return;

	CGRect	local_dr;

	CGContextSaveGState(gc);

	local_dr = dr;
	local_dr.origin.x = 0.0;
	local_dr.origin.y = 0.0;
	local_dr.origin.x = (bounds.size.width - xres)/2;
	local_dr.origin.y = (bounds.size.height - yres)/2;

	CGContextDrawImage(gc, local_dr, image);
	CGContextRestoreGState(gc);
	CGImageRelease(image);
}

static CGColorSpaceRef GetSystemColorSpace(void)
{
	static CGColorSpaceRef displayCS = NULL;
	if (displayCS == NULL)
	{
		displayCS = CGDisplayCopyColorSpace(CGMainDisplayID());
	}

	return displayCS;
}

static CGImageRef CreateCGImage(Int32 bw, Int32 bh, void *data)
{
	CGImageRef	image = NULL;
	CGColorSpaceRef	color_space = GetSystemColorSpace();

	CGDataProviderRef imageDataProvider = CGDataProviderCreateWithData(NULL, data, bh * bw * 3, NULL);
	if (imageDataProvider)
	{
		image = CGImageCreate(bw, bh, 8, 24, bw*3, color_space, kCGImageAlphaNone, imageDataProvider, NULL, FALSE, kCGRenderingIntentDefault);
		CGDataProviderRelease(imageDataProvider);						// release the data provider since the image retains it.
	}

	return image;
}
