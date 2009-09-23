#import "MD2Model.h"
#import "md2.h"

@implementation MD2Model

@synthesize frameIndex;

- (id)initWithModelFromFile:(NSString*)filePath {
    if (![super init]) return nil;
    
    MALLOCZ(model);
    BOOL loaded = MD2ReadModel([filePath cStringUsingEncoding:NSUTF8StringEncoding], model);
    if (!loaded) {
        [self release];
        return nil;
    }
    
    size_t num_vertices  = 3 * model->header.num_tris;

    CALLOC(3 * num_vertices, vertices); // x, y, z per vertex
    CALLOC(3 * num_vertices, normals); // x, y, z per normal
    CALLOC(2 * num_vertices, textureCoords); // u, v per vertex
    
    for (size_t i = 0, offset = 0; i < model->header.num_tris; ++i) {
        for (int j = 0; j < 3; ++j, offset += 2) {
            md2_texCoord_t *textCoordsCompacted = &model->texcoords[model->triangles[i].st[j]];
            textureCoords[offset + 0] = (GLfloat)textCoordsCompacted->s / model->header.skinwidth;
            textureCoords[offset + 1] = (GLfloat)textCoordsCompacted->t / model->header.skinheight;
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
    
    md2_frame_t *frame = &model->frames[newFrameIndex];
    
    for (size_t i = 0, offset = 0; i < model->header.num_tris; ++i) {
        md2_triangle_t *tri = &model->triangles[i];
        for (size_t j = 0; j < 3; ++j, offset += 3) {
            md2_vertex_t *vertex = &frame->verts[tri->vertex[j]];
            
            const GLfloat *norm = normals_table[vertex->normalIndex];
            normals[offset + 0] = norm[0];
            normals[offset + 1] = norm[1];
            normals[offset + 2] = norm[2];
            
            vertices[offset + 0] = (frame->scale[0] * (GLfloat)vertex->v[0]) + frame->translate[0];
            vertices[offset + 1] = (frame->scale[1] * (GLfloat)vertex->v[1]) + frame->translate[1];
            vertices[offset + 2] = (frame->scale[2] * (GLfloat)vertex->v[2]) + frame->translate[2];
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


- (void)renderNormals {
    if (![self prepareFrame:frameIndex]) return;

    glColor4f(1.0f, 0.0, 0.0f, 1.0f);
    glPointSize(2.0f);
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_POINTS, 0, model->header.num_vertices);

    glDisableClientState(GL_VERTEX_ARRAY);
}


- (void)dealloc {
    MD2FreeModel(model);
    free(model);
    free(vertices);
    free(normals);
    free(textureCoords);
    [super dealloc];
}


@end
