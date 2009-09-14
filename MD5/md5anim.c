/*
 * md5anim.c -- md5mesh model loader + animation
 * last modification: aug. 14, 2007
 *
 * Doom3's md5mesh viewer with animation.  Animation portion.
 * Dependences: md5model.h, md5mesh.c.
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

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "md5.h"


/* Joint info */
typedef struct _joint_info_t
{
    char name[64];
    int parent;
    int flags;
    int startIndex;
} joint_info_t;

/* Base frame joint */
typedef struct _baseframe_joint_t
{
    vec3_t pos;
    quat4_t orient;
} baseframe_joint_t;


/**
 * Check if an animation can be used for a given model.  Model's
 * skeleton and animation's skeleton must match.
 */
int MD5CheckAnimValidity(const md5_model_t *mdl, const md5_anim_t *anim) {
    /* md5mesh and md5anim must have the same number of joints */
    if (mdl->num_joints != anim->num_joints)
        return 0;

    for (int i = 0; i < anim->num_frames; i++) {
        for (int j = 0; i < mdl->num_joints; ++i) {
            /* Joints must have the same parent index */
            if (mdl->skel[j].parent != anim->skel_frames[i][j].parent)
                return 0;
            
            /* Joints must have the same name */
            if (strcmp (mdl->skel[j].name, anim->skel_frames[i][j].name) != 0)
                return 0;
        }
    }
    return 1;
}

/**
 * Build skeleton for a given frame data.
 */
