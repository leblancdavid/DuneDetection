
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

#ifndef _MATHTENSORS_H_
#define _MATHTENSORS_H_

/*
 *  This library provides a set of functions that can be used with row-defined tensors
 * of type tensor_t. The tensor_t type is managed as a plain C-class (simulation of OOP
 * using the C language). Only the templates and allocators are used from the C++ features.
 * This choice allows to reuse the tensor's funcitons defined here as tools to build up
 * different classes that work in different manners. The templates are used to generalize
 * the data type used to store single values, float and double are the main choices.
 *  The tensors can here represented in three modes:
 *   1) As tensor_t or tensor_p variablres (the data array or the pointer to it):
 *		This represent the tensor as a square simmetric matrix. The data stored are only 
 *		the upper triangle of the matrix stored by rows.
 *   2) As an eigensystem:
 *		This represents the orthogonal axis of the hiper-ellipsoid (eigenvectors) and the
 *		lengths of the axis (eigenvalues).
 *   3) As a set of N (generally D) base tensors used to obtain the given tensor as linear
 *		combination and the coefficients used in the combination.
 */

// Useful libs:
#include "mathMatrixes.h"

///////////////////
// Useful macros //
///////////////////

// The number of degrees of freedom of a tensor:
#define TNS_DOF_NUM(__dim) ((tensor_ind_t)(__dim)*((__dim)+1)/2)

// Row of the index (in [0:D-1]):
#define TNS_IND_ROW(__ind,__dim) ((__dim)-(tensor_dim_t)floorf(sqrtf(2.0f*(TNS_DOF_NUM(__dim)-(__ind)-1.0f)+1.25f)+0.5f))

// Column of the index (in [0:D-1]):
#define TNS_IND_COL_GIVEN_ROW(__ind,__row,__dim) (((((tensor_dim_t)(__row))*((__row)+1))/2+(__ind))%(__dim))
#define TNS_IND_COL(__ind,__dim) (((__tns_g_ind_tmp=TNS_IND_ROW((__ind),(__dim)),__tns_g_ind_tmp*(__tns_g_ind_tmp+1))/2+(__ind))%(__dim))

// Both indexes:
#define TNS_IND_BOTH(__ind,__dim,__row,__col) \
										(__row) = TNS_IND_ROW((__ind),(__dim)); \
										(__col) = TNS_IND_COL_GIVEN_ROW((__ind),(__row),(__dim));

// Index from coords:
#define __TNS_IND(__row,__col,__dim) (TNS_DOF_NUM(__dim)-TNS_DOF_NUM((__dim)-(__row))+((__col)-(__row)))
#define TNS_IND(__row,__col,__dim) __TNS_IND(MIN((__row),(__col)),MAX((__row),(__col)),(__dim))

// Is an index referring to the diagonal?
#define TNS_IND_ISDIAGONAL(__ind,__dim) \
	((__tns_g_ind_tmp=TNS_IND_ROW((__ind),(__dim))),__tns_g_ind_tmp==TNS_IND_COL_GIVEN_ROW((__ind),__tns_g_ind_tmp,(__dim)))

// Itearting on a tensor:
#define TNS_ITERATE_LOWER(__row,__col,__ind,__dim) \
	for ((__ind)=0,(__row)=0; (__row)<(__dim); (__row)++) \
		for ((__col)=(__row); (__col)<(__dim); (__col)++,(__ind)++)
#define TNS_ITERATE_FULL(__row,__col,__ind,__dim) \
	for ((__row)=0; (__row)<(__dim); (__row)++) \
		for ((__col)=0; (__col)<(__dim) && ((__ind)=TNS_IND((__row),(__col),(__dim)))>=0; (__col)++)
#define TNSE_ITERATE_VECT_H(__row,__col,__ind,__dim) \
	MAT_ITERATE_H(__row,__col,__ind,__dim,__dim)
#define TNSE_ITERATE_VECT_V(__row,__col,__ind,__dim) \
	MAT_ITERATE_V(__row,__col,__ind,__dim,__dim)
#define TNSE_ITERATE_VALS(__ind,__dim) \
	VEC_ITERATE(__ind,__dim)


// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {

	/////////////
	// Classes //
	/////////////

	// Local data structures:
	namespace DataStructures {

		// Usings:
		using namespace TVF::Math;

		/*
		* The bit masking interface:
		*/
		class ITVBitMask {
		public:
			/********************************
			* Constructors and destructor: *
			********************************/

			// Destructor:
			virtual ~ITVBitMask() {}

			/**************************
			* Accessors to the bits: *
			**************************/

			// Getting the value of a bit:
			virtual bool getBit(vector_ind_t i) const =0;

			// Getting the value of a bit:
			virtual ITVBitMask *setBit(vector_ind_t i,bool value)=0;

			// Setting all the bits:
			virtual ITVBitMask *setAll(bool value)=0;

			/***************************
			* Accessors to the infos: *
			***************************/

			// Getter for the max number of bits:
			virtual vector_ind_t getSize() const=0;
		};

	} // namespace DataStructures

	namespace Math {

		// Usings:
		using namespace TVF::DataStructures;

		////////////////
		// Data types //
		////////////////

		// A tensor type:
		template<class T>
		struct tensor_t {
			// The pointer:
			T *ptr;

			// The constructor:
			tensor_t(T *p=NULL) : ptr(p) {}

			// Casting to a pointer:
			operator T*() const {return ptr;}
		};

		// The tensor dimensionality:
		typedef vector_dim_t tensor_dim_t;
		typedef vector_ind_t tensor_ind_t;

		// The decoded tensor:
		template<class T>
		struct tensor_decoded_t {
			// The eigenvectors:
			T *eigenVectors;

			// The eigenvalues:
			T *eigenValues;
		};


		//////////////////////
		// Global veriables //
		//////////////////////

		/* Variables used in the macros: */

		// Temporary index:
		extern tensor_ind_t __tns_g_ind_tmp;

		/////////////
		// Classes //
		/////////////

		/*
		*  This class represents a saliency function. Here a dummy version is
		* implemented to represent a "no decay" function (alwais 1).
		*/
		template<class T>
		class ITVDecayFun {
		public:
			// Virtual destructor:
			virtual ~ITVDecayFun() {}

			// Saliency computation:
			virtual T compute(const T *p,tensor_dim_t dim,...) {
				// A dummy version implemented here returns 1:
				return 1;
			}
		};


		///////////////
		// Functions //
		///////////////

		// Generating an uninitialized tensor:
		template<class T>
		tensor_t<T> tns_create(tensor_dim_t dim) {
			// Allocating the tensor structure:
			tensor_t<T> ret;
			ret.ptr = vec_create<T>(TNS_DOF_NUM(dim));

			// Returning:
			return ret;
		}

		// Generating an empty tensor:
		template<class T>
		tensor_t<T> tns_create_empty(tensor_t<T> ret,tensor_dim_t dim) {
			// Allocating the tensor structure:
			tensor_dim_t r,c;
			tensor_ind_t i;
			if (ret.ptr==NULL) {
				ret.ptr = vec_create<T>(TNS_DOF_NUM(dim));
			}

			// Filling the matrix:
			TNS_ITERATE_LOWER(r,c,i,dim) {
				// Check for diagonal:
				ret.ptr[i] = r==c? (T)EPS: (T)0;
			}

			// Returning:
			return ret;
		}

		// Generating a ball tensor:
		template<class T>
		tensor_t<T> tns_create_ball(tensor_t<T> ret,tensor_dim_t dim) {
			// Allocating the tensor structure:
			tensor_dim_t r,c;
			tensor_ind_t i;
			if (ret.ptr==NULL) {
				ret.ptr = vec_create<T>(TNS_DOF_NUM(dim));
			}

			// Filling the matrix:
			TNS_ITERATE_LOWER(r,c,i,dim) {
				// Check for diagonal:
				ret.ptr[i] = r==c? (T)1: (T)0;
			}

			// Returning:
			return ret;
		}

		// Generating a tensor clone:
		template<class T>
		tensor_t<T> tns_create_clone(const tensor_t<T> tns,tensor_dim_t dim) {
			// Allocating a new tensor:
			tensor_ind_t tot = TNS_DOF_NUM(dim);
			tensor_t<T> ret;
			ret.ptr = vec_create_clone<T>(NULL,tns.ptr,tot);

			// Returning:
			return ret;
		}

		// Eliminating a tensor:
		template<class T>
		tensor_t<T> tns_free(tensor_t<T> tns) {
			// Unallocating:
			vec_free<T>(tns.ptr);
			tns.ptr = NULL;
			return tns;
		}

		// Copy of a tensor:
		template<class T>
		void tns_copy(tensor_t<T> tnsDest,const tensor_t<T> tnsSrc,tensor_dim_t dim) {
			// Coping the data:
			memcpy(tnsDest.ptr,tnsSrc.ptr,sizeof(T)*TNS_DOF_NUM(dim));
		}

		/*
		* Coping the data in a full matrix:
		* if (matDst!=NULL) the matrix is preallocated, if (matDst==NULL)
		* here the allocation is done.
		*/
		template<class T>
		T* tns_copy_in_matrix(T *matDst,const tensor_t<T> tnsSrc,tensor_dim_t dim) {
			// Allocating:
			if (matDst==NULL) {
				// Allocating:
				matDst = mat_create<T>(dim,dim);
			}

			// Iterating:
			tensor_ind_t r,c,i;
			TNS_ITERATE_LOWER(r,c,i,dim) {
				// Saving in the matrix:
				matDst[MAT_IND(r,c,dim,dim)] = tnsSrc.ptr[i];
				if (r!=c) {
					matDst[MAT_IND(c,r,dim,dim)] = tnsSrc.ptr[i];
				}
			}

			// Returning:
			return matDst;
		}

		/*
		* Coping the data from a full matrix, the simmetric part of the square matrix is automatically taken.
		*/
		template<class T>
		tensor_t<T> tns_copy_from_matrix(tensor_t<T> tnsDst,const T *matSrc,tensor_dim_t dim) {
			// Allocating:
			if (tnsDst.ptr==NULL)
				tnsDst = tns_create<T>(dim);

			// Iterating:
			tensor_ind_t r,c,i;
			TNS_ITERATE_LOWER(r,c,i,dim) {
				// Saving in the matrix:
				if (r!=c) {
					// The mean of the two elements:
					tnsDst.ptr[i] = (matSrc[MAT_IND(r,c,dim,dim)] + matSrc[MAT_IND(c,r,dim,dim)])/(T)2;
				}
				else {
					// The mean of the two elements:
					tnsDst.ptr[i] = matSrc[MAT_IND(r,c,dim,dim)];
				}
			}

			// Returning:
			return tnsDst;
		}

		// Accumulating "a tensor over another":
		template<class T>
		void tns_accumulate(tensor_t<T> tnsDst,const tensor_t<T> tnsSrc,tensor_dim_t dim) {
			// Iterating over the tensor:
			tensor_ind_t tot = TNS_DOF_NUM(dim);

			// Adding values:
			vec_accumulate(tnsDst.ptr,tnsSrc.ptr,tot);
		}

		// Adding two tensors:
		template<class T>
		tensor_t<T> tns_add(const tensor_t<T> tns1,const tensor_t<T> tns2,tensor_dim_t dim) {
			// Cloning the first tensor:
			tensor_t<T> ret = tns_create_clone(tns1,dim);

			// Accumulating the second:
			tns_accumulate(ret,tns2,dim);

			// Returning:
			return ret;
		}

		// Scaling a tensor:
		template<class T>
		void tns_scale(tensor_t<T> tns,T scale,tensor_dim_t dim) {
			// Iterating over the tensor:
			tensor_ind_t tot = TNS_DOF_NUM(dim);

			// Scaling values:
			vec_scale(tns.ptr,scale,tot);
		}

		// Scaling a tensor by cloning:
		template<class T>
		tensor_t<T> tns_mul(const tensor_t<T> tns,T scale,tensor_dim_t dim) {
			// Cloning the tensor:
			tensor_t<T> ret = tns_create_clone(tns,dim);

			// Scaling:
			tns_scale(ret,scale,dim);

			// Returning:
			return ret;
		}

		// Tensor norm L1:
		template<class T>
		T tns_norm_L1(const tensor_t<T> tns,tensor_dim_t dim) {
			// Indexes:
			tensor_ind_t r,c,i;
			T res = 0;

			// Iterating:
			TNS_ITERATE_LOWER(r,c,i,dim) {
				// Subtracting:
				res += tns.ptr[i];
				// Adding abs values:
				if (r==c) {
					// Only the diagonal element:
					res += ABS(tns.ptr[i]);
				}
				else {
					// Both the symmetric elements:
					res += 2*ABS(tns.ptr[i]);
				}
			}

			// Returning:
			return res;
		}

		// Tensor norm L2:
		template<class T>
		T tns_norm_L2(const tensor_t<T> tns,tensor_dim_t dim) {
			// Indexes:
			tensor_ind_t r,c,i;
			T res = 0;

			// Iterating:
			TNS_ITERATE_LOWER(r,c,i,dim) {
				// Adding squares:
				if (r==c) {
					// Only the diagonal element:
					res += tns.ptr[i]*tns.ptr[i];
				}
				else {
					// Both the symmetric elements:
					res += 2*tns.ptr[i]*tns.ptr[i];
				}
			}

			// Returning:
			return sqrt(res);
		}

		// Tensor norm L2:
		template<class T>
		T tns_norm_L2_squared(const tensor_t<T> tns,tensor_dim_t dim) {
			// Indexes:
			tensor_ind_t r,c,i;
			T res = 0;

			// Iterating:
			TNS_ITERATE_LOWER(r,c,i,dim) {
				// Adding squares:
				if (r==c) {
					// Only the diagonal element:
					res += tns.ptr[i]*tns.ptr[i];
				}
				else {
					// Both the symmetric elements:
					res += 2*tns.ptr[i]*tns.ptr[i];
				}
			}

			// Returning:
			return res;
		}

		// Tensor norm Linf:
		template<class T>
		T tns_norm_Linf(const tensor_t<T> tns,tensor_dim_t dim) {
			// Returning:
			return vec_norm_Linf<T>(tns.ptr,TNS_DOF_NUM(dim));
		}

		// Normalization given the norm:
		template<class T>
		void tns_normalize(tensor_t<T> tns,tensor_dim_t dim,T (*norm)(tensor_t<T>,tensor_dim_t)) {
			// Computing the norm:
			volatile T n = norm(tns,dim);

			// Normalizing:
			if (n!=0 && n!=1) {
				tns_scale(tns,1/n,dim);
			}
		}

		// Rotating a tensor with R:
		template<class T>
		tensor_t<T> tns_rotate(const tensor_t<T> tns,tensor_t<T> dest,T *R,tensor_dim_t dim) {
			// Input variables:
			static T *tmp1 = NULL, *tmp2 = NULL;
			static tensor_dim_t sdim = 0;

			// Deallocation:
			if (dim==0) {
				// A deallocation is required:
				vec_free(tmp1); tmp1 = NULL;
				vec_free(tmp2); tmp2 = NULL;
				return dest;
			}

			// Check for the storage:
			if (tmp1==NULL || sdim!=dim) {
				// Deallocating:
				mat_free<T>(tmp1);
				tmp1 = mat_create<T>(dim,dim);
			}
			if (tmp2==NULL || sdim!=dim) {
				// Deallocating:
				mat_free<T>(tmp2);
				tmp2 = mat_create<T>(dim,dim);
			}
			if (sdim!=dim) sdim = dim;

			// Allocating the destination:
			if (dest.ptr==NULL) {
				// Allocating a new tensor:
				dest = tns_create<T>(dim);
			}

			// Preparing the input:
			tns_copy_in_matrix(tmp1,tns,dim);

			// A tensor T rotation is done as RTR':
			mat_multiply_transpose(tmp1,R,tmp2,2,dim,dim,dim);
			mat_multiply_get_triangular(R,tmp2,dest.ptr,dim,dim);

			// Returning:
			return dest;
		}

		// Printing a tensor on a file:
		template<class T>
		void tns_fprint(FILE *fp,const tensor_t<T> tns,tensor_dim_t dim) {
			// Indexes:
			tensor_dim_t r,c;
			tensor_ind_t i;

			// Iterating on rows:
			TNS_ITERATE_FULL(r,c,i,dim) {
				// Showing the value:
				fprintf(fp,"\t%f",tns.ptr[i]);

				// The end of line:
				if (c>=dim-1) {
					// Adding a newline:
					fprintf(fp,"\n");
				}
			}
		}

		// Constructor for the decoded tensor:
		template<class T>
		tensor_decoded_t<T>* tnsd_create(tensor_dim_t dim) {
			// Allocating the structure:
			tensor_decoded_t<T> *res = new tensor_decoded_t<T>;

			// Allocating buffers:
			res->eigenVectors = mat_create<T>(dim,dim);
			res->eigenValues = vec_create<T>(dim);

			// Returning:
			return res;
		}

		// Constructor for the decoded tensor:
		// The given vectors are used directly without reallocaiton or copy!
		template<class T>
		tensor_decoded_t<T>* tnsd_create_data(T *eigenVectors,T *eigenValues,tensor_dim_t dim) {
			// Allocating the structure:
			tensor_decoded_t<T> *res = new tensor_decoded_t<T>;

			// Saving pointers:
			res->eigenVectors = eigenVectors;
			res->eigenValues = eigenValues;

			// Returning:
			return res;
		}

		// Destructor for a tensor_decoded_t:
		// Also the support is deleted.
		template<class T>
		void tnsd_free(tensor_decoded_t<T>* tnsd) {
			if (tnsd!=NULL) {
				// Removing vectors:
				vec_free(tnsd->eigenVectors);
				vec_free(tnsd->eigenValues);

				// Also the struct:
				delete tnsd;
			}
		}

		// Decoding of a tensor:
		template<class T>
		tensor_decoded_t<T>* tns_decode(const tensor_t<T> tns,tensor_dim_t dim,bool full=true) {
			// Input variables:
			static T *A = NULL;
			static integer sdim = 0;

			// Output variables:
			T	*eigenvalues,
				*eigenvectors = NULL;

			// Deallocation:
			if (dim==0) {
				// A deallocation is required:
				vec_free(A); A = NULL;
				return NULL;
			}

			// Check for the storage:
			if (A==NULL || sdim!=dim) {
				// Deallocating:
				mat_free<T>(A);
				A = mat_create<T>(dim,dim);
			}
			if (sdim!=dim) sdim = dim;

			// Allocating output:
			eigenvalues = vec_create<T>(dim);
			if (full) {
				// Also the eigenvectors:
				eigenvectors = mat_create<T>(dim,dim);
			}
			else {
				// No eigenvectors:
				eigenvectors = NULL;
			}

			// Preparing the input:
			tns_copy_in_matrix(A,tns,dim);

			// Computing the eigensystem of the tensor tns:
			mat_eig_sym(A,eigenvectors,eigenvalues,dim,full);
			
			// Returning:
			return tnsd_create_data<T>(eigenvectors,eigenvalues,dim);
		}

		// Encoding of a tensor:
		template<class T>
		tensor_t<T> tns_encode(const tensor_decoded_t<T>* tnsd,tensor_t<T> res,tensor_dim_t dim) {
			// Local variables:
			static T *tmp = NULL;
			static tensor_dim_t sdim = 0;

			// Deallocation:
			if (tnsd==NULL) {
				// A deallocation is required:
				mat_free(tmp);
				tmp = NULL;
				return res;
			}

			// Allocating res:
			if (res.ptr==NULL) res.ptr = vec_create<T>(TNS_DOF_NUM(dim));

			// Check for the storage:
			if (tmp==NULL || sdim!=dim) {
				// Deallocating:
				mat_free<T>(tmp);
				tmp = mat_create<T>(dim,dim);
			}
			if (sdim!=dim) sdim = dim;

			/* Add or remove the double '/' at the beginning of this line to change computation algorithm!
			// Computing X*L*pinv(X):
			// - Firstly I compute the pinv ov X:
			mat_pinv_save<T>(tnsd->eigenVectors,tmp,dim,dim);

			// - Then compute L*pinv(X) considering that L is diagonal:
			mat_multiply_diagonal(
				tmp,tnsd->eigenValues,tmp,dim,dim,dim,true);
			/*/

			// --- Algorithm based on the transposition: ---
			// Computing X*L*pinv(X)=X*L*X'=X*(X*L')'=X*(X*L)'
			// - First the (X*L)') is computed at once:
			mat_multiply_diagonal_transpose(
				tnsd->eigenVectors,tnsd->eigenValues,tmp,
				dim,dim,dim,false);
			//*/

			// - Then the X is multiplied on the left:
			mat_multiply_get_triangular<T>(tnsd->eigenVectors,tmp,res.ptr,dim,dim);

			// Returning:
			return res;
		}

		/*
		*  This function allows to compute the eigenvectors of a decoded tensor rotating the
		* identity matrix. Given the eigenvalues this can be used to generate a complete base
		* tensor. The votes generation procedure generates each single base tensor using
		* this function and then sums them all toghether. The eigenvalues of the generated
		* tensor are set to NULL and must be manually set. The order is managed circularly
		* (as ord=ord%dim).
		*	p:		The position of the vote wrt the centered and unoriented tensor.
		*	D:		The destination matrix.
		*	ord:	The order (number of compressed dimensions as trailing part of vectors).
		*	dim:	The dimensionality.
		*/
		template<class T>
		T *tnsd_orientation(const T* p,T* D,tensor_dim_t ord,tensor_dim_t dim) {
			// Computing the rotation angle in the span<P,Ph> linear subspace:
			T theta,ct,st;

			// The V matrix is managed as a static one:
			static T *V = NULL;
			static int sdim = 0;

			// Indexes:
			tensor_ind_t i,j,indD;

			// Has you required a cleaning?
			if (p==NULL) {
				// Removing the V matrix:
				mat_free(V);
				V = NULL;
				return NULL;
			}

			// The V matrix is managed as a static one:
			if (V==NULL || sdim!=dim) {
				// Reallocating:
				mat_free(V);
				V = mat_create<T>(dim,2);
			}
			if (sdim!=dim) sdim = dim;

			// Allocating D:
			if (D==NULL) D = mat_create<T>(dim,dim);

			// The order is managed circularly:
			ord = ord%dim;

			// Computing the transformation (rotation) matrix parts:
			theta = 2*vec_angle_linproj_zeroing(p,ord,dim);
			ct = cos(theta) - 1;
			st = sin(theta);

			// Checking for zero angles:
			if (ord==0 || (st>=-EPS && st<=EPS)) {
				// The identity is the resulting orientation:
				mat_create_identity(D,dim);
				return D;
			}

			// Computing V:

			// - Initial vectors:
			memcpy(V,p,dim*sizeof(T));
			vec_normalize(V,dim,vec_norm_L2<T>);
			memcpy(V+dim+dim-ord,p+dim-ord,ord*sizeof(T));
			memset(V+dim,0,(dim-ord)*sizeof(T));

			// - Orthonormalizing the second vector:
			vec_orthogonalize(V,V+dim,dim);
			vec_normalize(V+dim,dim,vec_norm_L2<T>);

			// Iterating on the destination matrix:
			MAT_ITERATE_V(j,i,indD,dim,dim) {
				// Computing a value:
				D[indD] =	(+ V[i]*ct + V[i+dim]*st)*V[j] +
							(- V[i]*st + V[i+dim]*ct)*V[j+dim];

				// Adding the identity:
				if (i==j) D[indD]++;
			}

			// Returning:
			return D;
		}

		/*
		*  This function allows to compute a base tensor vote encoded as a
		* tensor_t. The generation of the ball tensor is treated here as a
		* particular case hiding the distinction to the client of this
		* function. The emitting tensor is assumed to be in the origin
		* canonically oriented.
		*/
		template<class T>
		tensor_t<T> tns_create_vote_base(const T* p,tensor_t<T> dest,T sal,tensor_dim_t ord,tensor_dim_t dim) {
			// Static decoded tensor:
			static tensor_dim_t sdim = 0;
			static tensor_decoded_t<T>* tnsd = NULL;
			static T *eigenvalues = NULL, *eigenvectors = NULL;

			// Indexes:
			tensor_ind_t i,r,c;

			// Checking for cleaning:
			if (p==NULL) {
				// Deallocating:
				tnsd_free(tnsd); tnsd = NULL;
				return dest;
			}

			// Allocating the destination:
			if (dest.ptr==NULL) {
				dest.ptr = vec_create<T>(TNS_DOF_NUM(dim));
			}

			// Here a static decoded tensor is mantained as temporary working space:
			if (dim!=sdim || tnsd==NULL) {
				// Reallocating:
				tnsd_free(tnsd);
				tnsd = tnsd_create<T>(dim);
			}
			if (dim!=sdim) sdim = dim;

			// The order is managed circularly:
			ord = ord%dim;

			// Check the type of tensorial vote:
			if (ord==0) { // A ball tensor:
				// Computing the scaling of the tensor:
				// INFO: I'm using zero now!
				//T salB = sal*sal;
				//T scaling = (1-salB)/(vec_norm_L2_squared(p,dim)+(T)EPS);
				T scaling = 1/(vec_norm_L2_squared(p,dim)+(T)EPS);

				// Generating a (scaled) ball tensor directly and return:
				TNS_ITERATE_LOWER(r,c,i,dim) {
					// Computing directly the resulting value:
					dest.ptr[i] = (r==c? 1-p[r]*p[c]*scaling: -p[r]*p[c]*scaling)*sal;
				}
			}
			else { // A general not-ball tensor:
				// Generating the orientation:
				tnsd_orientation(p,tnsd->eigenVectors,ord,dim);

				// Generating the saliency vector:
				VEC_ITERATE(i,dim) {
					// Saving the value:
					tnsd->eigenValues[i] = i>=(tensor_ind_t)(dim-ord)? sal: 0;
				}

				// Encoding:
				tns_encode(tnsd,dest,dim);
			}

			// Returning:
			return dest;
		}

		/*
		*  This function allows to compute the second order tensor vote generated from
		* a second order tensor on a single point. Here the saliency function pointer
		* as well as the position of the tensor are required. The given point is ranslated
		* and rotated to match the tensor position and orientation.
		*	p:		The point.
		*	Tp:		The position of the voting tensor.
		*	tnsd:	The voting tensor.
		*	dest:	The destination tensor.
		*	decf:	The saliency function taking:
		*				T* p:				A point (centered and canonically oriented tensor is assumed).
		*				tensor_dim_t ord:	The type of tensor (number of uncompressed dimensions).
		*				tensor_dim_t dim:	The dimensionality.
		*	bm:		The bit mask that says if the single tensor' votes must be generated. (NULL=all)
		*	dim:	The dimensionality.
		*/
		template<class T>
		tensor_t<T> tns_create_vote(const T* p,const T* Tp,const tensor_decoded_t<T> *tnsd,tensor_t<T> dest,
										ITVDecayFun<T> *decf,const ITVBitMask *bm,tensor_dim_t dim) {
			// The canonical static objects:
			static T *pcan = NULL, *mcan = NULL;
			static tensor_t<T> tcan(NULL);
			static tensor_dim_t sdim = 0;

			// The local indexes:
			tensor_dim_t ord;
			tensor_ind_t tot = TNS_DOF_NUM(dim);
			bool same = true;
			vector_ind_t j;

			// Checking for cleaning:
			if (p==NULL) {
				// Deallocating:
				tns_free(tcan); tcan.ptr = NULL;
				vec_free(pcan); pcan = NULL;
				mat_free(mcan); mcan = NULL;
				return dest;
			}

			// Allocating the destination:
			if (dest.ptr==NULL) {
				dest.ptr = vec_create<T>(TNS_DOF_NUM(dim));
			}

			// Checking for same position:
			for (j=0; j<dim; j++){
				// Checkin:
				if (ABS(p[j]-Tp[j])>EPS) {
					// Ok, me gusta!
					same = false;
					break;
				}
			}

			// Is it the same?
			if (same) {
				// Returning the tensor itself:
				tns_encode(tnsd,dest,dim);
				return dest;
			}

			// Here static buffers are mantained as temporary working space:
			if (dim!=sdim || tcan.ptr==NULL) {
				// Reallocating:
				tns_free(tcan); tcan = tns_create<T>(dim);
			}
			if (dim!=sdim || pcan==NULL) {
				// Reallocating:
				vec_free(pcan); pcan = vec_create<T>(dim);
			}
			if (dim!=sdim || mcan==NULL) {
				// Reallocating:
				mat_free(mcan); mcan = mat_create<T>(dim,dim);
			}
			if (dim!=sdim) sdim = dim;

			// Creating a clone of the point:
			vec_create_clone(mcan,p,dim);

			// Translating and reorienting it (obtained pint in pcan):
			vec_subtract(mcan,Tp,dim);
			mat_multiply_transpose(tnsd->eigenVectors,mcan,pcan,1,dim,dim,1);

			// The first is saved in the destination tensor:
			if (bm==NULL || bm->getBit(0)) {
				// Creating the base vote:
				tns_create_vote_base(pcan,tcan,decf->compute(pcan,dim,(tensor_dim_t)0)*tnsd->eigenValues[0],0,dim);
			}
			else {
				// Starting from zero:
				tns_create_empty(tcan,dim);
			}

			// Iterating on base tensors:
			for (ord=1; ord<dim; ord++) {
				// Check:
				if (bm==NULL || bm->getBit(ord)) {
					// Creating a new vote:
					tns_create_vote_base(pcan,dest,
						decf->compute(pcan,dim,ord)*(tnsd->eigenValues[dim-ord]-tnsd->eigenValues[dim-ord-1]),
						ord,dim);
					vec_accumulate(tcan.ptr,dest.ptr,tot);
				}
			}

			// Rotating the obtained tensor:
			tns_rotate(tcan,dest,tnsd->eigenVectors,dim);

			// Returning:
			return dest;
		}

		/*
		*  This function allows to compute the first order tensor vote generated from
		* a second order tensor on a single point. Here the saliency function pointer
		* as well as the position of the tensor are required. The given point is ranslated
		* and rotated to match the tensor position and orientation.
		*	p:		The point.
		*	Tp:		The position of the voting tensor.
		*	tnsd:	The voting tensor.
		*	dest:	The destination matrix dimxdim.
		*	decf:	The saliency function taking:
		*				T* p:				A point (centered and canonically oriented tensor is assumed).
		*				tensor_dim_t ord:	The type of tensor (number of uncompressed dimensions).
		*				tensor_dim_t dim:	The dimensionality.
		*	bm:		The bit mask that says if the single tensor' votes must be generated. (NULL=all)
		*	dim:	The dimensionality.
		*/
		template<class T>
		T *vec_create_vote(const T* p,const T* Tp,const tensor_decoded_t<T> *tnsd,T *dest,
							ITVDecayFun<T> *decf,const ITVBitMask *bm,tensor_dim_t dim) {
			// The canonical static objects:
			static T *v = NULL, *v2 = NULL, *v3 = NULL;
			static tensor_dim_t sdim = 0;

			// The local indexes:
			tensor_dim_t ord;
			bool same = true;
			vector_ind_t j;
			T scal,theta;

			// Checking for cleaning:
			if (p==NULL) {
				// Deallocating:
				vec_free(v); v = NULL;
				vec_free(v2); v2 = NULL;
				vec_free(v3); v3 = NULL;
				return dest;
			}

			// The result is initialized to zero:
			dest = vec_create_init(dest,(T)0,dim);

			// Checking for same position:
			for (j=0; j<dim; j++){
				// Checkin:
				if (ABS(p[j]-Tp[j])>EPS) {
					// Ok, me gusta!
					same = false;
					break;
				}
			}

			// Is it the same?
			if (same) {
				// End of computation:
				return dest;
			}

			// Here static buffers are mantained as temporary working space:
			if (dim!=sdim || v==NULL) {
				// Reallocating:
				vec_free(v); v = vec_create<T>(dim);
			}
			if (dim!=sdim || v2==NULL) {
				// Reallocating:
				vec_free(v2); v2 = vec_create<T>(dim);
			}
			if (dim!=sdim || v3==NULL) {
				// Reallocating:
				vec_free(v3); v3 = vec_create<T>(dim);
			}
			if (dim!=sdim) sdim = dim;

			// Computing the ball unscaled part:
			if (bm==NULL || bm->getBit(0)) {
				// The vote is generated using the vector from the receiver to the emitter:
				vec_create_clone<T>(v,Tp,dim);
				vec_subtract(v,p,dim);

				// Computing the scaling:
				scal = decf->compute(v,dim,0)/(vec_norm_L2(v,dim)+(T)EPS);

				// Scaling and adding to the old vote:
				vec_accumulate_scaled(dest,v,scal,dim);
			}

			// The other tensors:
			if (tnsd) {
				// Scaling the ball T1 tensor:
				if (bm==NULL || bm->getBit(0)) {
					// The scaling:
					vec_scale(dest,tnsd->eigenValues[0],dim);
				}

				// Reorienting p (obtaining v2):
				vec_create_clone(v,p,dim);
				vec_subtract(v,Tp,dim);
				mat_multiply_transpose(tnsd->eigenVectors,v,v2,1,dim,dim,1);

				// Iterating on the orders:
				for (ord=1; ord<dim; ord++) {
					// Checking if the computation is enabled:
					if (bm==NULL || bm->getBit(ord)) {
						// Computing the angle theta:
						theta = 2*vec_angle_linproj_zeroing_inverse(v2,ord,dim);

						// Cloning:
						vec_create_clone(v,v2,dim);

						// The scaling:
						scal = decf->compute(v,dim,ord)*
							(tnsd->eigenValues[dim-ord]-tnsd->eigenValues[dim-ord-1]);

						// Scaling the parts:
						vec_scale(v,-cos(theta)*scal/(vec_norm_L2(v,dim-ord)+(T)EPS),dim-ord);
						vec_scale(v+dim-ord,-sin(theta)*scal/(vec_norm_L2(v+dim-ord,ord)+(T)EPS),ord);

						// Reorienting the result:
						mat_multiply(tnsd->eigenVectors,v,v3,dim,dim,1);

						// Accumulating:
						vec_accumulate(dest,v3,dim);
					}
				}
			}

			// Returning:
			return dest;
		}

		// Computing the saliency from a decoded tensor:
		template <class T>
		T *tnsd_compute_saliency(const tensor_decoded_t<T> *tnsd,T *dest,tensor_dim_t dim) {
			// The local indexes:
			tensor_ind_t i;

			// Allocating the destination vector:
			if (dest==NULL) {
				// Allocating:
				dest = vec_create<T>(dim);
			}

			// Saving the first (ball) eigenvalue:
			dest[0] = tnsd->eigenValues[0];

			// Saving the other eigenvalues:
			for (i=1; i<dim; i++) {
				// Computing the i-th saliency value:
				dest[i] = tnsd->eigenValues[i]-tnsd->eigenValues[i-1];
			}

			// Returning:
			return dest;
		}

		// Printing a decoded tensor on a file:
		template<class T>
		void tnsd_fprint(FILE *fp,const tensor_decoded_t<T> *tnsd,tensor_dim_t dim) {
			// Indexes:
			tensor_dim_t r,c;
			tensor_ind_t i;

			// The eigenvectors:
			fprintf(fp,"The eigenvectors:\n");

			// Iterating on rows:
			TNSE_ITERATE_VECT_H(r,c,i,dim) {
				// Showing the value:
				fprintf(fp,"\t%f",tnsd->eigenVectors[i]);

				// The end of line:
				if (c>=dim-1) {
					// Adding a newline:
					fprintf(fp,"\n");
				}
			}

			// The eigenvalues:
			fprintf(fp,"The eigenvalues:\n");

			// Iterating on values:
			TNSE_ITERATE_VALS(i,dim) {
				// Showing the value:
				fprintf(fp,"\t%f",tnsd->eigenValues[i]);
			}
			fprintf(fp,"\n");
		}

		/*
		* Cleaning of static variables:
		*  in lapack-based functions static buffers are allocated; here
		* all the buffers (for a given type) are released.
		*/
		template<class T>
		void tns_free_static_buffs() {
			// Cleaning the buffers:
			tensor_t<T> tns;
			tns.ptr = NULL;
			tns_decode(tns,0);
			tns_encode<T>(NULL,tns,0);
			tns_rotate<T>(tns,tns,NULL,0);
			tnsd_orientation<T>(NULL,NULL,0,0);
			tns_create_vote_base<T>(NULL,tns,0,0,0);
			tns_create_vote<T>(NULL,NULL,NULL,tns,NULL,NULL,0);
			vec_create_vote<T>(NULL,NULL,NULL,NULL,NULL,NULL,0);
		}

	} // namespace Math
} // namespace TVF

#endif //ndef _MATHTENSORS_H_
