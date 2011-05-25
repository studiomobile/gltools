#import <Foundation/Foundation.h>
#import "GLES.h"

@interface Mesh : NSObject {
    NSData *points;
    NSData *indexes;
    NSData *uvs;
#ifdef GLES2
    uint32_t buffers[2];
#endif
}

+ (Mesh*)meshWithPoints:(NSData*)points indexes:(NSData*)indexes uvs:(NSData*)uvs;

#ifdef GLES2
- (void)bind;
- (void)unbind;
#endif
- (void)draw;

@end
