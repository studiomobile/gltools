#include <string.h>
#include <math.h>
#include <assert.h>
#include "glmath.h"


void Vec_crossProduct(const vec3_t va, const vec3_t vb, vec3_t cross) {
    cross[X] = (va[Y] * vb[Z]) - (va[Z] * vb[Y]);
    cross[Y] = (va[Z] * vb[X]) - (va[X] * vb[Z]);
	cross[Z] = (va[X] * vb[Y]) - (va[Y] * vb[X]);
}


void Vec_normalize(vec3_t v) {
    GLfloat mag = (v[X] * v[X]) + (v[Y] * v[Y]) + (v[Z] * v[Z]);
    if (mag > 0.0) {
        GLfloat oneOverMag = 1.0 / sqrt (mag);
        v[X] *= oneOverMag;
        v[Y] *= oneOverMag;
        v[Z] *= oneOverMag;
    }
}


void Vec_computeNormal(const vec3_t p1, const vec3_t p2, const vec3_t p3, vec3_t normal) {
    vec3_t v1;
    vec3_t v2;

    VEC_COPY(v1, p1);
    VEC_SUB(v1, p2);

    VEC_COPY(v2, p1);
    VEC_SUB(v2, p3);

    Vec_crossProduct(v1, v2, normal);
    Vec_normalize(normal);
}


void Vec_multAdd(const vec3_t x, const vec3_t a, const vec3_t b, vec3_t out) {
    out[X] = x[X]*a[X] + b[X];
    out[Y] = x[Y]*a[Y] + b[Y];
    out[Z] = x[Z]*a[Z] + b[Z];
}


void Vec_interpolate(const vec3_t va, const vec3_t vb, float t, vec3_t out) {
    out[X] = va[X] + t * (vb[X] - va[X]);
    out[Y] = va[Y] + t * (vb[Y] - va[Y]);
    out[Z] = va[Z] + t * (vb[Z] - va[Z]);
}


void Vec_scale(const vec3_t v, float scale, vec3_t out) {
    out[X] = v[X] * scale;
    out[Y] = v[Y] * scale;
    out[Z] = v[Z] * scale;
}



void Quat_computeW(quat4_t q) {
    float t = 1.0f - (q[X] * q[X]) - (q[Y] * q[Y]) - (q[Z] * q[Z]);
    if (t < 0.0f) {
        q[W] = 0.0f;
    } else {
        q[W] = -sqrt (t);
    }
}


void Quat_normalize(quat4_t q) {
    GLfloat mag = (q[X] * q[X]) + (q[Y] * q[Y]) + (q[Z] * q[Z]) + (q[W] * q[W]);
    if (mag > 0.0f) {
        GLfloat oneOverMag = 1.0f / sqrt (mag);
        q[X] *= oneOverMag;
        q[Y] *= oneOverMag;
        q[Z] *= oneOverMag;
        q[W] *= oneOverMag;
    }
}


void Quat_inverse (const quat4_t q, quat4_t inv) {
    inv[X] = -q[X];
    inv[Y] = -q[Y];
    inv[Z] = -q[Z];
    inv[W] =  q[W];
}


void Quat_multQuat(const quat4_t qa, const quat4_t qb, quat4_t out) {
    out[W] = (qa[W] * qb[W]) - (qa[X] * qb[X]) - (qa[Y] * qb[Y]) - (qa[Z] * qb[Z]);
    out[X] = (qa[X] * qb[W]) + (qa[W] * qb[X]) + (qa[Y] * qb[Z]) - (qa[Z] * qb[Y]);
    out[Y] = (qa[Y] * qb[W]) + (qa[W] * qb[Y]) + (qa[Z] * qb[X]) - (qa[X] * qb[Z]);
    out[Z] = (qa[Z] * qb[W]) + (qa[W] * qb[Z]) + (qa[X] * qb[Y]) - (qa[Y] * qb[X]);
}


void Quat_multVec(const quat4_t q, const vec3_t v, quat4_t out) {
    out[W] = - (q[X] * v[X]) - (q[Y] * v[Y]) - (q[Z] * v[Z]);
    out[X] =   (q[W] * v[X]) + (q[Y] * v[Z]) - (q[Z] * v[Y]);
    out[Y] =   (q[W] * v[Y]) + (q[Z] * v[X]) - (q[X] * v[Z]);
    out[Z] =   (q[W] * v[Z]) + (q[X] * v[Y]) - (q[Y] * v[X]);
}


void Quat_rotatePoint(const quat4_t q, const vec3_t in, vec3_t out) {
    quat4_t tmp, inv, final;

    Quat_inverse(q, inv);
    Quat_normalize(inv);
    Quat_multVec(q, in, tmp);
    Quat_multQuat(tmp, inv, final);

    VEC_COPY(out, final);
}


float Quat_dotProduct(const quat4_t qa, const quat4_t qb) {
    return ((qa[X] * qb[X]) + (qa[Y] * qb[Y]) + (qa[Z] * qb[Z]) + (qa[W] * qb[W]));
}


void Quat_slerp(const quat4_t qa, const quat4_t qb, float t, quat4_t out) {
    /* Check for out-of range parameter and return edge points if so */
    if (t <= 0.0) {
        memcpy (out, qa, sizeof(quat4_t));
        return;
    }

    if (t >= 1.0) {
        memcpy (out, qb, sizeof (quat4_t));
        return;
    }

    /* Compute "cosine of angle between quaternions" using dot product */
    float cosOmega = Quat_dotProduct(qa, qb);

    /* If negative dot, use -q1.  Two quaternions q and -q
     represent the same rotation, but may produce
     different slerp.  We chose q or -q to rotate using
     the acute angle. */
    float q1w = qb[W];
    float q1x = qb[X];
    float q1y = qb[Y];
    float q1z = qb[Z];

    if (cosOmega < 0.0f) {
        q1w = -q1w;
        q1x = -q1x;
        q1y = -q1y;
        q1z = -q1z;
        cosOmega = -cosOmega;
    }

    /* We should have two unit quaternions, so dot should be <= 1.0 */
    assert (cosOmega < 1.1f);

    /* Compute interpolation fraction, checking for quaternions almost exactly the same */
    float k0, k1;

    if (cosOmega > 0.9999f) {
        /* Very close - just use linear interpolation, which will protect againt a divide by zero */
        k0 = 1.0f - t;
        k1 = t;
    } else {
        /* Compute the sin of the angle using the trig identity sin^2(omega) + cos^2(omega) = 1 */
        float sinOmega = sqrt (1.0f - (cosOmega * cosOmega));

        /* Compute the angle from its sin and cosine */
        float omega = atan2 (sinOmega, cosOmega);

        /* Compute inverse of denominator, so we only have to divide once */
        float oneOverSinOmega = 1.0f / sinOmega;

        /* Compute interpolation parameters */
        k0 = sin ((1.0f - t) * omega) * oneOverSinOmega;
        k1 = sin (t * omega) * oneOverSinOmega;
    }

    /* Interpolate and return new quaternion */
    out[W] = (k0 * qa[W]) + (k1 * q1w);
    out[X] = (k0 * qa[X]) + (k1 * q1x);
    out[Y] = (k0 * qa[Y]) + (k1 * q1y);
    out[Z] = (k0 * qa[Z]) + (k1 * q1z);
}

