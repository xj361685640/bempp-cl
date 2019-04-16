#ifndef bempp_kernels_h
#define bempp_kernels_h

#include "bempp_base_types.h"

/* This extra level of indirection is needed in C99
   https://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
*/
#define EVALUATOR(x, y) CAT(x, _ ## y)

#ifdef KERNEL_FUNCTION
#define KERNEL(modus) EVALUATOR(KERNEL_FUNCTION, modus)
#endif

/* Definition of constants needed in the kernels.
   They must not be left undefined if not defined
   externally. Otherwise, it will lead to compilation
   errors even when the corresponding routines are not 
   used.

*/

#ifndef OMEGA
#define OMEGA M_ZERO
#endif

#ifndef WAVENUMBER_REAL
#define WAVENUMBER_REAL M_ZERO
#endif


inline void diff_vec4(const REALTYPE3 vec1, const REALTYPE4 vec2[3], REALTYPE4 result[3]){

    result[0] = vec1.x - vec2[0];
    result[1] = vec1.y - vec2[1];
    result[2] = vec1.z - vec2[2];

}

inline void diff_vec8(const REALTYPE3 vec1, const REALTYPE8 vec2[3], REALTYPE8 result[3]){

    result[0] = vec1.x - vec2[0];
    result[1] = vec1.y - vec2[1];
    result[2] = vec1.z - vec2[2];

}

inline void diff_vec16(const REALTYPE3 vec1, const REALTYPE16 vec2[3], REALTYPE16 result[3]){

    result[0] = vec1.x - vec2[0];
    result[1] = vec1.y - vec2[1];
    result[2] = vec1.z - vec2[2];

}

inline void laplace_single_layer_novec(const REALTYPE3 testGlobalPoint, 
                                         const REALTYPE3 trialGlobalPoint, 
                                         const REALTYPE3 testNormal,
                                         const REALTYPE3 trialNormal,
                                         REALTYPE* result)
{
    REALTYPE dist = distance(testGlobalPoint, trialGlobalPoint);
    *result = M_INV_4PI / dist;

}

