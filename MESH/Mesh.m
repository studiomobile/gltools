#import "Mesh.h"
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@implementation Mesh

- (id)initWithPoints:(NSData*)aPoints indexes:(NSData*)aIndexes uvs:(NSData*)aUvs
{
    self = [super init];
    if (self) {
        points = [aPoints retain];
        indexes = [aIndexes retain];
        uvs = [aUvs retain];
    }
    return self;
}

+ (Mesh*)meshWithPoints:(NSData*)points indexes:(NSData*)indexes uvs:(NSData*)uvs {
    return [[[self alloc] initWithPoints:points indexes:indexes uvs:uvs] autorelease];
}

- (void)bind
{
    if (buffers[0]) return;

    memset(buffers, 0, sizeof(buffers));
    
    int buffersCount = indexes.length > 0 ? 2 : 1;
    glGenBuffers(buffersCount, buffers);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, points.length + uvs.length, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, points.length, points.bytes);
    glBufferSubData(GL_ARRAY_BUFFER, points.length, uvs.length, uvs.bytes);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (buffers[1])
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.length, indexes.bytes, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

- (void)unbind
{
    if (!buffers[0]) return;
    glDeleteBuffers(2, buffers);
    memset(buffers, 0, sizeof(buffers));
}

- (void)draw
{
    if (!buffers[0]) return;
    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)points.length);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);

    glDrawArrays(GL_TRIANGLES, 0, points.length / (sizeof(GLfloat) * 3));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

- (void)dealloc
{
    [points release];
    [indexes release];
    [uvs release];
    [self unbind];
    [super dealloc];
}

@end
