
#ifdef __OBJC__		// this declaration only appears for files compiling with objc enabled

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/glext.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/CGLContext.h>

@interface GLString : NSObject {
	CGLContextObj cgl_ctx; // current context at time of texture creation
	GLuint texName;
	NSSize texSize;
	
	NSAttributedString * string;
	NSFont * font;
	NSColor * textColor; // default is opaque white
	NSRect border;
	uint32_t flags;
	
	BOOL requiresUpdate;
}

- (id) initWithString:(NSString *)aString withFont:(NSFont *)inFont withTextColor:(NSColor *)color inBox:(NSRect *)box withFlags:(uint32_t) inFlags;

- (void) dealloc;

- (GLuint) texName; // 0 if no texture allocated
- (NSSize) texSize; // actually size of texture generated in texels, (0, 0) if no texture allocated
- (NSColor *) textColor; // get the pre-multiplied default text color (includes alpha) string attributes could override this
- (NSRect) border; // bounds for rect
- (NSFont *)font;
- (uint32_t) flags; // get the pre-multiplied default text color (includes alpha) string attributes could override this

- (void) setFont:(NSFont *)inFont;
- (void) setColor:(NSColor *)color;
- (void) setBox:(NSRect *)box;
- (void) setFlags:(uint32_t) inFlags;

- (void) genTexture; // generates the texture without drawing texture to current context
- (void) drawWithBounds:(NSRect)bounds; // will update the texture if required due to change in settings (note context should be setup to be orthographic scaled to per pixel scale)
- (void) drawAtPoint:(NSPoint)point;

@end

#endif