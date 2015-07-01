
/*
 * Copyright 2008 Gabriele Lombardi
 * 
 *
 *   This file is part of OpenTVF.
 *
 *   OpenTVF is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   OpenTVF is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with OpenTVF.  If not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef _MATHMATRIXES_H_
#define _MATHMATRIXES_H_

/*
 *  This lib declares a set of functions that allows to
 * manipulate matrixes and vectors without iterating on them.
 */

// Useful libs:
#include "mathExt.h"

// LAPACK's used functions:
#include <lapack/ssyevr.h>
#include <lapack/dsyevr.h>
#include <lapack/sgelss.h>
#include <lapack/dgelss.h>

////////////
// Macros //
////////////

// Dimensionality of the matrix:
#define MAT_DIM(__dimr,__dimc) (((vector_ind_t)(__dimr))*(__dimc))
#define MAT_DOF_NUM(__dimr,__dimc) (MAT_DIM((__dimr),(__dimc)))

// Indexing:
#define MAT_IND_ROW(__ind,__dimr,__dimc) ((__ind)%(__dimr))
#define MAT_IND_COL(__ind,__dimr,__dimc) ((__ind)/(__dimr))
#define MAT_IND_BOTH(__ind,__dimr,__dimc,__row,__col) \
										(__row) = MAT_IND_ROW((__ind),(__dimr),(__dimc)); \
										(__col) = MAT_IND_COL((__ind),(__dimr),(__dimc));
#define MAT_IND(__row,__col,__dimr,__dimc) (((vector_ind_t)(__dimr))*(__col)+(__row))


// Iterating:
#define VEC_ITERATE(__ind,__dim) \
	for ((__ind)=0; (__ind)<(__dim); (__ind)++)
#define MAT_ITERATE_H(__row,__col,__ind,__dimr,__dimc) \
	for ((__ind)=0,(__row)=0; (__row)<(__dimr); (__row)++,(__ind)=(__row)) \
		for ((__col)=0; (__col)<(__dimc); (__col)++,(__ind)+=(__dimr))
#define MAT_ITERATE_V(__row,__col,__ind,__dimr,__dimc) \
	for ((__ind)=0,(__col)=0; (__col)<(__dimc); (__col)++) \
		for ((__row)=0; (__row)<(__dimr); (__row)++,(__ind)++)
#define MAT_ITERATE(__ind,__dimr,__dimc) \
	for (volatile vector_ind_t __tot=MAT_DIM((__dimr),(__dimc)),(__ind)=0; (__ind)<__tot; (__ind)++)


// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Math {

		// Lapack templated declarations:
		template <class T>
		int syevr(	char *jobz, char *range, char *uplo, integer *n, 
					T *a, integer *lda, T *vl, T *vu, integer *il, integer *iu, 
					T *abstol, integer *m, T *w, T *z__, integer *ldz, integer *isuppz, 
					T *work, integer *lwork, integer *iwork, integer *liwork, integer *info);
		template <class T>
		int gelss(integer *m, integer *n, integer *nrhs, T *a, integer *lda, T *b, integer *ldb,
				T *s, T *rcond, integer *rank, T *work, integer *lwork, integer *info);


		////////////////
		// Data types //
		////////////////

		// The vector dimensionality:
		typedef unsigned short vector_dim_t;
		typedef signed short vector_dof_t;
		typedef unsigned long vector_ind_t;
		typedef signed long vector_iof_t;


		///////////////
		// Functions //
		///////////////

		// --------------------------------------------------------------------------
		// VECTORS
		// --------------------------------------------------------------------------

		// Generating a vector:
		template<class T>
		inline T *vec_create(vector_ind_t dim) {
			// Allocating a new vector:
			return new T[dim];
		}

		// Generating a vector:
		template<class T>
		inline T *vec_create_init(T *destVec,T val,vector_ind_t dim) {
			// Allocating a new vector:
			vector_ind_t i;
			if (destVec==NULL) destVec = vec_create<T>(dim);

			// Setting all to val:
			VEC_ITERATE(i,dim) {
				// Saving val:
				destVec[i] = val;
			}

			// Returning:
			return destVec;
		}

		// Generating a vector clone:
		template<class T>
		inline T *vec_create_clone(T *destVec,const T* v,vector_ind_t dim) {
			// Allocating a new vector:
			if (destVec==NULL) destVec = vec_create<T>(dim);

			// Cloning:
			memcpy(destVec,v,dim*sizeof(T));

			// Returning:
			return destVec;
		}

		// Freeing:
		template<class T>
		inline void vec_free(T* vec) {
			// Freeing the unused memory:
			if (vec!=NULL) delete vec;
		}

		// Vectors accumulation:
		template<class T>
		inline T *vec_accumulate(T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Adding:
				v1[i] += v2[i];
			}

			// Returning:
			return v1;
		}

		// Vectors difference with scaling of v2 (v1 = v1 + s*v2):
		template<class T>
		inline T *vec_accumulate_scaled(T* v1,const T* v2,T s,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Accumulating:
				v1[i] += v2[i]*s;
			}

			// Returning:
			return v1;
		}

		// Vectors accumulation:
		template<class T>
		inline T *vec_add(T* v1,T val,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Adding:
				v1[i] += val;
			}

			// Returning:
			return v1;
		}

		// Vectors difference:
		template<class T>
		inline T *vec_subtract(T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Subtracting:
				v1[i] -= v2[i];
			}

			// Returning:
			return v1;
		}

		// Vectors difference with scaling of v2 (v1 = v1 - s*v2):
		template<class T>
		inline T *vec_subtract_scaled(T* v1,const T* v2,T s,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Subtracting:
				v1[i] -= v2[i]*s;
			}

			// Returning:
			return v1;
		}

		// Vectors multiplication:
		template<class T>
		inline T *vec_multiply(T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Multiply:
				v1[i] *= v2[i];
			}

			// Returning:
			return v1;
		}

		// Vectors multiplication:
		template<class T>
		inline T *vec_scale(T* v1,T val,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Multiply:
				v1[i] *= val;
			}

			// Returning:
			return v1;
		}

		// Vectors ratio:
		template<class T>
		inline T *vec_divide(T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Dividing:
				v1[i] /= v2[i];
			}

			// Returning:
			return v1;
		}

		// Vectors internal product:
		template<class T>
		inline T vec_dot(const T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = 0;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Sum of products:
				res += v1[i]*v2[i];
			}

			// Returning:
			return res;
		}

		// Vector norm L1:
		template<class T>
		inline T vec_norm_L1(const T* v,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = 0;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Adding:
				res += ABS(v[i]);
			}

			// Returning:
			return res;
		}

		// Vector norm L2:
		template<class T>
		inline T vec_norm_L2(const T* v,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = 0;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Sum of squares:
				res += v[i]*v[i];
			}

			// Returning:
			return (T)sqrt(res);
		}

		// Vector norm L2 squared:
		template<class T>
		inline T vec_norm_L2_squared(const T* v,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = 0;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Sum of squares:
				res += v[i]*v[i];
			}

			// Returning:
			return res;
		}

		// Vector norm Linf:
		template<class T>
		inline T vec_norm_Linf(const T* v,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = v[0];

			// Iterating:
			for (i=1; i<dim; i++) {
				// Max value:
				res = res>v[i]? res: v[i];
			}

			// Returning:
			return res;
		}

		// Vector distance in L1:
		template<class T>
		inline T vec_dist_L1(const T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = 0;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Adding:
				res += ABS(v1[i]-v2[i]);
			}

			// Returning:
			return res;
		}

		// Vector distance in L2:
		template<class T>
		inline T vec_dist_L2(const T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = 0,val;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Sum of squares:
				val = v1[i]-v2[i];
				res += val*val;
			}

			// Returning:
			return (T)sqrt(res);
		}

		// Vector distance in L2 squared:
		template<class T>
		inline T vec_dist_L2_squared(const T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = 0,val;

			// Iterating:
			VEC_ITERATE(i,dim) {
				// Sum of squares:
				val = v1[i]-v2[i];
				res += val*val;
			}

			// Returning:
			return res;
		}

		// Vector distance in Linf:
		template<class T>
		inline T vec_dist_Linf(const T* v1,const T* v2,vector_ind_t dim) {
			// Indexes:
			vector_ind_t i;
			T res = v1[0]-v2[0],val;

			// Iterating:
			for (i=1; i<dim; i++) {
				// Max value:
				val = v1[i]-v2[i];
				res = res>val? res: val;
			}

			// Returning:
			return res;
		}

		// Point in ball (the radius R2 is squared):
		template<class T>
		inline bool vec_dist_L2_leq(const T* v1,const T* v2,T R2,vector_ind_t dim) {
			// The accumulated distance squared and a support variable:
			T dist2 = 0, val;

			// Indexes:
			vector_ind_t i;

			// Iterating on coordinates and accumulating:
			for (i=0; i<dim; i++) {
				// Computing and adding the squared distance on axis i:
				val = v2[i] - v1[i];
				dist2 += val*val;

				// Checking:
				if (dist2>R2) {
					// The answer is NO:
					return false;
				}
			}

			// The answer is yes:
			return true;
		}

		// Finding the index of the minimum value:
		template<class T>
		inline vector_ind_t vec_min(const T* v,vector_ind_t dim) {
			// By hipothesis the minimum is the first:
			vector_ind_t res = 0,i;

			// Iterating:
			for (i=1; i<dim; i++) {
				// Check:
				if (v[i]<v[res]) res = i;
			}

			// Returning:
			return res;
		}

		// Finding the index of the maximum value:
		template<class T>
		inline vector_ind_t vec_max(const T* v,vector_ind_t dim) {
			// By hipothesis the maximum is the first:
			vector_ind_t res = 0,i;

			// Iterating:
			for (i=1; i<dim; i++) {
				// Check:
				if (v[i]>v[res]) res = i;
			}

			// Returning:
			return res;
		}

		// Finding the indexes of the minimum and maximum values:
		template<class T>
		inline void vec_min_max(const T* v,vector_ind_t &min, vector_ind_t &max,vector_ind_t dim) {
			// By hipothesis the maximum is the first:
			vector_ind_t i;

			// Iterating:
			for (min=0,max=0,i=1; i<dim; i++) {
				// Check:
				if (v[i]<v[min]) min = i;
				if (v[i]>v[max]) max = i;
			}
		}

		// Angle between two vectors:
		template<class T>
		inline T vec_angle(const T* v1,const T* v2,vector_ind_t dim) {
			// The resulting angle:
			return acos(vec_dot(v1,v2,dim)/(vec_norm_L2(v1,dim)*vec_norm_L2(v2,dim)));
		}

		/*
		 *  Computing the curvature and the arc length of a circumference arc passing
		 * through the origin and a given point and aligned with an ord base tensor.
		 *	IN:
		 *		p:		The point.
		 *		ord:	The order of the base tensor.
		 *		dim:	The dimensionality of the space.
		 *	OUT:
		 *		s:		The arc length.
		 *		k:		The curvature.
		 *		length:	The circular section length.
		 *		theta:	The external circular section angle.
		 */
		template<class T>
		inline void vec_circ_curvature_and_length(
				// In parameters:
				const T* p,vector_dim_t ord,vector_dim_t dim,
				// Out parameters:
				T &s, T &k, T &length, T &theta) {
			// Checking the order:
			ord = ord%dim;
			if (ord==0) {
				// Length as distance:
				s = length = vec_norm_L2(p,dim);

				// No curvature:
				k = theta = 0;
			}
			else {
				// Local useful values:
				T st;

				// The uncompressed/compressed parts (in this order):
				T ph[2];

				// Computing the uncompressed and compressed parts:
				ph[0] = vec_norm_L2(p,dim-ord);
				ph[1] = vec_norm_L2(p+dim-ord,ord);

				// Computing the length:
				length = vec_norm_L2(ph,2);

				// Computing the angle:
				theta = atan2(ph[1],ph[0]);

				// Computing the arc length and the curvature:
				if (theta<(T)EPS && theta>-(T)EPS) {
					// Exceptional case with s==length and k==0:
					s = length;
					k = 0;
				}
				else {
					// Normal case:
					st = sin(theta);
					st = st<(T)EPS && st>-(T)EPS? (T)EPS: st;
					s = (theta*length)/st;
					k = 2*st/length;
				}
			}
		}

		/*
		*  Angle between a vector and its projection over a linear subspace
		* obtained zeroing dim-ord (uncompressed) initial components:
		*	v:		The vector in Rn.
		*	ord:	 The number of values not to be set to zero in the tralinig coordinates
		*			of v to get the projection.
		*  dim:	 The dimensionality of the space.
		*/
		template<class T>
		inline T vec_angle_linproj_zeroing(const T* v,vector_ind_t ord,vector_ind_t dim) {
			// The resulting angle:
			return atan2(vec_norm_L2(v,dim-ord),vec_norm_L2(v+dim-ord,ord));
		}

		/*
		*  Angle between a vector and its projection over a linear subspace
		* obtained zeroing ord (compressed) trailing components:
		*	v:		The vector in Rn.
		*	ord:	 The number of values to be set to zero in the tralinig coordinates
		*			of v to get the projection.
		*  dim:	 The dimensionality of the space.
		*/
		template<class T>
		inline T vec_angle_linproj_zeroing_inverse(const T* v,vector_ind_t ord,vector_ind_t dim) {
			// The resulting angle:
			return atan2(vec_norm_L2(v+dim-ord,ord),vec_norm_L2(v,dim-ord));
		}

		// Normalization given the norm:
		template<class T>
		inline void vec_normalize(T* v,vector_ind_t dim,T (*norm)(const T*,vector_ind_t)) {
			// Computing the norm:
			volatile T n = norm(v,dim);

			// Normalizing:
			if (n!=0 && n!=1) {
				vec_scale(v,1/n,dim);
			}
		}

		/* 
		*  Computing the tangent vector to the osculating circle. The returned vector is
		* normalized.
		*	p:		The position of the tangent wrt the centered and unoriented tensor.
		*	dest:	The destination vector.
		*	ord:	The order (number of compressed dimensions as trailing part of vectors).
		*	dim:	The dimensionality.
		*/
		template <class T>
		T *vec_create_tangent(const T* p,T *dest,vector_ind_t ord,vector_ind_t dim) {
			// Computing the angle:
			T theta = 2*vec_angle_linproj_zeroing_inverse(p,ord,dim);

			// Creating the destination vector:
			dest = vec_create_clone(dest,p,dim);

			// Normalizing the projections of the point on compressed ad uncompressed dimensions:
			vec_normalize(dest,dim-ord,vec_norm_L2<T>);
			vec_normalize(dest+dim-ord,ord,vec_norm_L2<T>);

			// Scaling the projections to obtain the desidered vector:
			vec_scale(dest,cos(theta),dim-ord);
			vec_scale(dest+dim-ord,sin(theta),ord);

			// Returning:
			return dest;
		}

		/*
		*  This function allows to orthogonalize (in place) the vector v2 wrt
		* the NORMAL vector v1. The norm of the vector v2 is not preserved.
		* If the two vectors are linearly dependent the resulting vector is
		+ zero.
		*/
		template<class T>
		inline void vec_orthogonalize(const T* v1,T* v2,vector_ind_t dim) {
			// Computing the projection of v2 in the v1's direction:
			volatile T proj = vec_dot(v1,v2,dim);

			// Removing the scaled version of v1 from v2:
			vec_subtract_scaled(v2,v1,proj,dim);
		}

		/*
		*  This function allows to project a point onto a normal base (orthogonality
		* is not strictly required but generates vectors in an affinely distorted space).
		* The base is defined with a dimxN matrix V of normal column vectors.
		*	v:	The point.
		*	V:	The subspace base.
		*/
		template<class T>
		void vec_proj_normal(const T* v,const T* V,T* dest,vector_ind_t N,vector_ind_t dim) {
			// The projection dot product:
			T dot;
			const T *Vp = V;

			// Allocating the destination:
			if (dest==NULL) {
				dest = vec_create<T>(dim);
			}

			// Initialization:
			memset(dest,0,dim*sizeof(T));

			// Iterating on the base:
			for (vector_ind_t i=0; i<N; i++,Vp+=dim) {
				// Dot for the first vector:
				dot = vec_dot(v,Vp,dim);

				// Adding the component:
				vec_accumulate_scaled(dest,Vp,dot,dim);

			}
		}

		/*
		*  This function implements the Graham-Schmidt orthonormalization procedure
		* of a set of column vectors stored in a matrix. The orthogonalization is
		* done on all the vectors from the i-th one to the last one. The vectors
		* in the columns range 0:i-1 are assumed to be yet orthonormals. The
		* procedure is executed in place. The index i denote the starting point
		* and the index j denotes the ending one, the extremas are inclusive.
		*/
		template<class T>
		void vec_graham_schmidt(T* vs,vector_dim_t i,vector_dim_t j,vector_ind_t dim) {
			// Indexes:
			vector_dim_t k, l;

			// Vectors:
			T *vk = vs + dim*i, *vl;

			// Iterating on vectors from i to j:
			for (k=i; k<=j; k++) {
				// This vector must be orthogonalized wrt all the others:
				for (l=0,vl=vs; l<k; l++,vl+=dim) {
					// Orthogonalization:
					vec_orthogonalize(vl,vk,dim);
				}

				// Normalization:
				vec_normalize(vk,dim,vec_norm_L2<T>);

				// The next vector:
				vk += dim;
			}
		}

		// Printing a vector:
		template<class T>
		void vec_fprint(FILE *fp,const T *vec,vector_dim_t dim) {
			// Indexes:
			vector_ind_t i;

			// Iterating on rows:
			VEC_ITERATE(i,dim) {
				// Showing the value:
				fprintf(fp,"\t%f",vec[i]);
			}

			// Adding a newline:
			fprintf(fp,"\n");
		}


		// --------------------------------------------------------------------------
		// MATRIXES
		// --------------------------------------------------------------------------

		// Creation of an empty matrix:
		template<class T>
		inline T *mat_create(vector_dim_t dimr,vector_dim_t dimc) {
			// Creating the matrix's vector and returning:
			return vec_create<T>(MAT_DIM(dimr,dimc));
		}

		// Creation of an initializied matrix:
		template<class T>
		inline T *mat_create_init(T *destMat,T val,vector_dim_t dimr,vector_dim_t dimc) {
			// Creating the matrix's vector:
			vector_ind_t tot=MAT_DIM(dimr,dimc),i;
			if (destmat==NULL) destMat = mat_create<T>(dim,dim);

			// Iterating in the matrix:
			VEC_ITERATE(i,tot) {
				// Saving the right value:
				destMat[i] = val;
			}

			// Returning:
			return destMat;
		}

		// Identity matrix:
		template<class T>
		inline T *mat_create_identity(T *destMat,vector_dim_t dim) {
			// Creating the matrix's vector:
			vector_dim_t r,c;
			vector_ind_t i;

			// Allocating the matrix:
			if (destMat==NULL) destMat = destMat = mat_create<T>(dim,dim);

			// Iterating in the matrix:
			MAT_ITERATE_V(r,c,i,dim,dim) {
				// Saving the right value:
				destMat[i] = r==c? (T)1: (T)0;
			}

			// Returning:
			return destMat;
		}

		// Generating a matrix clone:
		template<class T>
		inline T *mat_create_clone(T *destMat,T* mat,vector_dim_t dimr,vector_dim_t dimc) {
			// Cloning the content as a vector:
			return vec_create_clone(destMat,mat,MAT_DIM(dimr,dimc));
		}

		// Freeing a matrix:
		template<class T>
		inline void mat_free(T *mat) {
			// Freeing the associated vector:
			vec_free(mat);
		}

		// Printing a matrix:
		template<class T>
		void mat_fprint(FILE *fp,T *mat,vector_dim_t dimr,vector_dim_t dimc) {
			// Indexes:
			vector_dim_t r,c;
			vector_ind_t i;

			// Iterating on rows:
			MAT_ITERATE_H(r,c,i,dimr,dimc) {
				// Showing the value:
				fprintf(fp,"\t%f",mat[i]);

				// The end of line:
				if (c>=dimc-1) {
					// Adding a newline:
					fprintf(fp,"\n");
				}
			}
		}

		// Algebraic operations for matrixes:
		#define mat_accumulate(__mat1,__mat2,__dimr,__dimc	)	vec_accumulate((__mat1),(__mat2),MAT_DIM((__dimr),(__dimc)))
		#define mat_add(__mat1,__val,__dimr,__dimc)				vec_add((__mat1),(__val),MAT_DIM((__dimr),(__dimc)))
		#define mat_subtract(__mat1,__mat2,__dimr,__dimc)		vec_subtract((__mat1),(__mat2),MAT_DIM((__dimr),(__dimc)))
		#define mat_scale(__mat1,__val,__dimr,__dimc)			vec_scale((__mat1),(__val),MAT_DIM((__dimr),(__dimc)))
		#define mat_multiply_v(__mat1,__mat2,__dimr,__dimc)		vec_multiply((__mat1),(__mat2),MAT_DIM((__dimr),(__dimc)))
		#define mat_divide(__mat1,__mat2,__dimr,__dimc)			vec_divide((__mat1),(__mat2),MAT_DIM((__dimr),(__dimc)))
		#define mat_prod_internal(__mat1,__mat2,__dimr,__dimc)	vec_internal((__mat1),(__mat2),MAT_DIM((__dimr),(__dimc)))
		#define mat_norm_L1(__mat,__dimr,__dimc)				vec_norm_L1((__mat),MAT_DIM((__dimr),(__dimc)))
		#define mat_norm_L2										vec_norm_L2((__mat),MAT_DIM((__dimr),(__dimc)))
		#define mat_norm_Linf									vec_norm_Linf((__mat),MAT_DIM((__dimr),(__dimc)))

		/*
		* Matrix multiplication.
		*  (destination matrix preallocated or NULL to force the allocation)
		* (lxm)*(mxn) = (lxn)
		* TODO: Use the BLAS library here!!!
		*/
		template<class T>
		T* mat_multiply(const T *mat1,const T *mat2,T *matDest,
						vector_dim_t l,vector_dim_t m,vector_dim_t n) {
			// Indexes:
			vector_ind_t i,i2,j,j2,k,k2;

			// Allocation:
			if (matDest==NULL) {
				// Destination must be l*n:
				matDest = mat_create<T>(l,n);
			}

			// Iterating on the rows of mat1:
			for(i=0; i<l; i++) {
				// Iterating on the columns of mat2:
				for(j=0,i2=i,k2=0; j<n; j++,i2+=l) {
					// Initializing the resulting value:
					matDest[i2] = (T)0;

					// Iterating on the columns of mat1:
					for(k=0,j2=i; k<m; k++,k2++,j2+=l) {
						// Adding:
						matDest[i2] += mat1[j2]*mat2[k2];
					}
				}
			}

			// Returning:
			return matDest;
		}

		/*
		* Matrix multiplication with transposition.
		*  (destination matrix preallocated or NULL to force the allocation)
		* (lxm)*(mxn) = (lxn)
		* where this dimensions are referred to the transposed matrixes accordingly to
		* the selected transposition.
		*  The parameter "trans" identifies the transposition with the following values:
		*	1:		Transpose first.
		*	2:		Transpose second.
		*	3:		Transpose result.
		*	other:	No transposition.
		* TODO: Use the BLAS library here!!!
		*/
		template<class T>
		T* mat_multiply_transpose(	const T *mat1,const T *mat2,T *matDest,short trans,
									vector_dim_t l,vector_dim_t m,vector_dim_t n) {
			// Indexes:
			vector_ind_t i,i2,j,j2,k,k2;

			// Allocation:
			if (matDest==NULL) {
				// Destination must be l*n:
				matDest = mat_create<T>(l,n);
			}

			// Checking the transposition type:
			switch(trans) {
				case 1:	// Transposing the first matrix:
					// Iterating on the cols of mat1:
					for(i=0,j2=0; i<l; i++,j2+=m) {
						// Iterating on the columns of mat2:
						for(j=0,i2=i,k2=0; j<n; j++,i2+=l,j2-=m) {
							// Initializing the resulting value:
							matDest[i2] = (T)0;

							// Iterating on the rows of mat1:
							for(k=0; k<m; k++,k2++,j2++) {
								// Adding:
								matDest[i2] += mat1[j2]*mat2[k2];
							}
						}
					}
					break;
				case 2:	// Transposing the second matrix:
					// Iterating on the rows of mat1:
					for(i=0; i<l; i++) {
						// Iterating on the columns of mat2:
						for(j=0,i2=i; j<n; j++,i2+=l) {
							// Initializing the resulting value:
							matDest[i2] = (T)0;

							// Iterating on the columns of mat1:
							for(k=0,j2=i,k2=j; k<m; k++,k2+=n,j2+=l) {
								// Adding:
								matDest[i2] += mat1[j2]*mat2[k2];
							}
						}
					}
					break;
				case 3:	// Transposing the resulting matrix:
					// Iterating on the rows of mat1:
					for(i=0,i2=0; i<l; i++) {
						// Iterating on the columns of mat2:
						for(j=0,k2=0; j<n; j++,i2++) {
							// Initializing the resulting value:
							matDest[i2] = (T)0;

							// Iterating on the columns of mat1:
							for(k=0,j2=i; k<m; k++,k2++,j2+=l) {
								// Adding:
								matDest[i2] += mat1[j2]*mat2[k2];
							}
						}
					}
					break;
				default:
					// No transposition:
					mat_multiply(mat1,mat2,matDest,l,m,n);
			}

			// Returning:
			return matDest;
		}

		/*
		* Matrix multiplication with the first(second) diagonal.
		*  (destination matrix preallocated or NULL to force the allocation)
		* (lxm)*(mxn) = (lxn)
		* TODO: Use the BLAS library here!!!
		*/
		template<class T>
		T* mat_multiply_diagonal(const T *mat,const T *diag,T *matDest,
								vector_dim_t l,vector_dim_t m,vector_dim_t n,
								bool firstDiagonal) {
			// Indexes:
			vector_ind_t r,c,i,extr;

			// Allocation:
			if (matDest==NULL) {
				// Destination must be l*n:
				matDest = mat_create<T>(l,n);
			}

			// Selecting the case:
			if (firstDiagonal) {
				// Iterating on the dest matrix:
				extr = min(l,m);
				MAT_ITERATE_V(r,c,i,l,n) {
					// The actual row must be scaled:
					matDest[i] = r>=extr? 0: diag[r]*mat[MAT_IND(r,c,m,n)];
				}
			}
			else {
				// Iterating on the dest matrix:
				extr = MIN(m,n);
				MAT_ITERATE_V(r,c,i,l,n) {
					// The actual row must be scaled:
					matDest[i] = c>=extr? 0: mat[MAT_IND(r,c,l,m)]*diag[c];
				}
			}

			// Returning:
			return matDest;
		}

		/*
		* Matrix multiplication with the first(second) diagonal with transposition.
		*  (destination matrix preallocated or NULL to force the allocation)
		* (lxm)*(mxn) = (nxl)
		* TODO: Use the BLAS library here!!!
		*/
		template<class T>
		T* mat_multiply_diagonal_transpose(
									const T *mat,const T *diag,T *matDest,
									vector_dim_t l,vector_dim_t m,vector_dim_t n,
									bool firstDiagonal) {
			// Indexes:
			vector_ind_t r,c,i,extr;

			// Allocation:
			if (matDest==NULL) {
				// Destination must be n*l:
				matDest = mat_create<T>(n,l);
			}

			// Selecting the case:
			if (firstDiagonal) {
				// Iterating on the dest matrix:
				extr = min(l,m);
				MAT_ITERATE_V(c,r,i,n,l) {
					// The actual row must be scaled:
					matDest[i] = r>=extr? 0: diag[r]*mat[MAT_IND(r,c,m,n)];
				}
			}
			else {
				// Iterating on the dest matrix:
				extr = MIN(m,n);
				MAT_ITERATE_V(c,r,i,n,l) {
					// The actual row must be scaled:
					matDest[i] = c>=extr? 0: mat[MAT_IND(r,c,l,m)]*diag[c];
				}
			}

			// Returning:
			return matDest;
		}

		/*
		* Matrix multiplication generating only the lower triangle of the matrix as in tensors.
		*  (destination matrix preallocated or NULL to force the allocation)
		* (lxm)*(mxl) = (lxl)
		* TODO: Use the BLAS library here!!!
		*/
		template<class T>
		T* mat_multiply_get_triangular(T *mat1,T *mat2,T *matDest,
						vector_dim_t l,vector_dim_t m) {
			// Indexes:
			vector_ind_t i,j,k,pos,k2,
				tot=((vector_ind_t)l)*((vector_ind_t)l+1)/2;

			// Allocation:
			if (matDest==NULL) {
				// Destination must be l*n:
				matDest = vec_create<T>(tot);
			}

			// Initializing the index of the destination matrix:
			i = 0;

			// Iterating on the rows of mat1:
			for(j=0; j<l; j++) {
				// Indexes:
				k2 = MAT_IND(0,j,m,l);

				// Iterating on the columns of mat2:
				for(k=j; k<l; k++,i++) {
					// Initializing the resulting value:
					matDest[i] = (T)0;

					// Iterating on the columns of mat1:
					for(pos=0; pos<m; pos++,k2++) {
						// Adding:
						matDest[i] += mat1[MAT_IND(j,pos,l,m)]*mat2[k2];
					}
				}
			}

			// Returning:
			return matDest;
		}

		// Transpose:

		/* 
		* Eigensystem of a symmetric matrix:
		*  the passed matrix must be triangular, only the lower triagular part is used,
		* the resulting eigenvectors and eigenvalues are stored in two given and preallocated
		* buffers of dimensionality nxn and n rispectively. On exit the previous content of
		* the matrix mat is destroyed.
		*/
		template<class T>
		void mat_eig_sym(T *A,T *eigenvectors, T *eigenvalues, vector_dim_t dim,bool full=true) {
			// Parameters:
			T toll = 0;
			integer nEigenvalues,info,
					lwork = 26*dim,
					liwork = 10*dim,
					il = 0,iu = 0;
			T vl = 0,vu = 0;

			// Saved local variables (and memory buffers):
			static integer sdim = 0;
			static T *work = NULL;
			static integer *iwork = NULL;
			static integer *issupz = NULL;

			// Deallocation:
			if (A==NULL) {
				// A deallocation is required:
				vec_free(work); work = NULL;
				vec_free(iwork); iwork = NULL;
				vec_free(issupz); issupz = NULL;
				return;
			}

			// Check for the storage:
			if (work==NULL || sdim!=dim) { 
				// Deallocating:
				vec_free<T>(work);
				work = vec_create<T>(lwork);
			}
			if (iwork==NULL || sdim!=dim) {
				// Deallocating:
				vec_free<integer>(iwork);
				iwork = vec_create<integer>(liwork);
			}
			if (issupz==NULL || sdim!=dim) {
				// Deallocating:
				vec_free<integer>(issupz);
				issupz = vec_create<integer>(dim);
			}
			if (sdim!=dim) sdim = dim;

			// Computing the eigensystem of the tensor tns:
			if (full) {
				// Requiring full computation:
				syevr("V","A","L",&sdim,A,&sdim,
						&vl,&vu,&il,&iu,&toll,&nEigenvalues,
						eigenvalues,eigenvectors,&sdim,issupz,
						work,&lwork,iwork,&liwork,&info);
			}
			else {
				// Only eigenvalues computation is required:
				syevr<T>("N","A","L",&sdim,A,&sdim,
						&vl,&vu,&il,&iu,&toll,&nEigenvalues,
						eigenvalues,NULL,&sdim,issupz,
						work,&lwork,iwork,&liwork,&info);
			}
		}

		// Eigensystem of a symmetric matrix without matrix loss:
		template<class T>
		void mat_eig_sym_save(T *mat,T *eigenvectors, T *eigenvalues, vector_dim_t dim) {
			// Static variables:
			static vector_dim_t sdim = 0;
			static T *matCopy = NULL;

			// Deallocation:
			if (mat==NULL) {
				// A deallocation is required:
				vec_free(matCopy);
				matCopy = NULL;
				// Deallocating also in the non-save version:
				mat_eig_sym<T>(NULL,NULL,NULL,0);
				return;
			}

			// Allocation:
			if (matCopy==NULL || sdim!=dim) {
				// Deallocating:
				mat_free(matCopy);
				matCopy = mat_create<T>(dim,dim);
			}
			if (sdim!=dim) sdim = dim;

			// Saving the previous content of the matrix:
			memcpy(matCopy,mat,sizeof(T)*MAT_DIM(dim,dim));

			// Computing the eigensystem:
			mat_eig_sym(matCopy,eigenvectors,eigenvalues,dim);
		}

		/*
		* Pseudo-inverse of a given matrix:
		*  the inverse for invertible matrixes) or the pseudoinverse of the given matrix is
		* computed and returned. The content of the matrix mat will be lost. The matrix matDest
		* will be used to store the result and must be alwais of size MxM where M is the MAX(dimr,dimc);
		* a NULL value allows to allocate it internally.
		*/
		template<class T>
		T* mat_pinv(T *mat,T *matDest,vector_dim_t dimr,vector_dim_t dimc) {
			// Parameters:
			static integer sdimr = 0,sdimc = 0,info,lwork = 0;
			static T *S = NULL, *work = NULL;
			integer maxDim = MAX(dimr,dimc),rank;
			T rcond = -1;

			// Deallocation:
			if (mat==NULL) {
				// A deallocation is required:
				vec_free(work); work = NULL;
				vec_free(S); S = NULL;
				return NULL;
			}

			// Output variable:
			matDest = mat_create_identity<T>(matDest,(vector_dim_t)maxDim);

			// Check for the storage:
			if (S==NULL || sdimr!=dimr || sdimc!=dimc) {
				// Unallocating and reallocating:
				vec_free(S);
				S = vec_create<T>(MIN(dimr,dimc));
			}
			if (lwork==0 || work==NULL || dimr!=sdimr || dimc!=sdimc) {
				// Unallocating and reallocating:
				vec_free(work);
				// The optimal size of work must be computed:
				T goodSize;
				integer ldimr=dimr, ldimc=dimc;
				lwork = -1;
				gelss(&ldimr,&ldimc,&maxDim,mat,&ldimr,matDest,&maxDim,S,&rcond,&rank,&goodSize,&lwork,&info);
				// Reallocating:
				lwork = (integer)goodSize;
				work = vec_create<T>(lwork);
			}
			if (sdimr!=dimr || sdimc!=dimc) {
				// Saving again:
				sdimr = dimr;
				sdimc = dimc;
			}

			// Calling lapack:
			gelss(&sdimr,&sdimc,&maxDim,mat,&sdimr,matDest,&maxDim,S,&rcond,&rank,work,&lwork,&info);

			// No reallocation of a new matrix:
			return matDest;
		}

		// Pseudo-inverse of a matrix without matrix loss:
		template<class T>
		T* mat_pinv_save(T *mat,T *matDest,vector_dim_t dimr,vector_dim_t dimc) {
			// Static variables:
			static vector_dim_t sdimr = 0,sdimc = 0;
			static T *matCopy = NULL;

			// Deallocation:
			if (mat==NULL) {
				// A deallocation is required:
				vec_free(matCopy); matCopy = NULL;
				// Deallocating also in the non-save version:
				mat_pinv<T>(NULL,NULL,0,0);
				return NULL;
			}

			// Allocation:
			if (matCopy==NULL || sdimr!=dimr || sdimc!=dimc) {
				// Deallocating:
				mat_free(matCopy);
				matCopy = mat_create<T>(dimr,dimc);
			}
			if (sdimr!=dimr || sdimc!=dimc) {
				// Saving again:
				sdimr = dimr;
				sdimc = dimc;
			}

			// Saving the previous content of the matrix:
			memcpy(matCopy,mat,sizeof(T)*MAT_DIM(dimr,dimc));

			// Computing the pseudo-inverse and returning:
			return mat_pinv(matCopy,matDest,dimr,dimc);
		}

		/*
		*  This function allows to linearly transform a set of points in a given
		* subspace. The subspace must be provided as a rectangular matrix of
		* orthonormal column vectors. The points are give as column vectors in a
		* matrix and the transformed points are given in a new matrix as column
		* vectors as well. The transofrmation must be given as an MxM matrix where M
		* is the dimensionality of the subspace. 
		*	P: The marix of N dim-dimensional points (dimxN).
		*	V: The subspace matrix (dimxM).
		*	A: The transformation matrix (MxM).
		*	D: Destination matrix (dimxN).
		*	Ph:	Working buffer for Ph (>=MxN).
		*	VA:	Working buffer for VA (>=dimxM).
		*/
		template<class T>
		T *vec_points_transform_subspace(const T *P,const T *V,const T *A,T *D,
										T *Ph,T *VA,
										vector_dim_t M,vector_dim_t N,vector_dim_t dim) {
			/*
			* The computation can be done as:
			*  D = Pt + (P-V*Ph) = V*Pth + res = V*A*Ph + res = VA*Ph + res
			* computing the projected points Ph = V'*P first and mantaining them
			* for the residual computation (P-V*Ph), computing the VA = V*A
			* transformation and "de-projection" matrix, then computing
			* Pt = VA*Ph that are the transformed points in Rdim (not restored)
			* and finally adding the residual P-V*Ph (this can be done all in
			* once by a sub-function).
			*/

			// Indexes:
			vector_ind_t i,i2,j,j2,k,k2;

			// Allocation checks:
			bool PhAllocated = false,
				VAAllocated = false;

			// Allocating the destination matrix:
			if (D==NULL) D = mat_create<T>(dim,N);

			// Allocation of the work space:
			if (Ph==NULL) {
				// Allocating and setting the flag:
				Ph = mat_create<T>(M,N);
				PhAllocated = true;
			}
			if (VA==NULL) {
				// Allocating and setting the flag:
				VA = mat_create<T>(dim,M);
				VAAllocated = true;
			}

			// The points must be projected on the subspace (Ph = V'P):
			mat_multiply_transpose(V,P,Ph,1,M,dim,N);

			// Computing the transform-"de-project" matrix VA:
			mat_multiply(V,A,VA,dim,M,M);

			// The partial result (without residual):
			mat_multiply(VA,Ph,D,dim,M,N);

			// Adding the residual:(lxm)*(mxn) = (lxn)
			for(i=0; i<dim; i++) {
				// Iterating on the columns of mat2:
				for(j=0,i2=i,k2=0; j<N; j++,i2+=dim) {
					// Iterating on the columns of mat1:
					for(k=0,j2=i; k<M; k++,k2++,j2+=dim) {
						// Adding V*Ph:
						D[i2] -= V[j2]*Ph[k2];
					}

					// Adding P:
					D[i2] += P[i2];
				}
			}

			// Deallocating work space:
			if (PhAllocated) mat_free(Ph);
			if (VAAllocated) mat_free(VA);

			// Returning:
			return D;
		}

		/*
		* Cleaning of static variables:
		*  in lapack-based functions static buffers are allocated; here
		* all the buffers (for a given type) are released.
		*/
		template<class T>
		void mat_free_static_buffs() {
			// Cleaning the buffers:
			mat_eig_sym_save<T>(NULL,NULL,NULL,0);
			mat_pinv_save<T>(NULL,NULL,0,0);
		}

	} // namespace Math
} // namespace TVF

#endif //ndef _MATHMATRIXES_H_
