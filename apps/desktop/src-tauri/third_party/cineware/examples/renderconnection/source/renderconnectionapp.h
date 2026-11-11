//
//  RenderConnectionApp.h
//  RenderConnection
//
//  Created by Sven on 07.08.09.
//  Copyright 2009 Maxon Computer GmbH. All rights reserved.
//

#import <Cocoa/Cocoa.h>

// the NSApplication delegate
@interface RenderConnectionApp : NSObject
{
}

- (void)	applicationDidFinishLaunching:(NSNotification *)notification;
- (void)	doNothing:(id)ignored;

@end
