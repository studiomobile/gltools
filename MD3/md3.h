#ifndef __MD3_H__
#define __MD3_H__

#include "glmath.h"

#define MD3_XYZ_SCALE (1.0f / 64.0f)
#define MD3_MAX_VERTICES 4096

// MDL Header
typedef struct _md3_header_t {
    int ident;            // Magic number, "IDP3"
    int version;          // Md3 format version, should be 15

    char name[64];        // Path name
    int flags;            // ?

    int num_frames;       // Number of frames
    int num_tags;         // Number of tags
    int num_meshes;       // Number of meshes
    int num_skins;        // Number of skins

    int offset_frames;    // Offset to frame data
    int offset_tags;      // Offset to tag data
    int offset_meshes;    // Offset to meshes
    int offset_eof;       // Offset end of file
} md3_header_t;


// Mesh header
typedef struct _md3_mesh_header_t {
    int ident;            // Magic number, "IDP3"
    char name[64];        // Mesh's name
    int flags;            // ?

    int num_frames;       // Number of frames
    int num_shaders;      // Number of textures
    int num_verts;        // Number of vertices per frame
    int num_triangles;    // Number of triangles

    int offset_triangles; // Offset to triangle data
    int offset_shaders;   // Offset to skin data
    int offset_st;        // Offset to texture coords.
    int offset_xyznormal; // Offset to vertex data
    int offset_end;       // Offset to the end of the mesh
} md3_mesh_header_t;


// Frame data
typedef struct _md3_frame_t {
    vec3_t  min_bounds;   // First corner of the bbox
    vec3_t  max_bounds;   // Second corner of the bbox
    vec3_t  local_origin;
    GLfloat radius;       // Radius of bounding sphere
    char    creator[16];
} md3_frame_t;


// Tag information
typedef struct _md3_tag_t {
    char    name[64];
    vec3_t  origin;     // Position vector
    GLfloat axis[3][3]; // Orientation matrix
} md3_tag_t;


// Mesh texture
typedef struct _md3_shader_t {
    char name[64];
    int shader_index;
} md3_shader_t;


// Triangle data
typedef struct _md3_triangle_t {
    int index[3]; // Vertex indices
} md3_triangle_t;


// Packed triangle data
typedef struct _md3_packed_triangle_t {
    GLshort index[3]; // Vertex indices
} md3_ptriangle_t;


// Texture coordinates
typedef struct _md3_texCoord_t {
    float s;
    float t;
} md3_texCoord_t;


// Compressed vertex data
typedef struct _md3_compressed_vertex_t {
    short         v[3];
    unsigned char normal[2];
} md3_compressed_vertex_t;


// Uncompressed vertex data
typedef struct _md3_vertex_t {
    vec3_t pos;
    vec3_t normal;
} md3_vertex_t;


// MD3 model structure
typedef struct _md3_mesh_t {
    md3_mesh_header_t header;
    md3_shader_t     *shaders;
    md3_ptriangle_t  *triangles;
    md3_texCoord_t   *texCoords;
    md3_vertex_t     *vertices;
} md3_mesh_t;


// MD3 model structure
typedef struct _md3_model_t {
    md3_header_t header;
    md3_frame_t *frames;
    md3_tag_t   *tags;
    md3_mesh_t  *meshes;
} md3_model_t;


int  MD3ReadModel(const char *filename, md3_model_t *mdl);
void MD3FreeModel(md3_model_t *mdl);

void MD3UncompressVertex(const md3_compressed_vertex_t* cvertex, md3_vertex_t *vertex);

#endif