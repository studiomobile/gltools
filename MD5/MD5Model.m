#import "MD5Model.h"
#import "md5.h"

@implementation MD5Model

@synthesize drawSkeleton;
@synthesize drawNormals;
@synthesize animate;

- (id)initWithModelFromFile:(NSString*)filePath animFile:(NSString*)animFilePath {
    if (![super init]) return nil;
    
    MALLOCZ(model);
    BOOL loaded = MD5ReadModel([filePath cStringUsingEncoding:NSUTF8StringEncoding], model);
    if (!loaded) {
        [self release];
        return nil;
    }
    
    MALLOCZ(anim);
    loaded = MD5ReadAnim([animFilePath cStringUsingEncoding:NSUTF8StringEncoding], anim);
    if (!loaded) {
        [self release];
        return nil;
    }
    
    BOOL valid = MD5CheckAnimValidity(model, anim);
    if (!valid) {
        [self release];
        return nil;
    }
    
    size_t max_verts = 0;
    for (int i = 0; i < model->num_meshes; ++i) {
        size_t mesh_verts = model->meshes[i].num_verts;
        max_verts = MAX(mesh_verts, max_verts);
    }
    
    CALLOC(6 * max_verts, verticesAndNormals); // x, y, z per vertex and same per normal
    CALLOC(3 * model->num_joints, joint_vertices); // x, y, z per join position
    CALLOC(2 * model->num_joints, joint_indices); // 2 vertices per each bone
    
    max_time = 1.0 / anim->frameRate;
    
    return self;
}


- (void)dealloc {
    if (model) {
        MD5FreeModel(model);
        free(model);
    }
    if (anim) {
        MD5FreeAnim(anim);
        free(anim);
    }
    free(verticesAndNormals);
    free(joint_vertices);
    free(joint_indices);
    [super dealloc];
}


- (NSInteger)totalFrames {
    return anim->num_frames;
}


- (NSInteger)animationFrameRate {
    return anim->frameRate;
}


- (void)animate:(double)dt {
    int maxFrameIndex = anim->num_frames - 1;
    last_time += dt;
    
    /* move to next frame */
    if (last_time >= max_time) {
        curr_frame++;
        next_frame++;
        last_time = 0.0;
        
        if (curr_frame > maxFrameIndex)
            curr_frame = 0;
        
        if (next_frame > maxFrameIndex)
            next_frame = 0;
    }
}


- (void)drawSkeleton:(const md5_joint_t *)skeleton {
    vec3_t *verts = (vec3_t *)joint_vertices;
    for (size_t i = 0; i < model->num_joints; ++i) {
        VEC_COPY(verts[i], skeleton[i].pos);
    }
    glVertexPointer(3, GL_FLOAT, 0, joint_vertices);
    
    size_t offset = 0;
    for (size_t i = 0; i < model->num_joints; ++i) {
        if (skeleton[i].parent != -1) {
            joint_indices[offset + 0] = skeleton[i].parent;
            joint_indices[offset + 1] = i;
            offset += 2;
        }
    }

    /* Draw each joint */
    glPointSize( 5.0f );
    glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
    glDrawArrays(GL_POINTS, 0, model->num_joints);

    /* Draw each bone */
    glPointSize( 1.0f );
    glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
    glDrawElements(GL_LINES, offset, GL_UNSIGNED_SHORT, joint_indices);
}


- (void)render {
    static NSTimeInterval current_time = 0;

    last_time = current_time;
    current_time = [[NSDate date] timeIntervalSince1970];
    if (last_time == 0) {
        last_time = current_time;
    }

    md5_joint_t *skeleton = model->skel;

    if (animate)
    {
        /* Calculate current and next frames */
        [self animate:current_time - last_time];

        /* Interpolate skeletons between two frames */
        MD5InterpolateSkeletons (anim->skel_frames[curr_frame],
                              anim->skel_frames[next_frame],
                              anim->num_joints,
                              last_time * anim->frameRate,
                              skeleton);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    /* Draw each mesh of the model */
    for (size_t i = 0; i < model->num_meshes; ++i) {
        md5_mesh_t *mesh = &model->meshes[i];
        MD5CalculateVerticesAndNormals(mesh, skeleton, (vec3_t*)verticesAndNormals);
        glVertexPointer(3, GL_FLOAT, 6 * sizeof(verticesAndNormals[0]), verticesAndNormals);
        glNormalPointer(GL_FLOAT, 6 * sizeof(verticesAndNormals[0]), verticesAndNormals + 3 * sizeof(verticesAndNormals[0]));
        glTexCoordPointer(2, GL_FLOAT, 0, mesh->uvs);
        glDrawElements(GL_TRIANGLES, mesh->num_tris * 3, GL_UNSIGNED_SHORT, mesh->triangles);

        if (drawNormals) {
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glVertexPointer(3, GL_FLOAT, 0, verticesAndNormals);

            vec3_t *vn = (vec3_t *)verticesAndNormals;

            for (size_t i = 0, offset = 0; i < mesh->num_verts; ++i, offset += 2) {
                VEC_ADD(vn[offset + 1], vn[offset]);
            }

            glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
            glDrawArrays(GL_POINTS, 0, 2 * mesh->num_verts);

            glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
            glVertexPointer(3, GL_FLOAT, 0, verticesAndNormals);
            glDrawArrays(GL_LINES, 0, 2 * mesh->num_verts);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
        }
    }

    if (drawSkeleton) {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        [self drawSkeleton:skeleton];
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


@end