static void BuildFrameSkeleton (const joint_info_t *jointInfos, 
                                const baseframe_joint_t *baseFrame, 
                                const float *animFrameData, 
                                md5_joint_t *skelFrame, 
                                int num_joints) 
{
    for (int i = 0; i < num_joints; ++i) {
        const baseframe_joint_t *baseJoint = &baseFrame[i];
        vec3_t animatedPos;
        quat4_t animatedOrient;
        int j = 0;

        memcpy (animatedPos, baseJoint->pos, sizeof (vec3_t));
        memcpy (animatedOrient, baseJoint->orient, sizeof (quat4_t));

        if (jointInfos[i].flags & 1) /* Tx */ {
            animatedPos[X] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 2) /* Ty */ {
            animatedPos[Y] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 4) /* Tz */ {
            animatedPos[Z] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 8) /* Qx */ {
            animatedOrient[X] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 16) /* Qy */ {
            animatedOrient[Y] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 32) /* Qz */ {
            animatedOrient[Z] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        /* Compute orient quaternion's w value */
        Quat_computeW (animatedOrient);

        /* NOTE: we assume that this joint's parent has
         already been calculated, i.e. joint's ID should
         never be smaller than its parent ID. */
        md5_joint_t *thisJoint = &skelFrame[i];

        int parent = jointInfos[i].parent;
        thisJoint->parent = parent;
        strcpy (thisJoint->name, jointInfos[i].name);

        /* Has parent? */
        if (thisJoint->parent < 0) {
            memcpy (thisJoint->pos, animatedPos, sizeof (vec3_t));
            memcpy (thisJoint->orient, animatedOrient, sizeof (quat4_t));
        } else {
            md5_joint_t *parentJoint = &skelFrame[parent];
            vec3_t rpos; /* Rotated position */

            /* Add positions */
            Quat_rotatePoint (parentJoint->orient, animatedPos, rpos);
            thisJoint->pos[X] = rpos[X] + parentJoint->pos[X];
            thisJoint->pos[Y] = rpos[Y] + parentJoint->pos[Y];
            thisJoint->pos[Z] = rpos[Z] + parentJoint->pos[Z];

            /* Concatenate rotations */
            Quat_multQuat (parentJoint->orient, animatedOrient, thisJoint->orient);
            Quat_normalize (thisJoint->orient);
        }
    }
}

/**
 * Load an MD5 animation from file.
 */
int MD5ReadAnim (const char *filename, md5_anim_t *anim) {
    FILE *fp = NULL;
    char buff[512];
    joint_info_t *jointInfos = NULL;
    baseframe_joint_t *baseFrame = NULL;
    float *animFrameData = NULL;
    int version;
    int numAnimatedComponents;
    int frame_index;

    fp = fopen (filename, "rb");
    if (!fp) {
        fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
        return 0;
    }

    while (!feof (fp)) {
        /* Read whole line */
        fgets (buff, sizeof (buff), fp);

        if (sscanf (buff, " MD5Version %d", &version) == 1) {
            if (version != 10) {
                /* Bad version */
                fprintf (stderr, "Error: bad animation version\n");
                fclose (fp);
                return 0;
            }
        }
        else if (sscanf (buff, " numFrames %d", &anim->num_frames) == 1) {
            /* Allocate memory for skeleton frames and bounding boxes */
            if (anim->num_frames > 0) {
                CALLOC(anim->num_frames, anim->skel_frames);
                CALLOC(anim->num_frames, anim->bboxes);
            }
        }
        else if (sscanf (buff, " numJoints %d", &anim->num_joints) == 1) {
            if (anim->num_joints > 0) {
                for (int i = 0; i < anim->num_frames; ++i) {
                    /* Allocate memory for joints of each frame */
                    CALLOC(anim->num_joints, anim->skel_frames[i]);
                }

                /* Allocate temporary memory for building skeleton frames */
                CALLOC(anim->num_joints, jointInfos);
                CALLOC(anim->num_joints, baseFrame);
            }
        }
        else if (sscanf (buff, " frameRate %d", &anim->frameRate) == 1) {
            /* 
             printf ("md5anim: animation's frame rate is %d\n", anim->frameRate);
             */
        }
        else if (sscanf (buff, " numAnimatedComponents %d", &numAnimatedComponents) == 1) {
            if (numAnimatedComponents > 0) {
                /* Allocate memory for animation frame data */
                CALLOC(numAnimatedComponents, animFrameData);
            }
        }
        else if (strncmp (buff, "hierarchy {", 11) == 0) {
            for (int i = 0; i < anim->num_joints; ++i) {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                /* Read joint info */
                sscanf (buff, " %s %d %d %d", jointInfos[i].name, &jointInfos[i].parent, &jointInfos[i].flags, &jointInfos[i].startIndex);
            }
        }
        else if (strncmp (buff, "bounds {", 8) == 0) {
            for (int i = 0; i < anim->num_frames; ++i) {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                /* Read bounding box */
                sscanf (buff, " ( %f %f %f ) ( %f %f %f )", 
                        &anim->bboxes[i].min[X], &anim->bboxes[i].min[Y], &anim->bboxes[i].min[Z],
                        &anim->bboxes[i].max[X], &anim->bboxes[i].max[Y], &anim->bboxes[i].max[Z]);
            }
        }
        else if (strncmp (buff, "baseframe {", 10) == 0) {
            for (int i = 0; i < anim->num_joints; ++i) {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                /* Read base frame joint */
                if (sscanf (buff, " ( %f %f %f ) ( %f %f %f )", 
                            &baseFrame[i].pos[X], &baseFrame[i].pos[Y], &baseFrame[i].pos[Z],
                            &baseFrame[i].orient[X], &baseFrame[i].orient[Y], &baseFrame[i].orient[Z]) == 6) {
                    /* Compute the w component */
                    Quat_computeW (baseFrame[i].orient);
                }
            }
        }
        else if (sscanf (buff, " frame %d", &frame_index) == 1) {
            /* Read frame data */
            for (int i = 0; i < numAnimatedComponents; ++i)
                fscanf (fp, "%f", &animFrameData[i]);

            /* Build frame skeleton from the collected data */
            BuildFrameSkeleton (jointInfos, baseFrame, animFrameData, anim->skel_frames[frame_index], anim->num_joints);
        }
    }

    fclose (fp);

    /* Free temporary data allocated */
    if (animFrameData)
        free (animFrameData);

    if (baseFrame)
        free (baseFrame);

    if (jointInfos)
        free (jointInfos);

    return 1;
}

/**
 * Free resources allocated for the animation.
 */
void MD5FreeAnim(md5_anim_t *anim) {
  if (anim->skel_frames) {
      for (int i = 0; i < anim->num_frames; ++i) {
          if (anim->skel_frames[i]) {
              free (anim->skel_frames[i]);
              anim->skel_frames[i] = NULL;
          }
      }
      free (anim->skel_frames);
      anim->skel_frames = NULL;
  }
  if (anim->bboxes) {
      free (anim->bboxes);
      anim->bboxes = NULL;
  }
}

/**
 * Smoothly interpolate two skeletons
 */
void MD5InterpolateSkeletons(const md5_joint_t *skelA, const md5_joint_t *skelB, int num_joints, float interp, md5_joint_t *out) {
  for (int i = 0; i < num_joints; ++i) {
      /* Copy parent index */
      out[i].parent = skelA[i].parent;

      /* Linear interpolation for position */
      out[i].pos[X] = skelA[i].pos[X] + interp * (skelB[i].pos[X] - skelA[i].pos[X]);
      out[i].pos[Y] = skelA[i].pos[Y] + interp * (skelB[i].pos[Y] - skelA[i].pos[Y]);
      out[i].pos[Z] = skelA[i].pos[Z] + interp * (skelB[i].pos[Z] - skelA[i].pos[Z]);

      /* Spherical linear interpolation for orientation */
      Quat_slerp(skelA[i].orient, skelB[i].orient, interp, out[i].orient);
    }
}
