#include "stdafx.h"

#include "d3d.h"
#include "math.h"

extern "C"
 {
extern void FastRowTimesCol(float *dst,float *src1,float *src2);
/*
 * Normalises the vector v
 */
LPD3DVECTOR 
D3DVECTORNormalise(LPD3DVECTOR v)
{
    float vx, vy, vz, inv_mod;
    vx = v->x;
    vy = v->y;
    vz = v->z;
    if ((vx == 0) && (vy == 0) && (vz == 0))
	{
		v->y=1.f;
        return v;
	}
    inv_mod = (float)(1.0 / sqrt(vx * vx + vy * vy + vz * vz));
    v->x = vx * inv_mod;
    v->y = vy * inv_mod;
    v->z = vz * inv_mod;
    return v;
}

/*
void VectorMatrixMultiply(D3DTLVERTEX *dst,D3DVERTEX *srcv,D3DMATRIX *srcm,BOOL Homo)
{
	float *m,*tmp1,*tmp2,*d;
	m=(float *)srcm;

	d=(float *)dst;
	for (int i=0; i<4; i++) 
	{
		*d = 0.0f;
		tmp1=m;
		m++;
		tmp2=(float *)srcv;
		for (int j=0; j<3; j++) 
		{
			(*d)+=*(tmp2++)*(*tmp1);
			tmp1+=4;
		}
		(*d++)+=(*tmp1);
	}
}
*/

void VectorMatrixMultiply(D3DTLVERTEX *dst,D3DVERTEX *srcv,D3DMATRIX *srcm)
{
/*
	float *m,*tmp1,*tmp2,*d;
	m=(float *)srcm;

	d=(float *)dst;
	for (int i=0; i<4; i++) 
	{
		*d = 0.0f;
		tmp1=m;
		m++;
		tmp2=(float *)srcv;
		for (int j=0; j<3; j++) 
		{
			(*d)+=*(tmp2++)*(*tmp1);
			tmp1+=4;
		}
		(*d++)+=(*tmp1);
	}
*/
	__asm
	{
		mov		eax,srcv
		mov		ebx,srcm
		mov		edx,dst

		fld		dword ptr [eax]			; 0	 s0
		fmul	dword ptr [ebx]			; 1	 s0*m00
		fld		dword ptr [eax+4]		; 2	 s1, s0*m00
		fmul	dword ptr [ebx+16]		; 3	 s1*m10, s0*m00
		fld		dword ptr [eax+8]		; 4	 s2, s1*m10, s0*m00
		fmul	dword ptr [ebx+32]		; 5	 s2*m20, s1*m10, s0*m00
		fld		dword ptr [ebx+48]		; 6	 m30, s2*m20, s1*m10, s0*m00
		faddp	st(3),st(0)				; 7  s2*m20, s1*m10, (s0*m00)+m30
		fxch	st(1)					; 7  s1*m10, s2*m20, (s0*m00)+m30
		faddp	st(2),st(0)				; 8  s2*m20, (s1*m10)+(s0*m00)+m30

		fld		dword ptr [eax]			; 9	 s0, s2*m20, (s1*m10)+(s0*m00)+m30
		fmul	dword ptr [ebx+4]		; 10 s0*m01, s2*m20, (s1*m10)+(s0*m00)+m30
		fxch	st(2)					; 10 (s1*m10)+(s0*m00)+m30, s2*m20, s0*m01
		faddp	st(1),st(0)				; 11 (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+4]		; 12 s1, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+20]		; 13 s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+8]		; 14 s2, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+36]		; 15 s2*m21, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fxch	st(2)					; 15 (s2*m20)+(s1*m10)+(s0*m00)+m30, s1*m11, s2*m21, s0*m01
		fstp	dword ptr [edx]			; 16 s1*m11, s2*m21, s0*m01
		fld		dword ptr [ebx+52]		; 17 m31, s1*m11, s2*m21, s0*m01
		faddp	st(3),st(0)				; 18 s1*m11, s2*m21, (s0*m01)+m31
		fxch	st(1)					; 18 s2*m21, s1*m11, (s0*m01)+m31
		faddp	st(2),st(0)				; 19 s1*m11, (s2*m21)+(s0*m01)+m31

		fld		dword ptr [eax]			; 20	 s0, s2*m20, (s1*m10)+(s0*m00)+m30
		fmul	dword ptr [ebx+8]		; 21 s0*m02, s2*m20, (s1*m10)+(s0*m00)+m30
		fxch	st(2)					; 21 (s1*m10)+(s0*m00)+m30, s2*m20, s0*m01
		faddp	st(1),st(0)				; 22 (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+4]		; 23 s1, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+24]		; 24 s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+8]		; 25 s2, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+40]		; 26 s2*m21, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fxch	st(2)					; 26 (s2*m20)+(s1*m10)+(s0*m00)+m30, s1*m11, s2*m21, s0*m01
		fstp	dword ptr [edx+4]		; 27 s1*m12, s2*m22, s0*m02
		fld		dword ptr [ebx+56]		; 28 m31, s1*m11, s2*m21, s0*m01
		faddp	st(3),st(0)				; 29 s1*m11, s2*m21, (s0*m01)+m31
		fxch	st(1)					; 29 s2*m21, s1*m11, (s0*m01)+m31
		faddp	st(2),st(0)				; 30 s1*m11, (s2*m21)+(s0*m01)+m31

		fld		dword ptr [eax]			; 31 s0, s2*m20, (s1*m10)+(s0*m00)+m30
		fmul	dword ptr [ebx+12]		; 32 s0*m03, s2*m20, (s1*m10)+(s0*m00)+m30
		fxch	st(2)					; 32 (s1*m10)+(s0*m00)+m30, s2*m20, s0*m01
		faddp	st(1),st(0)				; 33 (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+4]		; 34 s1, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+28]		; 35 s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+8]		; 36 s2, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+44]		; 37 s2*m21, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fxch	st(2)					; 37 (s2*m20)+(s1*m10)+(s0*m00)+m30, s1*m11, s2*m21, s0*m01
		fstp	dword ptr [edx+8]		; 38 s1*m12, s2*m22, s0*m02
		fld		dword ptr [ebx+60]		; 39 m31, s1*m11, s2*m21, s0*m01
		faddp	st(3),st(0)				; 40 s1*m11, s2*m21, (s0*m01)+m31
		fxch	st(1)					; 40 s2*m21, s1*m11, (s0*m01)+m31
		faddp	st(2),st(0)				; 41 s1*m11, (s2*m21)+(s0*m01)+m31
		faddp	st(1),st(0)				; 42 (s1*m11)+(s2*m21)+(s0*m01)+m31
		fstp	dword ptr [edx+12]
	}
}

