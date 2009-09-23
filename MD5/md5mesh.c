#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"

void MD5ComputeWightNormals(md5_model_t *mdl);


int MD5ReadModel(const char *filename, md5_model_t *mdl) {
    FILE *fp;
    char buff[512];
    int version;
    int curr_mesh = 0;
    int max_verts = 0;
    int max_tris = 0;
    
    fp = fopen (filename, "rb");
    if (!fp) {
        fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
        return 0;
    }

    while (!feof (fp)) {
        /* Read whole line */
        fgets (buff, sizeof (buff), fp);

        if (sscanf (buff, " MD5Version %d", &version) == 1) {
            if (version != 10) {
                /* Bad version */
                fprintf (stderr, "Error: bad model version\n");
                fclose (fp);
                return 0;
            }
        } else if (sscanf (buff, " numJoints %d", &mdl->num_joints) == 1) {
            if (mdl->num_joints > 0) {
                CALLOC(mdl->num_joints, mdl->skel);
            }
        } else if (sscanf (buff, " numMeshes %d", &mdl->num_meshes) == 1) {
            if (mdl->num_meshes > 0) {
                CALLOC(mdl->num_meshes, mdl->meshes);
            }
        } else if (strncmp (buff, "joints {", 8) == 0) {
            /* Read each joint */
            for (int i = 0; i < mdl->num_joints; ++i) {
                md5_joint_t *joint = &mdl->skel[i];

                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                if (sscanf (buff, "%s %d ( %f %f %f ) ( %f %f %f )", joint->name, &joint->parent,
                            &joint->pos[X], &joint->pos[Y], &joint->pos[Z],
                            &joint->orient[X], &joint->orient[Y], &joint->orient[Z]) == 8) {
                    /* Compute the w component */
                    Quat_computeW (joint->orient);
                }
            }
        } else if (strncmp (buff, "mesh {", 6) == 0) {
            md5_mesh_t *mesh = &mdl->meshes[curr_mesh];
            int vert_index = 0;
            int tri_index = 0;
            int weight_index = 0;
            float fdata[4];
            int idata[3];

            while ((buff[0] != '}') && !feof (fp)) {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                if (strstr (buff, "shader ")) {
                    int quote = 0, j = 0;
                    /* Copy the shader name whithout the quote marks */
                    for (int i = 0; i < sizeof (buff) && (quote < 2); ++i) {
                        if (buff[i] == '\"') quote++;

                        if ((quote == 1) && (buff[i] != '\"')) {
                            mesh->shader[j] = buff[i];
                            j++;
                        }
                    }
                } 
                else if (sscanf (buff, " numverts %d", &mesh->num_verts) == 1) {
                    if (mesh->num_verts > 0) {
                        /* Allocate memory for vertices */
                        CALLOC(mesh->num_verts, mesh->vertices);
                        CALLOC(mesh->num_verts, mesh->uvs);
                    }

                    if (mesh->num_verts > max_verts) {
                        max_verts = mesh->num_verts;
                    }
                }
                else if (sscanf (buff, " numtris %d", &mesh->num_tris) == 1) {
                    if (mesh->num_tris > 0) {
                        /* Allocate memory for triangles */
                        CALLOC(mesh->num_tris, mesh->triangles);
                    }

                    if (mesh->num_tris > max_tris) {
                        max_tris = mesh->num_tris;
                    }
                }
                else if (sscanf (buff, " numweights %d", &mesh->num_weights) == 1) {
                    if (mesh->num_weights > 0) {
                        /* Allocate memory for vertex weights */
                        CALLOC(mesh->num_weights, mesh->weights);
                    }
                }
                else if (sscanf (buff, " vert %d ( %f %f ) %d %d", &vert_index, &fdata[0], &fdata[1], &idata[0], &idata[1]) == 5) {
                    /* Copy vertex data */
                    mesh->uvs[vert_index][U] = fdata[0];
                    mesh->uvs[vert_index][V] = fdata[1];
                    mesh->vertices[vert_index].start = idata[0];
                    mesh->vertices[vert_index].count = idata[1];
                } 
                else if (sscanf (buff, " tri %d %d %d %d", &tri_index, &idata[0], &idata[1], &idata[2]) == 4) {
                    /* Copy triangle data */
                    mesh->triangles[tri_index ].index[X] = idata[0];
                    mesh->triangles[tri_index ].index[Y] = idata[1];
                    mesh->triangles[tri_index ].index[Z] = idata[2];
                }
                else if (sscanf (buff, " weight %d %d %f ( %f %f %f )", &weight_index, &idata[0], &fdata[3], &fdata[0], &fdata[1], &fdata[2]) == 6) {
                    /* Copy vertex data */
                    mesh->weights[weight_index].joint  = idata[0];
                    mesh->weights[weight_index].bias   = fdata[3];
                    mesh->weights[weight_index].pos[X] = fdata[0];
                    mesh->weights[weight_index].pos[Y] = fdata[1];
                    mesh->weights[weight_index].pos[Z] = fdata[2];
                }
            }

            curr_mesh++;
        }
    }

    fclose (fp);
    
    MD5ComputeWightNormals(mdl);

    return 1;
}