inline void laplace_single_layer_vec4(const REALTYPE3 testGlobalPoint, 
                                      const REALTYPE4 trialGlobalPoint[3], 
                                      const REALTYPE3 testNormal,
                                      const REALTYPE4 trialNormal[3],
                                      REALTYPE4* result)
{
    REALTYPE4 diff[3];
    REALTYPE4 rdist;

    diff_vec4(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * rdist;

}

inline void laplace_single_layer_vec8(const REALTYPE3 testGlobalPoint, 
                                      const REALTYPE8 trialGlobalPoint[3], 
                                      const REALTYPE3 testNormal,
                                      const REALTYPE8 trialNormal[3],
                                      REALTYPE8* result)
{
    REALTYPE8 diff[3];
    REALTYPE8 rdist;

    diff_vec8(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * rdist;

}

inline void laplace_single_layer_vec16(const REALTYPE3 testGlobalPoint, 
                                       const REALTYPE16 trialGlobalPoint[3], 
                                       const REALTYPE3 testNormal,
                                       const REALTYPE16 trialNormal[3],
                                       REALTYPE16* result)
{
    REALTYPE16 diff[3];
    REALTYPE16 rdist;

    diff_vec16(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * rdist;

}

inline void laplace_double_layer_novec(const REALTYPE3 testGlobalPoint, 
                                         const REALTYPE3 trialGlobalPoint, 
                                         const REALTYPE3 testNormal,
                                         const REALTYPE3 trialNormal,
                                         REALTYPE* result)
{
    REALTYPE3 diff = trialGlobalPoint - testGlobalPoint;
    REALTYPE dist = length(diff);
    *result = -M_INV_4PI * dot(diff, trialNormal) / (dist * dist * dist);

}

inline void laplace_double_layer_vec4(const REALTYPE3 testGlobalPoint, 
                                         const REALTYPE4 trialGlobalPoint[3], 
                                         const REALTYPE3 testNormal,
                                         const REALTYPE4 trialNormal[3],
                                         REALTYPE4* result)
{
    REALTYPE4 diff[3];
    REALTYPE4 rdist;

    diff_vec4(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * (diff[0] * trialNormal[0] + diff[1] * trialNormal[1] + diff[2] * trialNormal[2]) * (rdist * rdist * rdist);

}

inline void laplace_double_layer_vec8(const REALTYPE3 testGlobalPoint, 
                                      const REALTYPE8 trialGlobalPoint[3], 
                                      const REALTYPE3 testNormal,
                                      const REALTYPE8 trialNormal[3],
                                      REALTYPE8* result)
{
    REALTYPE8 diff[3];
    REALTYPE8 rdist;

    diff_vec8(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * (diff[0] * trialNormal[0] + diff[1] * trialNormal[1] + diff[2] * trialNormal[2]) * (rdist * rdist * rdist);

}

inline void laplace_double_layer_vec16(const REALTYPE3 testGlobalPoint, 
                                       const REALTYPE16 trialGlobalPoint[3], 
                                       const REALTYPE3 testNormal,
                                       const REALTYPE16 trialNormal[3],
                                       REALTYPE16* result)
{
    REALTYPE16 diff[3];
    REALTYPE16 rdist;

    diff_vec16(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * (diff[0] * trialNormal[0] + diff[1] * trialNormal[1] + diff[2] * trialNormal[2]) * (rdist * rdist * rdist);

}

inline void laplace_adjoint_double_layer_novec(const REALTYPE3 testGlobalPoint, 
                                                 const REALTYPE3 trialGlobalPoint, 
                                                 const REALTYPE3 testNormal,
                                                 const REALTYPE3 trialNormal,
                                                 REALTYPE* result)
{
    REALTYPE3 diff = trialGlobalPoint - testGlobalPoint;
    REALTYPE dist = length(diff);
    *result = M_INV_4PI * dot(diff, testNormal) / (dist * dist * dist);

}

inline void laplace_adjoint_double_layer_vec4(const REALTYPE3 testGlobalPoint, 
                                              const REALTYPE4 trialGlobalPoint[3], 
                                              const REALTYPE3 testNormal,
                                              const REALTYPE4 trialNormal[3],
                                              REALTYPE4* result)
{
    REALTYPE4 diff[3];
    REALTYPE4 rdist;

    diff_vec4(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = -M_INV_4PI * (diff[0] * testNormal.x + diff[1] * testNormal.y + diff[2] * testNormal.z) * (rdist * rdist * rdist);

}



inline void laplace_adjoint_double_layer_vec8(const REALTYPE3 testGlobalPoint, 
                                              const REALTYPE8 trialGlobalPoint[3], 
                                              const REALTYPE3 testNormal,
                                              const REALTYPE8 trialNormal[3],
                                              REALTYPE8* result)
{
    REALTYPE8 diff[3];
    REALTYPE8 rdist;

    diff_vec8(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = -M_INV_4PI * (diff[0] * testNormal.x + diff[1] * testNormal.y + diff[2] * testNormal.z) * (rdist * rdist * rdist);

}

inline void laplace_adjoint_double_layer_vec16(const REALTYPE3 testGlobalPoint, 
                                               const REALTYPE16 trialGlobalPoint[3], 
                                               const REALTYPE3 testNormal,
                                               const REALTYPE16 trialNormal[3],
                                               REALTYPE16* result)
{
    REALTYPE16 diff[3];
    REALTYPE16 rdist;

    diff_vec16(testGlobalPoint, trialGlobalPoint, diff);
    rdist = rsqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = -M_INV_4PI * (diff[0] * testNormal.x + diff[1] * testNormal.y + diff[2] * testNormal.z) * (rdist * rdist * rdist);

}
inline void modified_helmholtz_real_single_layer_novec(const REALTYPE3 testGlobalPoint, 
                                                         const REALTYPE3 trialGlobalPoint, 
                                                         const REALTYPE3 testNormal,
                                                         const REALTYPE3 trialNormal,
                                                         REALTYPE* result)
{
    REALTYPE dist = distance(testGlobalPoint, trialGlobalPoint);
    *result = M_INV_4PI * exp(-OMEGA * dist) / dist;

}

inline void modified_helmholtz_real_single_layer_vec4(const REALTYPE3 testGlobalPoint, 
                                                      const REALTYPE4 trialGlobalPoint[3], 
                                                      const REALTYPE3 testNormal,
                                                      const REALTYPE4 trialNormal[3],
                                                      REALTYPE4* result)
{
    REALTYPE4 diff[3];
    REALTYPE4 dist;

    diff_vec4(testGlobalPoint, trialGlobalPoint, diff);
    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * exp(-OMEGA * dist) / dist;

}

inline void modified_helmholtz_real_single_layer_vec8(const REALTYPE3 testGlobalPoint, 
                                                      const REALTYPE8 trialGlobalPoint[3], 
                                                      const REALTYPE3 testNormal,
                                                      const REALTYPE8 trialNormal[3],
                                                      REALTYPE8* result)
{
    REALTYPE8 diff[3];
    REALTYPE8 dist;

    diff_vec8(testGlobalPoint, trialGlobalPoint, diff);
    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * exp(-OMEGA * dist) / dist;

}

inline void modified_helmholtz_real_single_layer_vec16(const REALTYPE3 testGlobalPoint, 
                                                       const REALTYPE16 trialGlobalPoint[3], 
                                                       const REALTYPE3 testNormal,
                                                       const REALTYPE16 trialNormal[3],
                                                       REALTYPE16* result)
{
    REALTYPE16 diff[3];
    REALTYPE16 dist;

    diff_vec16(testGlobalPoint, trialGlobalPoint, diff);
    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    *result = M_INV_4PI * exp(-OMEGA * dist) / dist;

}

inline void helmholtz_single_layer_novec(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE3 trialGlobalPoint, 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE3 trialNormal,
                                           REALTYPE* result)
{
    REALTYPE dist = distance(testGlobalPoint, trialGlobalPoint);
    result[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / dist;
    result[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / dist;

#ifdef WAVENUMBER_COMPLEX
    result[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    result[1] *= exp(-WAVENUMBER_COMPLEX * dist);
#endif
}

inline void helmholtz_single_layer_vec4(const REALTYPE3 testGlobalPoint, 
                                        const REALTYPE4 trialGlobalPoint[3], 
                                        const REALTYPE3 testNormal,
                                        const REALTYPE4 trialNormal[3],
                                        REALTYPE4* result)
{
    REALTYPE4 diff[3];
    REALTYPE4 dist;

    diff_vec4(testGlobalPoint, trialGlobalPoint, diff);
    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    result[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / dist;
    result[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / dist;

#ifdef WAVENUMBER_COMPLEX
    result[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    result[1] *= exp(-WAVENUMBER_COMPLEX * dist);
#endif


}

inline void helmholtz_single_layer_vec8(const REALTYPE3 testGlobalPoint, 
                                        const REALTYPE8 trialGlobalPoint[3], 
                                        const REALTYPE3 testNormal,
                                        const REALTYPE8 trialNormal[3],
                                        REALTYPE8* result)
{
    REALTYPE8 diff[3];
    REALTYPE8 dist;

    diff_vec8(testGlobalPoint, trialGlobalPoint, diff);
    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    result[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / dist;
    result[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / dist;

#ifdef WAVENUMBER_COMPLEX
    result[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    result[1] *= exp(-WAVENUMBER_COMPLEX * dist);
#endif

}

inline void helmholtz_single_layer_vec16(const REALTYPE3 testGlobalPoint, 
                                         const REALTYPE16 trialGlobalPoint[3], 
                                         const REALTYPE3 testNormal,
                                         const REALTYPE16 trialNormal[3],
                                         REALTYPE16* result)
{
    REALTYPE16 diff[3];
    REALTYPE16 dist;

    diff_vec16(testGlobalPoint, trialGlobalPoint, diff);
    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    result[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / dist;
    result[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / dist;

#ifdef WAVENUMBER_COMPLEX
    result[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    result[1] *= exp(-WAVENUMBER_COMPLEX * dist);
#endif

}

inline void helmholtz_double_layer_novec(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE3 trialGlobalPoint, 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE3 trialNormal,
                                           REALTYPE* result)
{
    REALTYPE3 diff = trialGlobalPoint - testGlobalPoint;
    REALTYPE dist = length(diff);

    REALTYPE inner = dot(diff, trialNormal);


    REALTYPE factor1[2];
    REALTYPE factor2[2];

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    result[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]) * inner;
    result[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]) * inner;

}

inline void helmholtz_double_layer_vec4(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE4 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE4 trialNormal[3],
                                           REALTYPE4* result)
{
    REALTYPE4 diff[3];
    REALTYPE4 inner;
    REALTYPE4 dist;

    REALTYPE4 factor1[2];
    REALTYPE4 factor2[2];

    diff_vec4(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    inner = -(trialNormal[0] * diff[0] + trialNormal[1] * diff[1] + trialNormal[2] * diff[2]);

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    result[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]) * inner;
    result[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]) * inner;

}

inline void helmholtz_double_layer_vec8(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE8 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE8 trialNormal[3],
                                           REALTYPE8* result)
{
    REALTYPE8 diff[3];
    REALTYPE8 inner;
    REALTYPE8 dist;

    REALTYPE8 factor1[2];
    REALTYPE8 factor2[2];

    diff_vec8(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    inner = -(trialNormal[0] * diff[0] + trialNormal[1] * diff[1] + trialNormal[2] * diff[2]);



    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    result[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]) * inner;
    result[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]) * inner;

}

inline void helmholtz_double_layer_vec16(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE16 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE16 trialNormal[3],
                                           REALTYPE16* result)
{
    REALTYPE16 diff[3];
    REALTYPE16 inner;
    REALTYPE16 dist;

    REALTYPE16 factor1[2];
    REALTYPE16 factor2[2];

    diff_vec16(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    inner = -(trialNormal[0] * diff[0] + trialNormal[1] * diff[1] + trialNormal[2] * diff[2]);

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    result[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]) * inner;
    result[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]) * inner;

}

inline void helmholtz_adjoint_double_layer_novec(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE3 trialGlobalPoint, 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE3 trialNormal,
                                           REALTYPE* result)
{
    REALTYPE3 diff = trialGlobalPoint - testGlobalPoint;
    REALTYPE dist = length(diff);

    REALTYPE inner = -dot(diff, testNormal);


    REALTYPE factor1[2];
    REALTYPE factor2[2];

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    result[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]) * inner;
    result[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]) * inner;

}

inline void helmholtz_adjoint_double_layer_vec4(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE4 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE4 trialNormal[3],
                                           REALTYPE4* result)
{
    REALTYPE4 diff[3];
    REALTYPE4 inner;
    REALTYPE4 dist;

    REALTYPE4 factor1[2];
    REALTYPE4 factor2[2];

    diff_vec4(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    inner = testNormal.x * diff[0] + testNormal.y * diff[1] + testNormal.z * diff[2];

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    result[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]) * inner;
    result[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]) * inner;

}

inline void helmholtz_adjoint_double_layer_vec8(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE8 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE8 trialNormal[3],
                                           REALTYPE8* result)
{
    REALTYPE8 diff[3];
    REALTYPE8 inner;
    REALTYPE8 dist;

    REALTYPE8 factor1[2];
    REALTYPE8 factor2[2];

    diff_vec8(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    inner = testNormal.x * diff[0] + testNormal.y * diff[1] + testNormal.z * diff[2];



    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    result[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]) * inner;
    result[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]) * inner;

}

inline void helmholtz_adjoint_double_layer_vec16(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE16 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE16 trialNormal[3],
                                           REALTYPE16* result)
{
    REALTYPE16 diff[3];
    REALTYPE16 inner;
    REALTYPE16 dist;

    REALTYPE16 factor1[2];
    REALTYPE16 factor2[2];

    diff_vec16(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
    inner = testNormal.x * diff[0] + testNormal.y * diff[1] + testNormal.z * diff[2];

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    result[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]) * inner;
    result[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]) * inner;

}

inline void helmholtz_gradient_novec(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE3 trialGlobalPoint, 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE3 trialNormal,
                                           REALTYPE result[3][2])
{
    REALTYPE3 diff = trialGlobalPoint - testGlobalPoint;
    REALTYPE dist = length(diff);

    REALTYPE product[2];

    REALTYPE factor1[2];
    REALTYPE factor2[2];

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    product[0] = -(factor1[0] * factor2[0] - factor1[1] * factor2[1]);
    product[1] = -(factor1[0] * factor2[1] + factor1[1] * factor2[0]);

    result[0][0] = product[0] * diff.x;
    result[0][1] = product[1] * diff.x;
    result[1][0] = product[0] * diff.y;
    result[1][1] = product[1] * diff.y;
    result[2][0] = product[0] * diff.z;
    result[2][1] = product[1] * diff.z;

}

inline void helmholtz_gradient_vec4(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE4 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE4 trialNormal[3],
                                           REALTYPE4 result[3][2])
{
    REALTYPE4 diff[3];
    REALTYPE4 dist;
    REALTYPE4 product[2];
    REALTYPE4 factor1[2];
    REALTYPE4 factor2[2];

    diff_vec4(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    product[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]);
    product[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]);

    result[0][0] = product[0] * diff[0];
    result[0][1] = product[1] * diff[0];
    result[1][0] = product[0] * diff[1];
    result[1][1] = product[1] * diff[1];
    result[2][0] = product[0] * diff[2];
    result[2][1] = product[1] * diff[2];

}

inline void helmholtz_gradient_vec8(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE8 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE8 trialNormal[3],
                                           REALTYPE8 result[3][2])
{
    REALTYPE8 diff[3];
    REALTYPE8 dist;
    REALTYPE8 product[2];
    REALTYPE8 factor1[2];
    REALTYPE8 factor2[2];

    diff_vec8(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    product[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]);
    product[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]);

    result[0][0] = product[0] * diff[0];
    result[0][1] = product[1] * diff[0];
    result[1][0] = product[0] * diff[1];
    result[1][1] = product[1] * diff[1];
    result[2][0] = product[0] * diff[2];
    result[2][1] = product[1] * diff[2];

}
inline void helmholtz_gradient_vec16(const REALTYPE3 testGlobalPoint, 
                                           const REALTYPE16 trialGlobalPoint[3], 
                                           const REALTYPE3 testNormal,
                                           const REALTYPE16 trialNormal[3],
                                           REALTYPE16 result[3][2])
{
    REALTYPE16 diff[3];
    REALTYPE16 dist;
    REALTYPE16 product[2];
    REALTYPE16 factor1[2];
    REALTYPE16 factor2[2];

    diff_vec16(testGlobalPoint, trialGlobalPoint, diff);

    dist = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

    factor1[0] = M_INV_4PI * cos(WAVENUMBER_REAL * dist) / (dist * dist * dist);
    factor1[1] = M_INV_4PI * sin(WAVENUMBER_REAL * dist) / (dist * dist * dist);

    factor2[0] = -M_ONE;
    factor2[1] = WAVENUMBER_REAL * dist;

#ifdef WAVENUMBER_COMPLEX
    factor1[0] *= exp(-WAVENUMBER_COMPLEX * dist);
    factor1[1] *= exp(-WAVENUMBER_COMPLEX * dist);

    factor2[0] += -WAVENUMBER_COMPLEX * dist;
#endif

    product[0] = (factor1[0] * factor2[0] - factor1[1] * factor2[1]);
    product[1] = (factor1[0] * factor2[1] + factor1[1] * factor2[0]);

    result[0][0] = product[0] * diff[0];
    result[0][1] = product[1] * diff[0];
    result[1][0] = product[0] * diff[1];
    result[1][1] = product[1] * diff[1];
    result[2][0] = product[0] * diff[2];
    result[2][1] = product[1] * diff[2];

}


#endif