void NormalTransform(D3DTLVERTEX *dst,D3DVERTEX *srcv,D3DMATRIX *srcm)
{
	__asm
	{
		mov		eax,srcv
		add		eax,12					; point to nx,ny,nz
		mov		ebx,srcm
		mov		edx,dst

		fld		dword ptr [eax]			; 0	 s0
		fmul	dword ptr [ebx]			; 1	 s0*m00
		fld		dword ptr [eax+4]		; 2	 s1, s0*m00
		fmul	dword ptr [ebx+16]		; 3	 s1*m10, s0*m00
		fld		dword ptr [eax+8]		; 4	 s2, s1*m10, s0*m00
		fmul	dword ptr [ebx+32]		; 5	 s2*m20, s1*m10, s0*m00
;		fld		dword ptr [ebx+48]		; 6	 m30, s2*m20, s1*m10, s0*m00
;		faddp	st(3),st(0)				; 7  s2*m20, s1*m10, (s0*m00)+m30
		fxch	st(1)					; 7  s1*m10, s2*m20, (s0*m00)+m30
		faddp	st(2),st(0)				; 8  s2*m20, (s1*m10)+(s0*m00)+m30
		add		ebx,4					; next col in srcm

		fld		dword ptr [eax]			; 9	 s0, s2*m20, (s1*m10)+(s0*m00)+m30
		fmul	dword ptr [ebx]			; 10 s0*m01, s2*m20, (s1*m10)+(s0*m00)+m30
		fxch	st(2)					; 10 (s1*m10)+(s0*m00)+m30, s2*m20, s0*m01
		faddp	st(1),st(0)				; 11 (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+4]		; 12 s1, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+16]		; 13 s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+8]		; 14 s2, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+32]		; 15 s2*m21, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fxch	st(2)					; 15 (s2*m20)+(s1*m10)+(s0*m00)+m30, s1*m11, s2*m21, s0*m01
		fstp	dword ptr [edx]			; 16 s1*m11, s2*m21, s0*m01
		add		edx,4
