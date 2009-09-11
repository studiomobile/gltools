#import "MD5Model.h"
#import "md5.h"

@implementation MD5Model

@synthesize drawSkeleton;

- (id)initWithModelFromFile:(NSString*)filePath animFile:(NSString*)animFilePath {
    if (![super init]) return nil;
    
    model = malloc( sizeof(struct md5_model_t) );
    
    BOOL loaded = ReadMD5Model([filePath cStringUsingEncoding:NSUTF8StringEncoding], model);
    if (!loaded) {
        [self release];
        return nil;
    }
    
    anim = malloc( sizeof(struct md5_anim_t) );
    loaded = ReadMD5Anim([animFilePath cStringUsingEncoding:NSUTF8StringEncoding], anim);
    if (!loaded) {
        [self release];
        return nil;
    }
    
    BOOL valid = CheckAnimValidity(model, anim);
    if (!valid) {
        [self release];
        return nil;
    }
    
    size_t max_verts = 0;
    size_t max_tris = 0;

    for (int i = 0; i < model->num_meshes; ++i) {
        size_t mesh_verts = model->meshes[i].num_verts;
        max_verts = MAX(mesh_verts, max_verts);
        size_t mesh_tris = model->meshes[i].num_tris;
        max_tris = MAX(mesh_tris, max_tris);
    }
    
    vertices       = malloc (sizeof(vertices[0]) * 3 * max_verts); // x, y, z per vertex
    joint_vertices = malloc (sizeof(joint_vertices[0]) * 3 * model->num_joints); // x, y, z per join position
    joint_indices  = malloc (sizeof(joint_indices[0])  * 2 * model->num_joints); // 2 vertices per each bone
    
    drawSkeleton = YES;

    max_time = 1.0 / anim->frameRate;
    
    return self;
}


- (void)dealloc {
    if (model) {
        FreeModel(model);
        free(model);
    }
    if (anim) {
        FreeAnim(anim);
        free(anim);
    }
    free(vertices);
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


- (void)prepareMesh:(const struct md5_mesh_t *)mesh skeleton:(const struct md5_joint_t *)skeleton {
    /* Setup vertices */
    for (size_t i = 0, offset = 0, uv_offset = 0; i < mesh->num_verts; ++i, offset += 3, uv_offset += 2) {
        vec3_t finalVertex = { 0.0f, 0.0f, 0.0f };
        
        /* Calculate final vertex to draw with weights */
        for (size_t j = 0; j < mesh->vertices[i].count; ++j)
        {
            const struct md5_weight_t *weight = &mesh->weights[mesh->vertices[i].start + j];
            const struct md5_joint_t *joint   = &skeleton[weight->joint];
            
            /* Calculate transformed vertex for this weight */
            vec3_t wv;
            Quat_rotatePoint (joint->orient, weight->pos, wv);
            
            /* The sum of all weight->bias should be 1.0 */
            finalVertex[0] += (joint->pos[0] + wv[0]) * weight->bias;
            finalVertex[1] += (joint->pos[1] + wv[1]) * weight->bias;
            finalVertex[2] += (joint->pos[2] + wv[2]) * weight->bias;
        }
        
        vertices[offset + 0] = finalVertex[0];
        vertices[offset + 1] = finalVertex[1];
        vertices[offset + 2] = finalVertex[2];
    }
}


- (void)drawSkeleton:(const struct md5_joint_t *)skeleton {
    for (size_t i = 0, offset = 0; i < model->num_joints; ++i, offset += 3) {
        joint_vertices[offset + 0] = skeleton[i].pos[0];
        joint_vertices[offset + 1] = skeleton[i].pos[1];
        joint_vertices[offset + 2] = skeleton[i].pos[2];
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
    glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
    glDrawArrays(GL_POINTS, 0, model->num_joints);

    /* Draw each bone */
    glPointSize( 1.0f );
    glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
    glDrawElements(GL_LINES, offset, GL_UNSIGNED_SHORT, joint_indices);
}


- (void)render {
    struct md5_joint_t *skeleton = model->baseSkel;
    static NSTimeInterval current_time = 0;

    last_time = current_time;
    current_time = [[NSDate date] timeIntervalSince1970];
    if (last_time == 0) {
        last_time = current_time;
    }
    
    if (YES)
    {
        /* Calculate current and next frames */
        [self animate:current_time - last_time];
        
        /* Interpolate skeletons between two frames */
        InterpolateSkeletons (anim->skelFrames[curr_frame],
                              anim->skelFrames[next_frame],
                              anim->num_joints,
                              last_time * anim->frameRate,
                              skeleton);
    }
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    /* Draw each mesh of the model */
    for (size_t i = 0; i < model->num_meshes; ++i) {
        struct md5_mesh_t *mesh = &model->meshes[i];
        [self prepareMesh:mesh skeleton:skeleton];
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, mesh->uvs);
        glDrawElements(GL_TRIANGLES, mesh->num_tris * 3, GL_UNSIGNED_SHORT, mesh->triangles);
    }
    
    if (drawSkeleton) {
        glDisable(GL_DEPTH_TEST);
        [self drawSkeleton:skeleton];
        glEnable(GL_DEPTH_TEST);
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


@end
