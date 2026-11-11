//
//  RenderView.h
//  RenderConnection
//
//  Created by Sven on 05.08.09.
//  Copyright 2009 Maxon Computer GmbH. All rights reserved.
//

#ifndef RENDERVIEW_H__
#define RENDERVIEW_H__

#import <Cocoa/Cocoa.h>

@interface RenderView : NSView
{
}

// Standard view create/free methods
- (id)		initWithFrame:(NSRect)frame;
- (void)	dealloc;

// Drawing
- (void)	drawRect:(NSRect)rect;
- (BOOL)	isOpaque;

@end

#endif // RENDERVIEW_H__
