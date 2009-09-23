#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

struct _md3_model_t;

@interface MD3Model : NSObject {
@private
    struct _md3_model_t *model;
    NSInteger frameIndex;
    BOOL drawNormals;
}
@property (nonatomic, readonly) NSInteger totalFrames;
@property (nonatomic, assign) NSInteger frameIndex;
@property (nonatomic, assign) BOOL drawNormals;

- (id)initWithModelFromFile:(NSString*)filePath;

- (void)render;

@end
