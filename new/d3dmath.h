/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File: d3dmath.h
 *
 ***************************************************************************/
#ifndef __D3DMATH_H__
#define __D3DMATH_H__

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Normalises the vector v
 */

void D3DVECTORSubtract(D3DVECTOR *dst,D3DVECTOR *v1,D3DVECTOR *v2);
void D3DVECTORAdd(D3DVECTOR *dst,D3DVECTOR *v1,D3DVECTOR *v2);
void D3DVECTORScalarMultiply(D3DVECTOR *dst,D3DVECTOR *v1,float Scalar);

LPD3DVECTOR D3DVECTORNormalise(LPD3DVECTOR v);

/*
 * Calculates cross product of a and b.
 */
LPD3DVECTOR D3DVECTORCrossProduct(LPD3DVECTOR lpd, LPD3DVECTOR lpa, LPD3DVECTOR lpb);

/*
 * lpDst = lpSrc1 * lpSrc2
 * lpDst can be equal to lpSrc1 or lpSrc2
 */
D3DVALUE D3DVECTORDotProduct(LPD3DVECTOR lpa,LPD3DVECTOR lpb);

LPD3DMATRIX MultiplyD3DMATRIX(LPD3DMATRIX lpDst, LPD3DMATRIX lpSrc1, 
                              LPD3DMATRIX lpSrc2);
/*
 * -1 d = a
 */
LPD3DMATRIX D3DMATRIXInvert(LPD3DMATRIX d, LPD3DMATRIX a);

/*
 * Set the rotation part of a matrix such that the vector lpD is the new
 * z-axis and lpU is the new y-axis.
 */
LPD3DMATRIX D3DMATRIXSetRotation(LPD3DMATRIX lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU);

LPD3DMATRIX D3DMATRIXCatX(LPD3DMATRIX lpM,float *radians);

/*
 * Calculates a point along a B-Spline curve defined by four points. p
 * n output, contain the point. t                                Position
 * along the curve between p2 and p3.  This position is a float between 0
 * and 1. p1, p2, p3, p4    Points defining spline curve. p, at parameter
 * t along the spline curve
 */
void spline(LPD3DVECTOR p, float t, LPD3DVECTOR p1, LPD3DVECTOR p2,
            LPD3DVECTOR p3, LPD3DVECTOR p4);

void VectorMatrixMultiply(D3DTLVERTEX *dst,D3DVERTEX *srcv,D3DMATRIX *srcm);
void NormalTransform(D3DTLVERTEX *dst,D3DVERTEX *srcv,D3DMATRIX *srcm);
void ConcatenateXRotation(LPD3DMATRIX lpM, float Radians );
void ConcatenateYRotation(LPD3DMATRIX lpM, float Radians );
void ConcatenateZRotation(LPD3DMATRIX lpM, float Radians );

#ifdef __cplusplus
};
#endif

#endif // __D3DMATH_H__

