
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

#ifndef _TVRELABEL_H_
#define _TVRELABEL_H_

// All this in the TVF namespace:
namespace TVF {
	namespace Extensions {
		namespace Labelling {

			/*
			*  This remitter allows to group objects using second order tensors
			* and saliency/decay values.
			*/

			// Usings:
			using namespace TVF::DataStructures;

			// The class:
			template <class T>
			class TVRELabel : public TVRemitter<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// My label:
				vector_ind_t _label;

				// My labels' group:
				TVRELabellingGroup<T> *_labelsGroup;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVRELabel(TVObject<T> *obj,TVRELabellingGroup<T> *labelsGroup=NULL) : 
				TVRemitter<T>(obj), _label(0), _labelsGroup(labelsGroup) {}

				// Destructor:
				~TVRELabel() {}

				/*************************************
				* Managment of the decay functions: *
				*************************************/

				// Setting the decay function:
				virtual void setDecay(ITVDecayFun<T> *func) {
					// Ignoring:
					if (func) delete func;
				}

				// Getting the decay function:
				virtual ITVDecayFun<T> *getDecay() const {
					// No decay:
					return NULL;
				}

				/*****************
				* Voting tools: *
				*****************/

				// Resetting the label:
				virtual void reset() {
					// No label:
					_label = 0;
				}

				// Initialization of the voting process:
				virtual void init() {
					// Resetting:
					_label = 0;
				}

				/*
				*  Voting:
				* EMITTER:	rem
				* RECEIVER:	this
				*/
				virtual void vote(TVRemitter<T> *rem) {
					// Getting infos:
					TVRELabel<T> *re = dynamic_cast<TVRELabel<T>*>(rem);
					vector_ind_t emClass = re->getClass();
					if (!_labelsGroup) return;

					// Checking if the voter has a label:
					if (emClass==0) {
						// Assigning a new label to it:
						emClass = _labelsGroup->getFreeLabel();
						re->setClass(emClass);
					}

					// Is this object equal to the given one?
					if (equal(re)) {
						// Is my label null?
						if (_label==0) {
							// My class is the same:
							_label = emClass;
						}
						else {
							// Set them as equal:
							_labelsGroup->getEqSet()->equivalence(_label,emClass);
						}
					}
				}

				// Computing the saliencies:
				virtual T *computeSals(T *buff) const {
					// The array to be returned:
					if (buff==NULL) 
						buff = vec_create<T>(getSaliencyNum());
					
					// Looking up my class:
					*buff = (T)getClass();

					// Returning:
					return buff;
				}

				// Sayng the number of saliency values generated:
				virtual vector_dim_t getSaliencyNum() const {
					// Returning:
					return 1;
				}

				// Printing:
				virtual void fprint(FILE *fp=stderr) const {
					// The real class' label:
					fprintf(fp,"%d",getClass());
				}

				/*********************
				* Abstract methods: *
				*********************/

				// The association predicate:
				virtual bool equal(TVRELabel<T> *rem) const=0;

				/**************
				* Accessors: *
				**************/

				// Getter for my class:
				vector_ind_t getClass() const {
					// Returning:
					return _labelsGroup? _labelsGroup->getClass(_label): 0;
				}

				// Setting the class of this object:
				TVRELabel<T> *setClass(vector_ind_t label) {
					// Setting and adding it:
					_label = label;
					_labelsGroup->getEqSet()->add(label);

					// Returning:
					return this;
				}

				// Getter and setter for the labelling group class:
				TVRELabellingGroup<T> *getLabellingGroup() const {
					// Returning:
					return _labelsGroup;
				}
				TVRELabel<T> *setLabellingGroup(TVRELabellingGroup<T> *labelsGroup) {
					// Setting:
					_labelsGroup = labelsGroup;

					// Returning:
					return this;
				}
			};

		} // namespace Labelling
	} // namespace Extensions
} // namespace TVF

#endif //ndef _TVRELABEL_H_
