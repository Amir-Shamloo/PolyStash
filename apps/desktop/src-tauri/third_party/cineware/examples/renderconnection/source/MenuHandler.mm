//
//  MenuHandler.mm
//  RenderConnection
//
//  Created by Wilfried on 03.08.09.
//  Copyright 2009 Maxon Computer GmbH. All rights reserved.
//

#include "renderconnection_mac.h"
#import "MenuHandler.h"

using namespace cineware;

extern void CallCommand(Int32 type, const Filename &filename);
extern Bool FileSelect(Filename &name, Int32 val_title);

@implementation MenuHandler

- (IBAction)	aboutRenderConnection:(id)sender
{
	// do something
}

- (IBAction)	newDocument:(id)sender
{
	CallCommand(0, Filename());
}

- (IBAction)	openDocument:(id)sender
{
	Filename name;
	if (FileSelect(name, 1))
		CallCommand(2, name);
}

- (IBAction)	stopRendering:(id)sender
{
	CallCommand(4, Filename());
}

- (IBAction)	importMaterial:(id)sender
{
  Filename name;
  if (FileSelect(name, 1))
    CallCommand(5, name);
}

- (IBAction)	importIesLight:(id)sender
{
  Filename name;
  if (FileSelect(name, 1))
    CallCommand(6, name);
}

- (IBAction)	openLicenseManager:(id)sender
{
	CallCommand(7, nullptr);
}

- (IBAction)	signOut:(id)sender
{
	CallCommand(8, nullptr);
}

- (IBAction)	locateAndStartRenderServer:(id)sender
{
	Filename filename_executable;
	if (FileSelect(filename_executable, 0))
		CallCommand(3, filename_executable);
}

- (IBAction)	shutDownRenderServer:(id)sender
{
	CallCommand(1, NULL);
}

- (void)dealloc
{
	[super dealloc];
}

// here's the place to disable/enable menu items...
- (BOOL)	validateMenuItem:(NSMenuItem *)item
{
	if ([item action] == @selector(openDocument:))
		return YES;
	else if ([item action] == @selector(stopRendering:))
		return YES;
	else if ([item action] == @selector(importMaterial:))
		return YES;
	else if ([item action] == @selector(importIesLight:))
		return YES;
	else if ([item action] == @selector(openLicenseManager:))
		return YES;
	else if ([item action] == @selector(signOut:))
		return YES;
	else if ([item action] == @selector(newDocument:))			// disable, if not available...
		return YES;
	else if ([item action] == @selector(aboutRenderConnection:))			// disable, if not available...
		return YES;
	else if ([item action] == @selector(locateAndStartRenderServer:))			// disable, if not available...
		return YES;
	else if ([item action] == @selector(shutDownRenderServer:))			// disable, if not available...
		return YES;
	else if ([item action] == @selector(showHelp:))			// disable, if not available...
		return YES;

	return NO;
}

@end


