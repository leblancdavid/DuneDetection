
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

#ifndef _TVFACPROTO_H_
#define _TVFACPROTO_H_

/*
 *  Here the factory implementation with prototypes.
 */

// All this in the TVF::ProcessTools namespace:
namespace TVF {
	namespace ProcessTools {

		// Namespaces used:
		using namespace TVF::Math;

		template <class T>
		class TVFacProto : public TVFactory<T> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The object prototype:
			TVObject<T> *_objProto;

			// The space prototype:
			TVSpace<T> *_spaceProto;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVFacProto(TVObject<T> *obj=NULL, TVSpace<T> *space=NULL) 
				: _objProto(obj), _spaceProto(space) {}

			// Desctructor:
			~TVFacProto() {
				// Deleting the object:
				delete _objProto;

				// Deleting the space:
				delete _spaceProto;
			}

			/************************************
			 * Accessor methods for prototypes: *
			 ************************************/

			// Getter and setter for the object:
			TVObject<T> *getObjProto() const {
				// Returning:
				return _objProto;
			}
			void setObjProto(TVObject<T> *obj) {
				// Removing the old one:
				if (_objProto!=NULL) {
					// Deleting:
					delete _objProto;
				}

				// Setting:
				_objProto = obj;

				// Saving properties:
				TVParameters<T> *params = TVParameters<T>::getInstance()
					->setParameterInt(TVPARI_REMS_NUM,_obj->getNrems());
			}

			// Getter and setter for the space:
			TVSpace<T> *getSpaceProto() const {
				// Returning:
				return _spaceProto;
			}
			void setSpaceProto(TVSpace<T> *space) {
				// Removing the old one:
				if (_spaceProto!=NULL) {
					// Deleting:
					delete _spaceProto;
				}

				// Setting:
				_spaceProto = space;
			}

			/*************************************
			 * Abstract methods' implementation: *
			 *************************************/

			// Creation of an object:
			TVObject<T> *createObject(T *coords) const {
				// Creating a new object:
				if (_objProto!=NULL) {
					// Generating a new instance:
					return _objProto->newInstance(coords);
				}
				else {
					// Cannot instantiate:
					return NULL;
				}
			}

			// Creation of a space:
			TVSpace<T> *createSpace(T *data, vector_ind_t N, vector_dim_t dim) const {
				// Creating a new space:
				if (_spaceProto!=NULL) {
					// The new space:
					TVSpace<T> *sp;

					// Indexes:
					vector_ind_t i;

					// Creating the space:
					sp = _spaceProto->newInstance(dim);

					// Adding the set of data:
					if (N>0) {
						// Iterating on points:
						for (i=0; i<N; i++,data+=dim) {
							// Adding an object:
							sp->add(this->createObject(data));
						}

						// Compacting the space:
						sp->compact();
					}

					// Returning it:
					return sp;
				}
				else {
					// Cannot instantiate:
					return NULL;
				}
			}
		};

		/*
		 *  Useful declaration of a template tool to generate quickly a factory:
		 */
		// Remitters creation:
		template <class T,class RemsTList,class DecayTList>
		struct _createTVFacProto_createRems {
			static void doIt(TVObject<T> *obj,TVRemitter<T> **rems) {
				// Creating my remitter:
				rems[0] = new RemsTList::car(obj);

				// Setting the decay:
				rems[0]->setDecay(new DecayTList::car);

				// Other remitters:
				_createTVFacProto_createRems<T,RemsTList::cdr,DecayTList::cdr>::doIt(obj,rems+1);
			}
		};
		template <class T,class RemsTList>
		struct _createTVFacProto_createRems<T,RemsTList,Nil> {
			static void doIt(TVObject<T> *obj,TVRemitter<T> **rems) {
				// Creating my remitter:
				rems[0] = new RemsTList::car(obj);

				// Other remitters:
				_createTVFacProto_createRems<T,RemsTList::cdr,Nil>::doIt(obj,rems+1);
			}
		};
		template <class T>
		struct _createTVFacProto_createRems<T,Nil,Nil> {
			static void doIt(TVObject<T> *obj,TVRemitter<T> **rems) {}
		};
		// Packaging:
		template <class T,class RemsTList,class DecayTList,class Space>
		TVFacProto<T> *createTVFacProto(bool aggregated=true) {
			// Declaring the remitters list:
			TVRemitter<T> **rems = new TVRemitter<T>*[TVTLLength<RemsTList>::RES];

			// Creating the prototipical object:
			TVObject<T> *obj = new TVObject<T>(0,aggregated);

			// Generating the remitters:
			_createTVFacProto_createRems<T,RemsTList,DecayTList>::doIt(obj,rems);

			// Setting the remitters in the object:
			obj->setRemitters(rems,TVTLLength<RemsTList>::RES);

			// Creating the space:
			TVSpace<T> *space = new Space;

			// Creating the factory:
			TVFacProto<T> *fac = new TVFacProto<T>(obj,space);

			// Returning:
			return fac;
		}
		template <class T,class TypesList,class Space>
		inline TVFacProto<T> *createTVFacProtoPredef(bool aggregated=true) {
			// Calling the correct function:
			return createTVFacProto<T,TypesList::Remitters,TypesList::Decays,Space>(aggregated);
		}

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVFACPROTO_H_