;		fld		dword ptr [ebx+48]		; 17 m31, s1*m11, s2*m21, s0*m01
;		faddp	st(3),st(0)				; 18 s1*m11, s2*m21, (s0*m01)+m31
		fxch	st(1)					; 18 s2*m21, s1*m11, (s0*m01)+m31
		faddp	st(2),st(0)				; 19 s1*m11, (s2*m21)+(s0*m01)+m31
		add		ebx,4					; next col in srcm

		fld		dword ptr [eax]			; 20	 s0, s2*m20, (s1*m10)+(s0*m00)+m30
		fmul	dword ptr [ebx]			; 21 s0*m02, s2*m20, (s1*m10)+(s0*m00)+m30
		fxch	st(2)					; 21 (s1*m10)+(s0*m00)+m30, s2*m20, s0*m01
		faddp	st(1),st(0)				; 22 (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+4]		; 23 s1, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+16]		; 24 s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+8]		; 25 s2, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+32]		; 26 s2*m21, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fxch	st(2)					; 26 (s2*m20)+(s1*m10)+(s0*m00)+m30, s1*m11, s2*m21, s0*m01
		fstp	dword ptr [edx]			; 27 s1*m12, s2*m22, s0*m02
		add		edx,4
;		fld		dword ptr [ebx+48]		; 28 m31, s1*m11, s2*m21, s0*m01
;		faddp	st(3),st(0)				; 29 s1*m11, s2*m21, (s0*m01)+m31
		fxch	st(1)					; 29 s2*m21, s1*m11, (s0*m01)+m31
		faddp	st(2),st(0)				; 30 s1*m11, (s2*m21)+(s0*m01)+m31
		add		ebx,4					; next col in srcm

		fld		dword ptr [eax]			; 31 s0, s2*m20, (s1*m10)+(s0*m00)+m30
		fmul	dword ptr [ebx]			; 32 s0*m03, s2*m20, (s1*m10)+(s0*m00)+m30
		fxch	st(2)					; 32 (s1*m10)+(s0*m00)+m30, s2*m20, s0*m01
		faddp	st(1),st(0)				; 33 (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+4]		; 34 s1, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+16]		; 35 s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fld		dword ptr [eax+8]		; 36 s2, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fmul	dword ptr [ebx+32]		; 37 s2*m21, s1*m11, (s2*m20)+(s1*m10)+(s0*m00)+m30, s0*m01
		fxch	st(2)					; 37 (s2*m20)+(s1*m10)+(s0*m00)+m30, s1*m11, s2*m21, s0*m01
		fstp	dword ptr [edx]			; 38 s1*m12, s2*m22, s0*m02
		add		edx,4
;		fld		dword ptr [ebx+48]		; 39 m31, s1*m11, s2*m21, s0*m01
;		faddp	st(3),st(0)				; 40 s1*m11, s2*m21, (s0*m01)+m31
		fxch	st(1)					; 40 s2*m21, s1*m11, (s0*m01)+m31
		faddp	st(2),st(0)				; 41 s1*m11, (s2*m21)+(s0*m01)+m31
		faddp	st(1),st(0)				; 42 (s1*m11)+(s2*m21)+(s0*m01)+m31
		fstp	dword ptr [edx]
	}

}

