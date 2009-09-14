/*
 * md5model.h -- md5mesh model loader + animation
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __MD5MODEL_H__
#define __MD5MODEL_H__

#include "gl.h"
#include "glmath.h"

#define CALLOC(count, collection_ptr) collection_ptr = calloc(count, sizeof(collection_ptr[0]))
#define CALLOCZ(count, collection_ptr) collection_ptr = calloc(count, sizeof(collection_ptr[0])); memset(collection_ptr, 0, count * sizeof(collection_ptr[0]))

typedef struct _md5_joint_t
{
    int parent;
    vec3_t pos;
    quat4_t orient;
    char name[64];
} md5_joint_t;


typedef struct _md5_vertex_t
{
    int start; /* start weight */
    int count; /* weight count */
} md5_vertex_t;


typedef struct _md5_triangle_t
{
    GLushort index[3];
} md5_triangle_t;


typedef struct _md5_weight_t
{
    int joint;
    GLfloat bias;
    vec3_t pos;
    vec3_t normal;
} md5_weight_t;


typedef struct _md5_bbox_t
{
    vec3_t min;
    vec3_t max;
} md5_bbox_t;


typedef struct _md5_mesh_t
{
    md5_vertex_t   *vertices;
    md5_triangle_t *triangles;
    md5_weight_t   *weights;
    vec2_t *uvs;

    int num_verts;
    int num_tris;
    int num_weights;

    char shader[256];
} md5_mesh_t;


typedef struct _md5_model_t
{
    md5_joint_t *skel;
    int num_joints;
    md5_mesh_t *meshes;
    int num_meshes;
} md5_model_t;


typedef struct _md5_anim_t
{
    md5_joint_t **skel_frames;
    int num_joints;
    md5_bbox_t *bboxes;
    int num_frames;
    int frameRate;
} md5_anim_t;


int  MD5ReadModel(const char *filename, md5_model_t *mdl);
void MD5FreeModel(md5_model_t *mdl);
void MD5CalculateVerticesAndNormals(const md5_mesh_t *mesh, md5_joint_t *skel, vec3_t *verticesAndNormals);


int  MD5ReadAnim(const char *filename, md5_anim_t *anim);
void MD5FreeAnim(md5_anim_t *anim);
int  MD5CheckAnimValidity(const md5_model_t *mdl, const md5_anim_t *anim);
void MD5InterpolateSkeletons(const md5_joint_t *skelA, const md5_joint_t *skelB, int num_joints, float interp, md5_joint_t *out);

#endif /* __MD5MODEL_H__ */
