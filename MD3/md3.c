#include "md3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Magic number (should be 860898377)
static int     md3_ident = 'I' + ('D'<<8) + ('P'<<16) + ('3'<<24);
static int     md3_version = 15;
static vec3_t  md3_precalculated_normals[256][256];
static int     md3_normals_calculated = 0;


int MD3ReadModel(const char *filename, md3_model_t *mdl) {
    FILE *fp = fopen (filename, "rb");
    if (!fp)
    {
        fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
        return 0;
    }

    /* Read header */
    fread (&mdl->header, 1, sizeof (md3_header_t), fp);
    if ((mdl->header.ident != md3_ident) || (mdl->header.version != md3_version))
    {
        /* Error! */
        fprintf (stderr, "Error: bad version or identifier\n");
        fclose (fp);
        return 0;
    }

    /* Read frames */
    if (mdl->header.num_frames > 0) {
        CALLOC(mdl->header.num_frames, mdl->frames);
        fseek (fp, mdl->header.offset_frames, SEEK_SET);
        fread (mdl->frames, sizeof (md3_frame_t), mdl->header.num_frames, fp);
    }

    /* Read tags */
    if (mdl->header.num_tags > 0) {
        CALLOC(mdl->header.num_tags, mdl->tags);
        fseek (fp, mdl->header.offset_tags, SEEK_SET);
        fread (mdl->tags, sizeof (md3_tag_t), mdl->header.num_tags, fp);
    }

    /* Read meshes */
    if (mdl->header.num_meshes > 0) {
        CALLOCZ(mdl->header.num_meshes, mdl->meshes);
        size_t mesh_offset = mdl->header.offset_meshes;
        for (size_t i = 0; i < mdl->header.num_meshes; ++i) {
            fseek (fp, mesh_offset, SEEK_SET);

            md3_mesh_t *mesh = &mdl->meshes[i];
            fread (&mesh->header, sizeof (md3_mesh_header_t), 1, fp);

            if (mesh->header.ident != md3_ident) {
                /* Error! */
                fprintf (stderr, "Error: bad version or identifier\n");
                fclose (fp);
                return 0;
            }

            /* Memory allocations */
            CALLOC(mesh->header.num_shaders, mesh->shaders);
            CALLOC(mesh->header.num_triangles, mesh->triangles);
            CALLOC(mesh->header.num_verts, mesh->texCoords);
            CALLOC(mesh->header.num_verts * mesh->header.num_frames, mesh->vertices);


            /* Read shaders */
            fseek (fp, mesh_offset + mesh->header.offset_shaders, SEEK_SET);
            fread (mesh->shaders, sizeof (md3_shader_t), mesh->header.num_shaders, fp);


            /* Read triangles */
            md3_triangle_t *tris = NULL;
            CALLOC(mesh->header.num_triangles, tris);
            fseek (fp, mesh_offset + mesh->header.offset_triangles, SEEK_SET);
            fread (tris, sizeof (md3_triangle_t), mesh->header.num_triangles, fp);
            for (size_t i = 0; i < mesh->header.num_triangles; ++i) {
                mesh->triangles[i].index[0] = tris[i].index[0];
                mesh->triangles[i].index[1] = tris[i].index[1];
                mesh->triangles[i].index[2] = tris[i].index[2];
            }
            free (tris);


            /* Read texture coords */
            fseek (fp, mesh_offset + mesh->header.offset_st, SEEK_SET);
            fread (mesh->texCoords, sizeof (md3_texCoord_t), mesh->header.num_verts, fp);


            /* Read vertices */
            md3_compressed_vertex_t *compressed_vertices = NULL;
            CALLOC(mesh->header.num_verts * mesh->header.num_frames, compressed_vertices);
            fseek (fp, mesh_offset + mesh->header.offset_xyznormal, SEEK_SET);
            fread (compressed_vertices, sizeof (md3_compressed_vertex_t), mesh->header.num_verts * mesh->header.num_frames, fp);
            for (size_t i = 0; i < mesh->header.num_verts * mesh->header.num_frames; ++i) {
                MD3UncompressVertex(&compressed_vertices[i], &mesh->vertices[i]);
            }
            free (compressed_vertices);


            mesh_offset += mesh->header.offset_end;
        }
    }

    fclose (fp);
    return 1;
}


void MD3FreeModel(md3_model_t *mdl) {
    if (!mdl) return;

    if (mdl->frames)
    {
        free (mdl->frames);
        mdl->frames = NULL;
    }

    if (mdl->tags)
    {
        free (mdl->tags);
        mdl->tags = NULL;
    }

    if (mdl->meshes)
    {
        for (size_t i = 0; i < mdl->header.num_meshes; ++i)
        {
            md3_mesh_t *mesh = &mdl->meshes[i];
            if (mesh->shaders)   free (mesh->shaders);
            if (mesh->triangles) free (mesh->triangles);
            if (mesh->texCoords) free (mesh->texCoords);
            if (mesh->vertices)  free (mesh->vertices);
        }
        free (mdl->meshes);
        mdl->meshes = NULL;
    }
}


void MD3UncompressVertex(const md3_compressed_vertex_t* cvertex, md3_vertex_t *vertex) {
    if (!md3_normals_calculated) {
        for (size_t i = 0; i < 256; ++i) {
            for (size_t j = 0; j < 256; ++j) {
                GLfloat lng = (GLfloat)i * 2.0f * M_PI / 255.0f;
                GLfloat lat = (GLfloat)j * 2.0f * M_PI / 255.0f;
                md3_precalculated_normals[i][j][X] = cos (lat) * sin (lng);
                md3_precalculated_normals[i][j][Y] = sin (lat) * sin (lng);
                md3_precalculated_normals[i][j][Z] = cos (lng);
            }
        }
        md3_normals_calculated = 1;
    }

    vertex->pos[X] = cvertex->v[X] * MD3_XYZ_SCALE;
    vertex->pos[Y] = cvertex->v[Y] * MD3_XYZ_SCALE;
    vertex->pos[Z] = cvertex->v[Z] * MD3_XYZ_SCALE;

    VEC_COPY(vertex->normal, md3_precalculated_normals[cvertex->normal[0]][cvertex->normal[1]]);
}