D3DVALUE D3DVECTORDotProduct(LPD3DVECTOR lpa,LPD3DVECTOR lpb)
{
	float r;
	r=lpa->x*lpb->x+lpa->y*lpb->y+lpa->z*lpb->z;
	return(r);
}

void D3DVECTORSubtract(D3DVECTOR *dst,D3DVECTOR *v1,D3DVECTOR *v2)
{
	dst->x=v1->x-v2->x;
	dst->y=v1->y-v2->y;
	dst->z=v1->z-v2->z;
}

void D3DVECTORAdd(D3DVECTOR *dst,D3DVECTOR *v1,D3DVECTOR *v2)
{
	dst->x=v1->x+v2->x;
	dst->y=v1->y+v2->y;
	dst->z=v1->z+v2->z;
}


void D3DVECTORScalarMultiply(D3DVECTOR *dst,D3DVECTOR *v1,float scalar)
{
	dst->x=v1->x*scalar;
	dst->y=v1->y*scalar;
	dst->z=v1->z*scalar;
}

/*
 * Calculates cross product of a and b.
 */
LPD3DVECTOR 
D3DVECTORCrossProduct(LPD3DVECTOR lpd, LPD3DVECTOR lpa, LPD3DVECTOR lpb)
{

    lpd->x = lpa->y * lpb->z - lpa->z * lpb->y;
    lpd->y = lpa->z * lpb->x - lpa->x * lpb->z;
    lpd->z = lpa->x * lpb->y - lpa->y * lpb->x;
    return lpd;
}

/*
 * lpDst = lpSrc1 * lpSrc2
 * lpDst cannot be equal to lpSrc1 or lpSrc2
 */
#pragma optimize("",off)

