#import "MD3Model.h"
#import "md3.h"

@implementation MD3Model

@synthesize frameIndex;
@synthesize drawNormals;

- (id)initWithModelFromFile:(NSString*)filePath {
    if (![super init]) return nil;

    MALLOCZ(model);
    BOOL loaded = MD3ReadModel([filePath cStringUsingEncoding:NSUTF8StringEncoding], model);
    if (!loaded) {
        [self release];
        return nil;
    }

    return self;
}


- (NSInteger)totalFrames {
    return model->header.num_frames;
}


- (void)renderNormalsForMesh:(md3_mesh_t*)mesh {
    size_t points = 2 * mesh->header.num_verts;
    vec3_t *lines = NULL;
    CALLOC(points, lines);
    memcpy(lines, mesh->vertices + mesh->header.num_verts * frameIndex, mesh->header.num_verts * sizeof(mesh->vertices[0]));

    for (size_t i = 0; i < points; i += 2) {
        VEC_ADD(lines[i+1], lines[i]);
    }

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, lines);
    glDrawArrays(GL_LINES, 0, mesh->header.num_verts * 2);

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    free(lines);
}


- (void)render {
    if (frameIndex < 0 || frameIndex >= self.totalFrames) return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    md3_frame_t *frame = &model->frames[frameIndex];
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(frame->local_origin[X], frame->local_origin[Y], frame->local_origin[Z]);

    for (size_t i = 0; i < model->header.num_meshes; ++i) {
        md3_mesh_t *mesh = &model->meshes[i];
        size_t frame_offset = frameIndex * mesh->header.num_verts;

        glVertexPointer(3, GL_FLOAT, sizeof(mesh->vertices[0]), &mesh->vertices[frame_offset]);
        glNormalPointer(GL_FLOAT, sizeof(mesh->vertices[0]), (char*)&mesh->vertices[frame_offset] + offsetof(md3_vertex_t, normal));
        glTexCoordPointer(2, GL_FLOAT, 0, mesh->texCoords);

        glDrawElements(GL_TRIANGLES, mesh->header.num_triangles * 3, GL_UNSIGNED_SHORT, mesh->triangles);

        if (drawNormals) {
            [self renderNormalsForMesh:mesh];
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


- (void)dealloc {
    MD3FreeModel(model);
    free(model);
    [super dealloc];
}

@end
