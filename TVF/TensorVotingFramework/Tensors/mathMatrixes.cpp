
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

// My header:
#include "mathMatrixes.h"


// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Math {

		/////////////////
		// Global vars //
		/////////////////


		///////////////
		// Functions //
		///////////////

		/*
		* Lapack templated declarations.
		*/
		template <>
		int syevr(char *jobz, char *range, char *uplo, integer *n, 
					real *a, integer *lda, real *vl, real *vu, integer *il, integer *iu, 
					real *abstol, integer *m, real *w, real *z__, integer *ldz, integer *isuppz, 
					real *work, integer *lwork, integer *iwork, integer *liwork, integer *info) {
			// Calling lapack:
			return ssyevr_(jobz,range,uplo,n,a,lda,vl,vu,il,iu,abstol,m,w,z__,ldz,isuppz,work,lwork,iwork,liwork,info);
		}
		template <>
		int syevr(char *jobz, char *range, char *uplo, integer *n, 
					doublereal *a, integer *lda, doublereal *vl, doublereal *vu, integer *il, integer *iu, 
					doublereal *abstol, integer *m, doublereal *w, doublereal *z__, integer *ldz, integer *isuppz, 
					doublereal *work, integer *lwork, integer *iwork, integer *liwork, integer *info) {
			// Calling lapack:
			return dsyevr_(jobz,range,uplo,n,a,lda,vl,vu,il,iu,abstol,m,w,z__,ldz,isuppz,work,lwork,iwork,liwork,info);
		}
		template <>
		int gelss(integer *m, integer *n, integer *nrhs, real *a, integer *lda, real *b, integer *ldb,
				real *s, real *rcond, integer *rank, real *work, integer *lwork, integer *info) {
			// Calling lapack:
			return sgelss_(m,n,nrhs,a,lda,b,ldb,s,rcond,rank,work,lwork,info);
		}
		template <>
		int gelss(integer *m, integer *n, integer *nrhs, doublereal *a, integer *lda, doublereal *b, integer *ldb,
				doublereal *s, doublereal *rcond, integer *rank, doublereal *work, integer *lwork, integer *info) {
			// Calling lapack:
			return dgelss_(m,n,nrhs,a,lda,b,ldb,s,rcond,rank,work,lwork,info);
		}

		/*
		* My function delcarations.
		*/

	} // namespace Math
} // namespace TVF
