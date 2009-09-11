#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

struct md2_model_t;

@interface MD2Model : NSObject {
@private
    struct md2_model_t *model;
    GLfloat *vertices;
    GLfloat *normals;
    GLfloat *textureCoords;
    NSInteger frameIndex;
    NSInteger preparedFrame;
}
@property (nonatomic, readonly) NSInteger totalFrames;
@property (nonatomic, assign) NSInteger frameIndex;

- (id)initWithModelFromFile:(NSString*)filePath;

- (void)render;

@end