void MD5ComputeWightNormals(md5_model_t *mdl) {
    md5_mesh_t *mesh;
    vec3_t *bindpose_verts;
    vec3_t *bindpose_norms;
    vec3_t norm;
    quat4_t rot;
    
    for (int m = 0; m < mdl->num_meshes; ++m) {
        mesh = &mdl->meshes[m];
        CALLOC(mesh->num_verts, bindpose_verts);
        CALLOC(mesh->num_verts, bindpose_norms);
        
        for (size_t i = 0; i < mesh->num_verts; ++i) {
            vec3_t finalVertex = { 0.0f, 0.0f, 0.0f };

            /* Calculate final vertex to draw with weights */
            for (size_t j = 0; j < mesh->vertices[i].count; ++j) {
                const md5_weight_t *weight = &mesh->weights[mesh->vertices[i].start + j];
                const md5_joint_t *joint   = &mdl->skel[weight->joint];
                
                /* Calculate transformed vertex for this weight */
                vec3_t wv;
                Quat_rotatePoint(joint->orient, weight->pos, wv);
                
                /* The sum of all weight->bias should be 1.0 */
                finalVertex[X] += (joint->pos[X] + wv[X]) * weight->bias;
                finalVertex[Y] += (joint->pos[Y] + wv[Y]) * weight->bias;
                finalVertex[Z] += (joint->pos[Z] + wv[Z]) * weight->bias;
            }
            
            VEC_COPY(bindpose_verts[i], finalVertex);
            VEC_ZERO(bindpose_norms[i]);
        }
        
        for (int t = 0; t < mesh->num_tris; ++t) {
            md5_triangle_t *tri = &mesh->triangles[t];
            Vec_computeNormal(bindpose_verts[tri->index[0]], bindpose_verts[tri->index[1]], bindpose_verts[tri->index[2]], norm);
            for (int j = 0; j < 3; ++j) {
                VEC_ADD(bindpose_norms[tri->index[j]], norm);
            }
        }
        
        for (int n = 0; n < mesh->num_verts; ++n) {
            VEC_NEG(bindpose_norms[n]);
            Vec_normalize(bindpose_norms[n]);
        }
        
        for (int w = 0; w < mesh->num_weights; ++w) {
            VEC_ZERO(mesh->weights[w].normal);
        }
        
        for (int i = 0; i < mesh->num_verts; ++i) {
            for (int w = 0; w < mesh->vertices[i].count; ++w) {
                md5_weight_t *weight = &mesh->weights[mesh->vertices[i].start + w];
                Quat_inverse(mdl->skel[weight->joint].orient, rot);
                Quat_rotatePoint(rot, bindpose_norms[i], norm);
                VEC_ADD(weight->normal, norm);
            }
        }
        
        for (int w = 0; w < mesh->num_weights; ++w) {
            Vec_normalize(mesh->weights[w].normal);
        }
        
        free(bindpose_verts);
        free(bindpose_norms);
    }
}


void MD5FreeModel(md5_model_t *mdl) {
    if (!mdl) return;

    if (mdl->skel) {
        free (mdl->skel);
        mdl->skel = NULL;
    }

    if (mdl->meshes) {
        /* Free mesh data */
        for (int i = 0; i < mdl->num_meshes; ++i) {
            if (mdl->meshes[i].vertices) {
                free (mdl->meshes[i].vertices);
                mdl->meshes[i].vertices = NULL;
            }
            if (mdl->meshes[i].uvs) {
                free (mdl->meshes[i].uvs);
                mdl->meshes[i].uvs = NULL;
            }
            if (mdl->meshes[i].triangles) {
                free (mdl->meshes[i].triangles);
                mdl->meshes[i].triangles = NULL;
            }
            if (mdl->meshes[i].weights) {
                free (mdl->meshes[i].weights);
                mdl->meshes[i].weights = NULL;
            }
        }
        free (mdl->meshes);
        mdl->meshes = NULL;
    }
}


void MD5CalculateVerticesAndNormals(const md5_mesh_t *mesh, const md5_joint_t *skel, vec3_t *verticesAndNormals) {
    for (size_t i = 0, offset = 0; i < mesh->num_verts; ++i, offset += 2) {
        vec3_t finalVertex = { 0.0f, 0.0f, 0.0f };
        vec3_t finalNormal = { 0.0f, 0.0f, 0.0f };
        
        /* Calculate final vertex to draw with weights */
        for (size_t j = 0; j < mesh->vertices[i].count; ++j) {
            const md5_weight_t *weight = &mesh->weights[mesh->vertices[i].start + j];
            const md5_joint_t *joint   = &skel[weight->joint];
            
            /* Calculate transformed vertex for this weight */
            vec3_t wv;
            Quat_rotatePoint (joint->orient, weight->pos, wv);
            
            /* The sum of all weight->bias should be 1.0 */
            finalVertex[X] += (joint->pos[X] + wv[X]) * weight->bias;
            finalVertex[Y] += (joint->pos[Y] + wv[Y]) * weight->bias;
            finalVertex[Z] += (joint->pos[Z] + wv[Z]) * weight->bias;
            
            vec3_t wn;
            Quat_rotatePoint(joint->orient, weight->normal, wn);
            
            finalNormal[X] += wn[X] * weight->bias;
            finalNormal[Y] += wn[Y] * weight->bias;
            finalNormal[Z] += wn[Z] * weight->bias;
        }
        
        VEC_COPY(verticesAndNormals[offset + 0], finalVertex);
        VEC_COPY(verticesAndNormals[offset + 1], finalNormal);
    }
}
