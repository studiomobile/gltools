#ifndef _MD2_H_INCLUDED_
#define _MD2_H_INCLUDED_

#include "glmath.h"


/* MD2 header */
typedef struct _md2_header_t {
    int ident;
    int version;
    
    int skinwidth;
    int skinheight;
    
    int framesize;
    
    int num_skins;
    int num_vertices;
    int num_st;
    int num_tris;
    int num_glcmds;
    int num_frames;
    
    int offset_skins;
    int offset_st;
    int offset_tris;
    int offset_frames;
    int offset_glcmds;
    int offset_end;
} md2_header_t;


/* Texture name */
typedef struct _md2_skin_t {
    char name[64];
} md2_skin_t;


/* Texture coords */
typedef struct _md2_texCoord_t {
    short s;
    short t;
} md2_texCoord_t;


/* Triangle info */
typedef struct _md2_triangle_t {
    unsigned short vertex[3];
    unsigned short st[3];
} md2_triangle_t;


/* Compressed vertex */
typedef struct _md2_vertex_t {
    unsigned char v[3];
    unsigned char normalIndex;
} md2_vertex_t;


/* Model frame */
typedef struct _md2_frame_t {
    vec3_t scale;
    vec3_t translate;
    char name[16];
    md2_vertex_t *verts;
} md2_frame_t;


/* GL command packet */
typedef struct _md2_glcmd_t {
    float s;
    float t;
    int index;
} md2_glcmd_t;


/* MD2 model structure */
typedef struct _md2_model_t {
    md2_header_t header;
    md2_skin_t *skins;
    md2_texCoord_t *texcoords;
    md2_triangle_t *triangles;
    md2_frame_t *frames;
    int *glcmds;
} md2_model_t;


// Predefined normals for MD2 format
const vec3_t normals_table[162];


/**
 * Load an MD2 model from file.
 *
 * Note: MD2 format stores model's data in little-endian ordering.  On
 * big-endian machines, you'll have to perform proper conversions.
 */
int  MD2ReadModel(const char *filename, md2_model_t *mdl);
void MD2FreeModel(md2_model_t *mdl);

#endif