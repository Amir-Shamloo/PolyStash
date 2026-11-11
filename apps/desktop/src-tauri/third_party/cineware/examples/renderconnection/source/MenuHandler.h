//
//  MenuHandler.h
//  RenderConnection
//
//  Created by Wilfried on 03.08.09.
//  Copyright 2009 Maxon Computer GmbH. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class	NSMenuItem;

@interface MenuHandler : NSObject
{

}

- (IBAction)	aboutRenderConnection:(id)sender;
- (IBAction)	newDocument:(id)sender;
- (IBAction)	openDocument:(id)sender;
- (IBAction)	stopRendering:(id)sender;
- (IBAction)	importMaterial:(id)sender;
- (IBAction)	importIesLight:(id)sender;
- (IBAction)	openLicenseManager:(id)sender;
- (IBAction)	signOut:(id)sender;
- (IBAction)	locateAndStartRenderServer:(id)sender;
- (IBAction)	shutDownRenderServer:(id)sender;
- (BOOL)	validateMenuItem:(NSMenuItem *)item;

@end
