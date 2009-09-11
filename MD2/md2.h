#ifndef _MD2_H_INCLUDED_
#define _MD2_H_INCLUDED_

/* Vector */
typedef float vec3_t[3];

/* MD2 header */
struct md2_header_t
{
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
};

/* Texture name */
struct md2_skin_t
{
    char name[64];
};

/* Texture coords */
struct md2_texCoord_t
{
    short s;
    short t;
};

/* Triangle info */
struct md2_triangle_t
{
    unsigned short vertex[3];
    unsigned short st[3];
};

/* Compressed vertex */
struct md2_vertex_t
{
    unsigned char v[3];
    unsigned char normalIndex;
};

/* Model frame */
struct md2_frame_t
{
    vec3_t scale;
    vec3_t translate;
    char name[16];
    struct md2_vertex_t *verts;
};

/* GL command packet */
struct md2_glcmd_t
{
    float s;
    float t;
    int index;
};

/* MD2 model structure */
struct md2_model_t
{
    struct md2_header_t header;
    
    struct md2_skin_t *skins;
    struct md2_texCoord_t *texcoords;
    struct md2_triangle_t *triangles;
    struct md2_frame_t *frames;
    int *glcmds;
};


// Predefined normals for MD2 format
const vec3_t normals_table[162];


/**
 * Load an MD2 model from file.
 *
 * Note: MD2 format stores model's data in little-endian ordering.  On
 * big-endian machines, you'll have to perform proper conversions.
 */
int ReadMD2Model (const char *filename, struct md2_model_t *mdl);
    

/**
 * Free resources allocated for the model.
 */
void FreeMD2Model (struct md2_model_t *mdl);

#endif