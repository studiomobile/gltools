#import "MD2Model.h"
#import "md2.h"

@implementation MD2Model

@synthesize frameIndex;

- (id)initWithModelFromFile:(NSString*)filePath {
    if (![super init]) return nil;
    
    model = malloc( sizeof(struct md2_model_t) );

    BOOL loaded = ReadMD2Model([filePath cStringUsingEncoding:NSUTF8StringEncoding], model);
    if (!loaded) {
        [self release];
        return nil;
    }
    
    size_t triVerticesNum  = 3 * model->header.num_tris;
    size_t triVerticesSize = 3 * sizeof (GLfloat) * triVerticesNum;
    
    vertices      = malloc( triVerticesSize );
    normals       = malloc( triVerticesSize );
    textureCoords = malloc( triVerticesSize );
    
    for (int i = 0; i < model->header.num_tris; i++) {
        for (int j = 0; j < 3; ++j) {
            size_t unitOffset = 3*i+j;
            size_t offset = unitOffset * 2;
            struct md2_texCoord_t textCoordsCompacted = model->texcoords[model->triangles[i].st[j]];
            textureCoords[offset + 0] = (GLfloat)textCoordsCompacted.s / model->header.skinwidth;
            textureCoords[offset + 1] = (GLfloat)textCoordsCompacted.t / model->header.skinheight;
        }
    }
    
    preparedFrame = -1;
    
    return self;
}


- (NSInteger)totalFrames {
    return model->header.num_frames;
}


- (BOOL)prepareFrame:(NSInteger)newFrameIndex {
    if (newFrameIndex < 0 || newFrameIndex > self.totalFrames - 1) return NO;
    if (newFrameIndex == preparedFrame) return YES;
    
    struct md2_frame_t  *frame = &model->frames[newFrameIndex];
    struct md2_vertex_t *compactedVertex;
    
    for (int i = 0; i < model->header.num_tris; i++) {
        for (int j = 0; j < 3; ++j) {
            compactedVertex = &frame->verts[model->triangles[i].vertex[j]];
            
            size_t unitOffset = 3*i+j;
            size_t offset = unitOffset * 3;
            
            const GLfloat *norm = normals_table[compactedVertex->normalIndex];
            normals[offset + 0] = norm[0];
            normals[offset + 1] = norm[1];
            normals[offset + 2] = norm[2];
            
            vertices[offset + 0] = (frame->scale[0] * (GLfloat)compactedVertex->v[0]) + frame->translate[0];
            vertices[offset + 1] = (frame->scale[1] * (GLfloat)compactedVertex->v[1]) + frame->translate[1];
            vertices[offset + 2] = (frame->scale[2] * (GLfloat)compactedVertex->v[2]) + frame->translate[2];
        }
    }

    return YES;
}


- (void)render {
    if (![self prepareFrame:frameIndex]) return;

    // MD2 triangles are clockwise
    glFrontFace(GL_CW);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glNormalPointer(GL_FLOAT, 0, normals);
    glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);

    glDrawArrays(GL_TRIANGLES, 0, 3 * model->header.num_tris);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


- (void)dealloc {
    FreeMD2Model(model);
    free(model);
    free(vertices);
    free(normals);
    free(textureCoords);
    [super dealloc];
}


@end
