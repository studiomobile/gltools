#import "Mesh.h"

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

#ifdef GLES2

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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)points.length);
    
    glDrawArrays(GL_TRIANGLES, 0, points.length / (sizeof(GLfloat) * 3));
    
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#else

- (void)draw
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, 0, points.bytes);
    glTexCoordPointer(2, GL_FLOAT, 0, uvs.bytes);
    
    if (indexes.length > 0) {
        glDrawElements(GL_TRIANGLES, indexes.length / sizeof(GLshort), GL_SHORT, indexes.bytes);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, points.length / (sizeof(GLfloat) * 3));
    }
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

#endif

- (void)dealloc
{
    [points release];
    [indexes release];
    [uvs release];
#ifdef GLES2
    [self unbind];
#endif
    [super dealloc];
}

@end