LPD3DMATRIX D3DMATRIXCatX(LPD3DMATRIX lpDst, float *lpRadians) 
{
	float aElements[4][4],*tmp;
	memcpy(&aElements[0][0], lpDst, sizeof(D3DMATRIX));
	tmp=&aElements[0][0];
	__asm
	{
		push	eax
		push	ebx
		push	edx
		mov		eax,tmp
		mov		ebx,lpRadians
		mov		edx,lpDst
		fld		dword ptr [ebx]		// radians
		fsincos						// cos, sin
		fld		st(1)				// sin, cos, sin
		fld		st(1)				// cos, sin, cos, sin
		fld		st(1)				// sin, cos, sin, cos, sin
		fld		st(1)				// cos, sin, cos, sin, cos, sin
		fmul	dword ptr [eax+4]	// a[0][1]*cos, sin, cos, sin, cos, sin
		fxch	st(1)				// sin, a[0][1]*cos, cos, sin, cos, sin
		fmul	dword ptr [eax+8]	// a[0][2]*sin, a[0][1]*cos, cos, sin, cos, sin
		fxch	st(2)				// cos, a[0][1]*cos, a[0][2]*sin, sin, cos, sin
		fmul	dword ptr [eax+20]	// a[1][1]*cos, a[0][1]*cos, a[0][2]*sin, sin, cos, sin
		fxch	st(3)				// sin, a[0][1]*cos, a[0][2]*sin, a[1][1]*cos, cos, sin
		fmul	dword ptr [eax+24]	// a[1][2]*sin, a[0][1]*cos, a[0][2]*sin, a[1][1]*cos, cos, sin
		fxch	st(1)				// a[0][1]*cos, a[1][2]*sin, a[0][2]*sin, a[1][1]*cos, cos, sin
		faddp	st(2),st(0)			// a[1][2]*sin, a[0][2]*sin+a[0][1]*cos, a[1][1]*cos, cos, sin
		faddp	st(2),st(0)			// a[0][2]*sin+a[0][1]*cos, a[1][1]*cos+a[1][2]*sin, , cos, sin
		fstp	dword ptr [edx+4]	// a[1][1]*cos+a[1][2]*sin, cos, sin
		fstp	dword ptr [edx+20]	// cos, sin
		fld		st(1)				// sin, cos, sin
		fld		st(1)				// cos, sin, cos, sin
		fld		st(1)				// sin, cos, sin, cos, sin
		fld		st(1)				// cos, sin, cos, sin, cos, sin
		fmul	dword ptr [eax+36]	// a[2][1]*cos, sin, cos, sin, cos, sin
		fxch	st(1)				// sin, a[0][1]*cos, cos, sin, cos, sin
		fmul	dword ptr [eax+40]	// a[2][2]*sin, a[2][1]*cos, cos, sin, cos, sin
		fxch	st(2)				// cos, a[2][2]*sin, a[2][1]*cos, sin, cos, sin
		fmul	dword ptr [eax+52]	// a[3][1]*cos, a[2][2]*sin, a[2][1]*cos, sin, cos, sin
		fxch	st(3)				// sin, a[1][1]*cos, a[0][2]*sin, a[0][1]*cos, cos, sin
		fmul	dword ptr [eax+56]	// a[3][2]*sin, a[3][1]*cos, a[2][2]*sin, a[2][1]*cos, cos, sin
		fxch	st(1)				// a[2][2]*sin, a[3][2]*sin, a[1][1]*cos, a[2][1]*cos, cos, sin
		faddp	st(2),st(0)			// a[3][2]*sin, a[3][1]*cos+a[2][2]*sin, a[2][1]*cos, cos, sin
		faddp	st(2),st(0)			// a[3][1]*cos+a[2][2]*sin, a[2][1]*cos+a[3][2]*sin, cos, sin
		fstp	dword ptr [edx+36]	// a[2][1]*cos+a[3][2]*sin, cos, sin
		fstp	dword ptr [edx+52]	// cos, sin
		fxch	st(1)				// sin, cos
		fchs						// -sin, cos
		fld		st(1)				// cos, -sin, cos
		fld		st(1)				// -sin, cos, -sin, cos
		fld		st(1)				// cos, -sin, cos, -sin, cos
		fld		st(1)				// -sin, cos, -sin, cos, -sin, cos
		fmul	dword ptr [eax+4]	// a[0][1]*cos, sin, cos, sin, cos, sin
		fxch	st(1)				// sin, a[0][1]*cos, cos, sin, cos, sin
		fmul	dword ptr [eax+8]	// a[0][2]*sin, a[0][1]*cos, cos, sin, cos, sin
		fxch	st(2)				// cos, a[0][2]*sin, a[0][1]*cos, sin, cos, sin
		fmul	dword ptr [eax+20]	// a[1][1]*cos, a[0][2]*sin, a[0][1]*cos, sin, cos, sin
		fxch	st(3)				// sin, a[1][1]*cos, a[0][2]*sin, a[0][1]*cos, cos, sin
		fmul	dword ptr [eax+24]	// a[1][2]*sin, a[1][1]*cos, a[0][2]*sin, a[0][1]*cos, cos, sin
		fxch	st(1)				// a[0][2]*sin, a[1][2]*sin, a[1][1]*cos, a[0][1]*cos, cos, sin
		faddp	st(2),st(0)			// a[1][2]*sin, a[1][1]*cos+a[0][2]*sin, a[0][1]*cos, cos, sin
		faddp	st(2),st(0)			// a[1][1]*cos+a[0][2]*sin, a[0][1]*cos+a[1][2]*sin, cos, sin
		fstp	dword ptr [edx+8]	// a[0][1]*cos+a[1][2]*sin, cos, sin
		fstp	dword ptr [edx+24]	// cos, sin
		fld		st(1)				// sin, cos, sin
		fld		st(1)				// cos, sin, cos, sin
		fld		st(1)				// sin, cos, sin, cos, sin
		fld		st(1)				// cos, sin, cos, sin, cos, sin
		fmul	dword ptr [eax+36]	// a[2][1]*cos, sin, cos, sin, cos, sin
		fxch	st(1)				// sin, a[0][1]*cos, cos, sin, cos, sin
		fmul	dword ptr [eax+40]	// a[2][2]*sin, a[2][1]*cos, cos, sin, cos, sin
		fxch	st(2)				// cos, a[2][2]*sin, a[2][1]*cos, sin, cos, sin
		fmul	dword ptr [eax+52]	// a[3][1]*cos, a[2][2]*sin, a[2][1]*cos, sin, cos, sin
		fxch	st(3)				// sin, a[1][1]*cos, a[0][2]*sin, a[0][1]*cos, cos, sin
		fmul	dword ptr [eax+56]	// a[3][2]*sin, a[3][1]*cos, a[2][2]*sin, a[2][1]*cos, cos, sin
		fxch	st(1)				// a[2][2]*sin, a[3][2]*sin, a[1][1]*cos, a[2][1]*cos, cos, sin
		faddp	st(2),st(0)			// a[3][2]*sin, a[3][1]*cos+a[2][2]*sin, a[2][1]*cos, cos, sin
		faddp	st(2),st(0)			// a[3][1]*cos+a[2][2]*sin, a[2][1]*cos+a[3][2]*sin, cos, sin
		fstp	dword ptr [edx+40]	// a[2][1]*cos+a[3][2]*sin, cos, sin
		fstp	dword ptr [edx+56]	// cos, sin
		pop		edx
		pop		ebx
		pop		eax
	}
	return(lpDst);
}

