#ifndef __GLMATH_H__
#define __GLMATH_H__

#include "gl.h"

/* Vectors */
typedef GLfloat vec2_t[2];
typedef GLfloat vec3_t[3];


/* Quaternion (x, y, z, w) */
typedef GLfloat quat4_t[4];


enum {
    X = 0, Y = 1, Z = 2, W = 3
};


enum {
    U = 0, V = 1
};


#define VEC_ZERO(dest) dest[X] = dest[Y] = dest[Z] = 0;
#define VEC_NEG(dest) dest[X] = -dest[X]; dest[Y] = -dest[Y]; dest[Z] = -dest[Z];
#define VEC_COPY(dest, source) dest[X] = source[X]; dest[Y] = source[Y]; dest[Z] = source[Z];
#define VEC_SUB(minuend, subtrahend) minuend[X] -= subtrahend[X]; minuend[Y] -= subtrahend[Y]; minuend[Z] -= subtrahend[Z];
#define VEC_ADD(addend1, addend2) addend1[X] += addend2[X]; addend1[Y] += addend2[Y]; addend1[Z] += addend2[Z];

/**
 * Vector prototypes
 */
void Vec_crossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
void Vec_normalize(vec3_t v);
void Vec_computeNormal(const vec3_t p1, const vec3_t p2, const vec3_t p3, vec3_t normal);
void Vec_multAdd(const vec3_t x, const vec3_t a, const vec3_t b, vec3_t out);


/**
 * Quaternion prototypes
 */
void  Quat_computeW (quat4_t q);
void  Quat_normalize (quat4_t q);
void  Quat_inverse (const quat4_t q, quat4_t inv);
void  Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out);
void  Quat_multVec (const quat4_t q, const vec3_t v, quat4_t out);
void  Quat_rotatePoint (const quat4_t q, const vec3_t in, vec3_t out);
float Quat_dotProduct (const quat4_t qa, const quat4_t qb);
void  Quat_slerp (const quat4_t qa, const quat4_t qb, float t, quat4_t out);


#endif
