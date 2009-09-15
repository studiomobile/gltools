#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

struct _md5_model_t;
struct _md5_anim_t;

@interface MD5Model : NSObject {
@private
    struct _md5_model_t *model;
    struct _md5_anim_t  *anim;

    GLfloat   *verticesAndNormals;
    GLfloat   *joint_vertices;
    GLushort  *joint_indices;

    BOOL drawSkeleton;
    BOOL drawNormals;
    BOOL animate;
    
    int curr_frame;
    int next_frame;
    NSTimeInterval last_time;
    NSTimeInterval max_time;
}
@property (nonatomic, readonly) NSInteger totalFrames;
@property (nonatomic, readonly) NSInteger animationFrameRate;
@property (nonatomic, assign) BOOL drawSkeleton;
@property (nonatomic, assign) BOOL drawNormals;
@property (nonatomic, assign) BOOL animate;

- (id)initWithModelFromFile:(NSString*)filePath animFile:(NSString*)animFilePath;

- (void)render;

@end