/*
 * lpDst = lpSrc1 * lpSrc2
 * lpDst cannot be equal to lpSrc1 or lpSrc2
 */
LPD3DMATRIX MultiplyD3DMATRIX(LPD3DMATRIX lpDst, LPD3DMATRIX lpSrc1, 
                              LPD3DMATRIX lpSrc2)
{
/*
*/
    ZeroMemory( lpDst, sizeof(D3DMATRIX) );
	float *pA=(float *)lpSrc1;
	float *pB=(float *)lpSrc2;
	float *pM=(float *)lpDst;


    for( WORD i=0; i<4; i++ ) 
        for( WORD j=0; j<4; j++ ) 
            for( WORD k=0; k<4; k++ ) 
                pM[4*i+j] += pA[4*k+j] * pB[4*i+k];
	return lpDst;
	__asm
	{
		push	ebx
		push	ecx
		push	edx

		mov		ecx,4
		mov		eax,dword ptr lpSrc1
		mov		ebx,dword ptr lpSrc2
		mov		edx,dword ptr lpDst

matrix_loop:
		fld		dword ptr [eax]			//a00
		fmul	dword ptr [ebx]			//a00*b00
		fld		dword ptr [eax+4]		//a01,a00*b00
		fmul	dword ptr [ebx+16]		//a01*b10,a00*b00
		fld		dword ptr [eax+8]		//a02,a01*b10,a00*b00
		fmul	dword ptr [ebx+32]		//a02*b20,a01*b10,a00*b00
		fld		dword ptr [eax+12]		//a03,a02*b20,a01*b10,a00*b00
		fmul	dword ptr [ebx+48]		//a03*b30,a02*b20,a01*b10,a00*b00
		fxch	st(2)					//a01*b10,a02*b20,a03*b30,a00*b00
		faddp	st(3),st(0)				//a01*b10+a00*b00,a02*b20,a03*b30
		faddp	st(1),st(0)				//a01*b10+a00*b00+a02*b20,a03*b30
		faddp	st(1),st(0)				//a01*b10+a00*b00+a02*b20+a03*b30
		fstp	dword ptr [edx]			//c00=a01*b10+a00*b00+a02*b20+a03*b30

		fld		dword ptr [eax]
		fmul	dword ptr [ebx+4]
		fld		dword ptr [eax+4]
		fmul	dword ptr [ebx+20]
		fld		dword ptr [eax+8]
		fmul	dword ptr [ebx+36]
		fld		dword ptr [eax+12]	
		fmul	dword ptr [ebx+52]
		fxch	st(2)
		faddp	st(3),st(0)
		faddp	st(1),st(0)
		faddp	st(1),st(0)
		fstp	dword ptr [edx+4]

		fld		dword ptr [eax]
		fmul	dword ptr [ebx+8]
		fld		dword ptr [eax+4]
		fmul	dword ptr [ebx+24]
		fld		dword ptr [eax+8]
		fmul	dword ptr [ebx+40]
		fld		dword ptr [eax+12]	
		fmul	dword ptr [ebx+56]
		fxch	st(2)
		faddp	st(3),st(0)
		faddp	st(1),st(0)
		faddp	st(1),st(0)
		fstp	dword ptr [edx+8]

		fld		dword ptr [eax]
		fmul	dword ptr [ebx+12]
		fld		dword ptr [eax+4]
		fmul	dword ptr [ebx+28]
		fld		dword ptr [eax+8]
		fmul	dword ptr [ebx+44]
		fld		dword ptr [eax+12]	
		fmul	dword ptr [ebx+60]
		fxch	st(2)
		faddp	st(3),st(0)
		faddp	st(1),st(0)
		faddp	st(1),st(0)
		fstp	dword ptr [edx+12]

		add		edx,16
		add		eax,16
		dec		ecx
		cmp		ecx,0
		jnz		matrix_loop

		pop		edx
		pop		ebx
		pop		eax

	}
    return lpDst;
}

