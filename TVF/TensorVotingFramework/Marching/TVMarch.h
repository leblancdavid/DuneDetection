
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

#ifndef _TVMARCH_H_
#define _TVMARCH_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			/*
			 *  This class defines a general marching algorithm that
			 * can generate new points and objects in the space where
			 * the manifold to be infered is supposed to be.
			 */

			template <class T>
			class TVMarch {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The process to vote:
				TVProcess<T> *_pro;

				// The time discretization:
				T _dt;

				// The maximizer:
				ITVSaliencyMaximizer<T> *_maximizer;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVMarch(TVProcess<T> *pro,ITVSaliencyMaximizer<T> *maximizer,T dt)
					: _pro(pro), _maximizer(maximizer), _dt(dt) {}

				// Destructor:
				virtual ~TVMarch() {
					// The maximizer is composed with me:
					delete _maximizer;
				}

				/*********************
				 * Abstract methods: *
				 *********************/

				// Marching (the stopping criteria is a subclasses feature):
				virtual TVSpace<T> *march(TVSpace<T> *front)=0;

				// Generating new points near to the front:
				virtual TVSpace<T> *expand(TVSpace<T> *structure)=0;

				/**************
				 * Accessors: *
				 **************/

				// Getter for the process:
				const TVProcess<T> *getProcess() const {
					// Returning:
					return _pro;
				}

				// Getter and setter for the dt parameter:
				T getDt() const {
					// Returning:
					return _dt;
				}
				TVMarch<T> *setDt(T dt) {
					// Setting:
					_dt = dt;

					// Returning:
					return this;
				}

				/***************************
				 * Tools implemented here: *
				 ***************************/

				// Creating a new front starting from the process' space:
				template <class EXPANDER>
				TVSpace<T> *createFront() {
					// Getting the parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// Getting the original space and a new one:
					TVSpace<T> *sp = _pro->getSpace(),
						*nsp = _pro->getFactory()->createSpace(NULL,0,_pro->getSpace()->getDim());

					// Iterating on objects:
					TVObject<T>::iterator_t *i;
					EXPANDER *expa;
					for (i=sp->getIterator(); i->hasActual(); i->next()) {
						// Adding a cloned object:
						nsp->add(expa=new EXPANDER(i->getActual()));
					}
					delete i;

					// If required, correcting the points positions:
					long corrections = pars->getParameterInt(TVPARI_MARCH_FRONT_CORRECTIONS);
					if (corrections>0) {
						// Correcting the front:
						TVSpace<T> *sp = _pro->getSpace();
						_pro->setSpace(nsp);
						vector_dim_t dim = sp->getDim(), strOrd = expa->getStructureDim();
						ITVCorrector<T> *corr = new TVCorrMaximizer<T>(_maximizer);
						// Iterating the correction:
						for (int j=0; j<corrections; j++) {
							// A single correction step:
							corr->correct(_pro,dim-strOrd);
						}
						delete corr;
						sp->clean();
						sp->add(nsp);
						_pro->setSpace(sp);
					}

					// Returning:
					return nsp;
				}

			protected:
				/********************
				 * Protected tools: *
				 ********************/

			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVMARCH_H_
