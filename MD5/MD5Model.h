#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

struct md5_model_t;
struct md5_anim_t;
struct vec3_t;

@interface MD5Model : NSObject {
@private
    struct md5_model_t *model;
    struct md5_anim_t  *anim;

    GLfloat   *vertices;
    GLfloat   *joint_vertices;
    GLushort  *joint_indices;

    BOOL drawSkeleton;
    
    int curr_frame;
    int next_frame;
    NSTimeInterval last_time;
    NSTimeInterval max_time;
}
@property (nonatomic, readonly) NSInteger totalFrames;
@property (nonatomic, readonly) NSInteger animationFrameRate;
@property (nonatomic, assign) BOOL drawSkeleton;

- (id)initWithModelFromFile:(NSString*)filePath animFile:(NSString*)animFilePath;

- (void)render;

@end
