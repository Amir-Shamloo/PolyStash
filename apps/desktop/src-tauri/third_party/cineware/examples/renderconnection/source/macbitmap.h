//
//  MacBitmap.h
//  RenderConnection
//
//  Created by Sven on 05.08.09.
//  Copyright 2009 Maxon Computer GmbH. All rights reserved.
//

#ifndef MACBITMAP_H__
#define MACBITMAP_H__

#include "ApplicationServices/ApplicationServices.h"

// this is a simple bitmap class used to display raw RGB image data
class MacBitmap
{
private:
	void *Bits;
	long xres, yres;

public:
	MacBitmap(void);
	~MacBitmap(void);
	void	Free(void);
	bool	Init(long x_resolution, long y_resolution);
	void	Clear(void);
	void	Update(void *userdata, long xpos, long ypos, long xcnt, long bpp, unsigned char *rgba_data);
	void	DrawImage(CGContextRef gc, const CGRect &bounds);
};

#endif // MACBITMAP_H__
