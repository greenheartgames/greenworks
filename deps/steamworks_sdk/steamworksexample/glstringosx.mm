#include "stdafx.h"
#include "GameEngine.h"

#import "glstringosx.h"

// GLString follows

@implementation GLString

- (void) deleteTexture
{
	if (texName && cgl_ctx) {
		(*cgl_ctx->disp.delete_textures)(cgl_ctx->rend, 1, &texName);
		texName = 0; // ensure it is zeroed for failure cases
		cgl_ctx = 0;
	}
}

- (void) dealloc
{
	[self deleteTexture];
	[textColor release];
	[string release];
	[super dealloc];
}

// designated initializer
- (id) initWithString:(NSString *)aString withFont:(NSFont *) inFont withTextColor:(NSColor *)color inBox:(NSRect *)box withFlags:(uint32_t) inFlags
{
	[super init];
	cgl_ctx = NULL;
	texName = 0;
	texSize.width = 0.0f;
	texSize.height = 0.0f;

    [color retain];
	textColor = color;
	
	[inFont retain];
	font = inFont;
	
	flags = inFlags;

	NSMutableDictionary *attribs = [NSMutableDictionary dictionary];
    [attribs setObject: font forKey: NSFontAttributeName];
    [attribs setObject: textColor forKey: NSForegroundColorAttributeName];
    
	string = [[NSAttributedString alloc] initWithString:aString attributes:attribs];

	border = *box;

	requiresUpdate = YES;
	return self;
}

- (void) setFont:(NSFont *)inFont
{
	if ( [font isEqual: inFont] )
		return;

	[string release];
	[font release];
	
	[inFont retain];
	font = inFont;
	
	NSMutableDictionary *attribs = [NSMutableDictionary dictionary];
    [attribs setObject: font forKey: NSFontAttributeName];
    [attribs setObject: textColor forKey: NSForegroundColorAttributeName];
    
	string = [[NSAttributedString alloc] initWithString:[string string] attributes:attribs];
	
	requiresUpdate = YES;
}

- (void) setColor:(NSColor *)color
{
	if ( [textColor isEqual:color] )
		return;
	
	[string release];
	[textColor release];
	
	[color retain];
	textColor = color;
	
	NSMutableDictionary *attribs = [NSMutableDictionary dictionary];
    [attribs setObject: font forKey: NSFontAttributeName];
    [attribs setObject: textColor forKey: NSForegroundColorAttributeName];
    
	string = [[NSAttributedString alloc] initWithString:[string string] attributes:attribs];
	
	requiresUpdate = YES;
}

- (void) setBox:(NSRect *)box
{
	if ( NSEqualRects(border, *box ) )
		return;
	
	border = *box;
	requiresUpdate = YES;
}

- (void) setFlags:(uint32_t) inFlags
{
	if ( inFlags == flags )
		return;
	
	flags = inFlags;
	requiresUpdate = YES;
}


// generates the texture without drawing texture to current context
- (void) genTexture
{
	NSSize previousSize = texSize;

	NSBitmapImageRep *bitmap = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL
																	   pixelsWide:border.size.width
																	   pixelsHigh:border.size.height
																	bitsPerSample:8
																  samplesPerPixel:4
																		 hasAlpha:YES
																		 isPlanar:NO
																   colorSpaceName:NSCalibratedRGBColorSpace
																	  bytesPerRow:border.size.width * 4
																	 bitsPerPixel:0];

	[textColor set];

	float x = 0.0f;
	float y = (border.size.height - [string size].height)/2;

	if ( flags & TEXTPOS_CENTER )
		x = (border.size.width - [string size].width)/2;
	else if ( flags & TEXTPOS_RIGHT )
		x = border.size.width - [string size].width;

	[NSGraphicsContext saveGraphicsState];
	NSGraphicsContext *context = [NSGraphicsContext graphicsContextWithBitmapImageRep:bitmap];
	[context setShouldAntialias:YES];
	[NSGraphicsContext setCurrentContext:context];

	[string drawAtPoint:NSMakePoint(x, y)]; // draw at offset position

	[NSGraphicsContext restoreGraphicsState];

	texSize.width = [bitmap pixelsWide];
	texSize.height = [bitmap pixelsHigh];

	if ( (cgl_ctx = CGLGetCurrentContext () ) )
	{ // if we successfully retrieve a current context (required)
		glPushAttrib(GL_TEXTURE_BIT);
		if (0 == texName) glGenTextures (1, &texName);
		glBindTexture (GL_TEXTURE_RECTANGLE_EXT, texName);
		if (NSEqualSizes(previousSize, texSize)) {
			glTexSubImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, 0, 0, texSize.width, texSize.height, [bitmap hasAlpha] ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
		} else {
			glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, texSize.width, texSize.height, 0, [bitmap hasAlpha] ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
		}
		glPopAttrib();
	}
	else
	{
		NSLog (@"-genTexture: Failure to get current OpenGL context\n");
	
	}
	[bitmap release];

	requiresUpdate = NO;
}

- (GLuint) texName
{
	return texName;
}

- (NSSize) texSize
{
	return texSize;
}

- (void) setTextColor:(NSColor *)color // set default text color
{
	[color retain];
	[textColor release];
	textColor = color;
	requiresUpdate = YES;
}

- (NSColor *) textColor
{
	return textColor;
}

- (NSRect) border
{
	return border;
}

- (NSFont *)font
{
	return font;
}

- (uint32_t) flags
{
	return flags;
}

- (void) drawWithBounds:(NSRect)bounds
{
	if (requiresUpdate)
		[self genTexture];
	if (texName) 
    {
		glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT); // GL_COLOR_BUFFER_BIT for glBlendFunc, GL_ENABLE_BIT for glEnable / glDisable
		
		glDisable (GL_DEPTH_TEST); // ensure text is not remove by depth buffer test.
		glEnable (GL_BLEND); // for text fading
		glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // ditto
		glEnable (GL_TEXTURE_RECTANGLE_EXT);	
		
		glBindTexture (GL_TEXTURE_RECTANGLE_EXT, texName);
		glBegin (GL_QUADS);
			glTexCoord2f (0.0f, 0.0f); // draw upper left in world coordinates
			glVertex2f (bounds.origin.x, bounds.origin.y);
	
			glTexCoord2f (0.0f, texSize.height); // draw lower left in world coordinates
			glVertex2f (bounds.origin.x, bounds.origin.y + bounds.size.height);
	
			glTexCoord2f (texSize.width, texSize.height); // draw upper right in world coordinates
			glVertex2f (bounds.origin.x + bounds.size.width, bounds.origin.y + bounds.size.height);
	
			glTexCoord2f (texSize.width, 0.0f); // draw lower right in world coordinates
			glVertex2f (bounds.origin.x + bounds.size.width, bounds.origin.y);
		glEnd ();
		
		glPopAttrib();
	}
}

- (void) drawAtPoint:(NSPoint)point
{
	if (requiresUpdate)
		[self genTexture]; // ensure size is calculated for bounds
	if (texName) // if successful
		[self drawWithBounds:NSMakeRect (point.x, point.y, texSize.width, texSize.height)];
}

@end
