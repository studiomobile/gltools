#ifndef __MD5_H__
#define __MD5_H__

#include "gl.h"
#include "glmath.h"


typedef struct _md5_joint_t {
    int parent;
    vec3_t pos;
    quat4_t orient;
    char name[64];
} md5_joint_t;


typedef struct _md5_vertex_t {
    int start; /* start weight */
    int count; /* weight count */
} md5_vertex_t;


typedef struct _md5_triangle_t {
    unsigned short index[3];
} md5_triangle_t;


typedef struct _md5_weight_t {
    int joint;
    GLfloat bias;
    vec3_t pos;
    vec3_t normal;
} md5_weight_t;


typedef struct _md5_bbox_t {
    vec3_t min;
    vec3_t max;
} md5_bbox_t;


typedef struct _md5_mesh_t {
    md5_vertex_t   *vertices;
    md5_triangle_t *triangles;
    md5_weight_t   *weights;
    vec2_t *uvs;

    int num_verts;
    int num_tris;
    int num_weights;

    char shader[256];
} md5_mesh_t;


typedef struct _md5_model_t {
    md5_joint_t *skel;
    int num_joints;
    md5_mesh_t *meshes;
    int num_meshes;
} md5_model_t;


typedef struct _md5_anim_t {
    md5_joint_t **skel_frames;
    int num_joints;
    md5_bbox_t *bboxes;
    int num_frames;
    int frameRate;
} md5_anim_t;


int  MD5ReadModel(const char *filename, md5_model_t *mdl);
void MD5FreeModel(md5_model_t *mdl);
void MD5CalculateVerticesAndNormals(const md5_mesh_t *mesh, const md5_joint_t *skel, vec3_t *verticesAndNormals);


int  MD5ReadAnim(const char *filename, md5_anim_t *anim);
void MD5FreeAnim(md5_anim_t *anim);
int  MD5CheckAnimValidity(const md5_model_t *mdl, const md5_anim_t *anim);
void MD5InterpolateSkeletons(const md5_joint_t *skelA, const md5_joint_t *skelB, int num_joints, float interp, md5_joint_t *out);

#endif