#pragma optimize("",on)

/*
 * -1 d = a
 */
LPD3DMATRIX 
D3DMATRIXInvert(LPD3DMATRIX d, LPD3DMATRIX a)
{
    d->_11 = a->_11;
    d->_12 = a->_21;
    d->_13 = a->_31;
    d->_14 = a->_14;

    d->_21 = a->_12;
    d->_22 = a->_22;
    d->_23 = a->_32;
    d->_24 = a->_24;

    d->_31 = a->_13;
    d->_32 = a->_23;
    d->_33 = a->_33;
    d->_34 = a->_34;

    d->_41 = a->_14;
    d->_42 = a->_24;
    d->_43 = a->_34;
    d->_44 = a->_44;

    return d;
}


/*
 * Set the rotation part of a matrix such that the vector lpD is the new
 * z-axis and lpU is the new y-axis.
 */
LPD3DMATRIX 
D3DMATRIXSetRotation(LPD3DMATRIX lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU)
{
    float t;
    D3DVECTOR d, u, r;

    /*
     * Normalise the direction vector.
     */
    d.x = lpD->x;
    d.y = lpD->y;
    d.z = lpD->z;
    D3DVECTORNormalise(&d);

    u.x = lpU->x;
    u.y = lpU->y;
    u.z = lpU->z;
    /*
     * Project u into the plane defined by d and normalise.
     */
    t = u.x * d.x + u.y * d.y + u.z * d.z;
    u.x -= d.x * t;
    u.y -= d.y * t;
    u.z -= d.z * t;
    D3DVECTORNormalise(&u);

    /*
     * Calculate the vector pointing along the matrix x axis (in a right
     * handed coordinate system) using cross product.
     */
    D3DVECTORCrossProduct(&r, &u, &d);

    lpM->_11 = r.x;
    lpM->_12 = r.y, lpM->_13 = r.z;
    lpM->_21 = u.x;
    lpM->_22 = u.y, lpM->_23 = u.z;
    lpM->_31 = d.x;
    lpM->_32 = d.y;
    lpM->_33 = d.z;

    return lpM;
}

/*
 * Calculates a point along a B-Spline curve defined by four points. p
 * n output, contain the point. t                                Position
 * along the curve between p2 and p3.  This position is a float between 0
 * and 1. p1, p2, p3, p4    Points defining spline curve. p, at parameter
 * t along the spline curve
 */
