#import "renderview.h"
#import "macbitmap.h"

@implementation RenderView

// global variables for this example
extern NSView *global_view;
extern MacBitmap *global_bmp;

- (id)	initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
	global_view = self;
	return self;
}

- (void)	dealloc
{
	[super dealloc];
}

- (void)	drawRect:(NSRect)rect
{
	CGContextRef	gc;
	CGRect				bounds;

	gc = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];

	// fill with white rectangle in case our rendered bitmap is smaller than the view
	[[NSColor whiteColor] set];
	NSRectFill( [self bounds] );

	// centered output of the rendered bitmap
	bounds = NSRectToCGRect( [self bounds] );
	if ( global_bmp )
		global_bmp->DrawImage( gc, bounds );
}

- (BOOL)isOpaque
{
	return YES;
}

@end