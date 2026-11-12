//
//  RenderConnectionApp.mm
//  RenderConnection
//
//  Created by Sven on 07.08.09.
//  Copyright 2009 Maxon Computer GmbH. All rights reserved.
//

#import "renderconnectionapp.h"

// the NSApplication delegate
@implementation RenderConnectionApp

// delegate method
- (void)	applicationDidFinishLaunching:(NSNotification *)notification
{
	if ( [NSThread isMultiThreaded] == NO )
		[NSThread detachNewThreadSelector: @selector(doNothing:) toTarget:self withObject:NULL];	// start a detached dummy thread to make sure that Cocoa is in multithreaded mode
}

// a dummy thread method to make sure Cocoa goes into multithreaded mode (see applicationDidFinishLaunching)
- (void)	doNothing:(id)ignored
{
}

@end