void 
spline(LPD3DVECTOR p, float t, LPD3DVECTOR p1, LPD3DVECTOR p2,
       LPD3DVECTOR p3, LPD3DVECTOR p4)
{
    double t2, t3;
    float m1, m2, m3, m4;

    t2 = (double)(t * t);
    t3 = t2 * (double)t;

    m1 = (float)((-1.0 * t3) + (2.0 * t2) + (-1.0 * (double)t));
    m2 = (float)((3.0 * t3) + (-5.0 * t2) + (0.0 * (double)t) + 2.0);
    m3 = (float)((-3.0 * t3) + (4.0 * t2) + (1.0 * (double)t));
    m4 = (float)((1.0 * t3) + (-1.0 * t2) + (0.0 * (double)t));

    m1 /= (float)2.0;
    m2 /= (float)2.0;
    m3 /= (float)2.0;
    m4 /= (float)2.0;

    p->x = p1->x * m1 + p2->x * m2 + p3->x * m3 + p4->x * m4;
    p->y = p1->y * m1 + p2->y * m2 + p3->y * m3 + p4->y * m4;
    p->z = p1->z * m1 + p2->z * m2 + p3->z * m3 + p4->z * m4;
}

void ConcatenateXRotation(LPD3DMATRIX lpM, float Radians )
{
  float aElements[4][4];

  float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

  memcpy(aElements, lpM, sizeof(D3DMATRIX));
  lpM->_12 = aElements[0][1] * Cos + aElements[0][2] * Sin;
  lpM->_22 = aElements[1][1] * Cos + aElements[1][2] * Sin;
  lpM->_32 = aElements[2][1] * Cos + aElements[2][2] * Sin;
  lpM->_42 = aElements[3][1] * Cos + aElements[3][2] * Sin;

  lpM->_13 = aElements[0][1] * -Sin + aElements[0][2] * Cos;
  lpM->_23 = aElements[1][1] * -Sin + aElements[1][2] * Cos;
  lpM->_33 = aElements[2][1] * -Sin + aElements[2][2] * Cos;
  lpM->_43 = aElements[3][1] * -Sin + aElements[3][2] * Cos;

}


void ConcatenateYRotation(LPD3DMATRIX lpM, float Radians )
{
  float aElements[4][4];

  float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

  memcpy(aElements, lpM, sizeof(D3DMATRIX));
  lpM->_11 = aElements[0][0] * Cos + aElements[0][2] * -Sin;
  lpM->_21 = aElements[1][0] * Cos + aElements[1][2] * -Sin;
  lpM->_31 = aElements[2][0] * Cos + aElements[2][2] * -Sin;
  lpM->_41 = aElements[3][0] * Cos + aElements[3][2] * -Sin;

  lpM->_13 = aElements[0][0] * Sin + aElements[0][2] * Cos;
  lpM->_23 = aElements[1][0] * Sin + aElements[1][2] * Cos;
  lpM->_33 = aElements[2][0] * Sin + aElements[2][2] * Cos;
  lpM->_43 = aElements[3][0] * Sin + aElements[3][2] * Cos;
}


void ConcatenateZRotation(LPD3DMATRIX lpM, float Radians )
{
  float aElements[4][4];

  float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

  memcpy(aElements, lpM, sizeof(D3DMATRIX));
  lpM->_11 = aElements[0][0] * Cos + aElements[0][1] * Sin;
  lpM->_21 = aElements[1][0] * Cos + aElements[1][1] * Sin;
  lpM->_31 = aElements[2][0] * Cos + aElements[2][1] * Sin;
  lpM->_41 = aElements[3][0] * Cos + aElements[3][1] * Sin;

  lpM->_12 = aElements[0][0] * -Sin + aElements[0][1] * Cos;
  lpM->_22 = aElements[1][0] * -Sin + aElements[1][1] * Cos;
  lpM->_32 = aElements[2][0] * -Sin + aElements[2][1] * Cos;
  lpM->_42 = aElements[3][0] * -Sin + aElements[3][1] * Cos;
}

}
