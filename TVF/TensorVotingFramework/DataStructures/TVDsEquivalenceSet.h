
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

#ifndef _TVDSEQUIVALENCESET_H_
#define _TVDSEQUIVALENCESET_H_

/*
 *  This data structure represents an associative set where equivalence
 * between elements can defined and representants can be retrieved.
 */

// All this in the plain TVF namespace:
namespace TVF {
	namespace DataStructures {

		// The usings:
		using namespace std;

		// Definition of the set:
		template <class LABEL>
		class TVDsEquivalenceSet {
		protected:
			/*********************
			 * Class prototypes: *
			 *********************/

			// The equivalence node:
			class TVDsEquivalenceSetNode;

			/**********
			* Types: *
			**********/

			// A map type:
			typedef hash_map<LABEL,TVDsEquivalenceSetNode*> _hash_t;

			// A key/value pair:
			typedef pair<LABEL,TVDsEquivalenceSetNode*> _hash_pair;

			// An hash iterator:
			typedef typename _hash_t::iterator _hash_iterator;

			/***************
			 * Attributes: *
			 ***************/

			// All the nodes are inside an hash map:
			_hash_t *_nodes;

			// The null value:
			LABEL _null;

			// The number of classes:
			vector_ind_t _numClasses;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVDsEquivalenceSet(LABEL null) : _nodes(NULL), _null(null), _numClasses(0) {}

			// Destructor:
			~TVDsEquivalenceSet() {
				// Resetting:
				reset();
			}

			/***************************
			 * Structure manipulation: *
			 ***************************/

			// Resetting the eqSet:
			void reset() {
				// Check:
				if (_nodes) {
					// Deleting all the nodes:
					for (_hash_iterator i=_nodes->begin(); i!=_nodes->end(); i++) {
						// Deleting the element:
						delete i->second;
					}

					// Deleting the hash map:
					delete _nodes;
					_nodes = NULL;
				}

				// No classes:
				_numClasses = 0;
			}

			// Adding an element:
			void add(LABEL elem) {
				// Do nothing if it is null:
				if (elem==_null) return;

				// Creating the hash map:
				if (!_nodes) _nodes = new _hash_t;

				// Checking if the element is present:
				if (_nodes->find(elem)==_nodes->end()) {
					// Not found, adding it:
					_nodes->insert(_hash_pair(elem,new TVDsEquivalenceSetNode(elem)));

					// A new class:
					_numClasses++;
				}
			}

			// Equivalence between two (known) elements:
			void equivalence(LABEL el1, LABEL el2) {
				// Do nothing if they are equal or null:
				if (el1==el2 || el1==_null || el2==_null) return;

				// The nodes:
				TVDsEquivalenceSetNode *n1, *n2;

				// An iterator:
				_hash_iterator i;

				// Creating the hash map:
				if (!_nodes) _nodes = new _hash_t;

				// Checking if the elements are present:
				i = _nodes->find(el1);
				if (i==_nodes->end()) {
					// Adding it:
					n1 = new TVDsEquivalenceSetNode(el1);
					_nodes->insert(_hash_pair(el1,n1));
					_numClasses++;
				}
				else {
					// Getting the node:
					n1 = i->second;
				}
				i = _nodes->find(el2);
				if (i==_nodes->end()) {
					// Adding it:
					n2 = new TVDsEquivalenceSetNode(el2);
					_nodes->insert(_hash_pair(el2,n2));
					_numClasses++;
				}
				else {
					// Getting the node:
					n2 = i->second;
				}

				// Setting the equivalence:
				n1 = n1->getClass();
				n2 = n2->getClass();
				if (n1!=n2) {
					// A real new equivalence:
					n2->setClass(n1);
					_numClasses--;
				}
			}

			// Getting the class of a given label:
			LABEL getClass(LABEL elem) {
				// Creating the hash map:
				if (!_nodes) _nodes = new _hash_t;

				// An iterator:
				_hash_iterator i = _nodes->find(elem);

				// Checking if the element is present:
				if (i==_nodes->end()) {
					// Cannot find the class:
					return _null;
				}
				else {
					// Returning the class:
					return i->second->getClassLabel();
				}
			}

			// Returning the number of classes:
			vector_ind_t getNumClasses() {
				// Returning:
				return _numClasses;
			}

			// Returning the null value:
			LABEL getNull() const {
				// Returning:
				return _null;
			}

			// Printing:
			void fprint(FILE *fp=stderr) const {
				// Checking:
				if (_nodes) {
					// Iterating on the nodes:
					TVDsEquivalenceSetNode *n;
					for (_hash_iterator i=_nodes->begin(); i!=_nodes->end(); i++) {
						// Printing the elements:
						// fprintf(fp,"%d(%d)\n",i->second->getLabel(),i->second->getClassLabel());
						n = i->second;
						fprintf(fp,"%d",n->getLabel());
						while (n!=n->getDirectClass()) {
							n = n->getDirectClass();
							fprintf(fp,"->%d",n->getLabel());
						}
						fprintf(fp,"]\n");
					}
				}
			}

		protected:
			/********************************************************************
			*                       The nodes' structure:                      *
			********************************************************************/

			class TVDsEquivalenceSetNode {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The label:
				LABEL _label;

				// The class' representative:
				TVDsEquivalenceSetNode *_class;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVDsEquivalenceSetNode(LABEL label) : _label(label) {
					// I'm a new class:
					_class = this;
				}

				// Destructor:
				~TVDsEquivalenceSetNode() {}

				/**************
				 * Accessors: *
				 **************/

				// Getter and setter for the class:
				TVDsEquivalenceSetNode *getClass() const {
					// Returning the class:
					if (_class==this) {
						// Base case:
						return _class;
					}
					else {
						// Recursive case:
						return _class->getClass();
					}
				}
				void setClass(TVDsEquivalenceSetNode *cl) {
					// Check:
					if (cl) {
						// Real classes:
						TVDsEquivalenceSetNode	*n1 = getClass(),
												*n2 = cl->getClass();

						// Setting:
						n1->setDirectClass(n2);
					}
				}
				void setDirectClass(TVDsEquivalenceSetNode *cl) {
					// Base case:
					_class = cl? cl: this;
				}
				TVDsEquivalenceSetNode *getDirectClass() const {
					// Base case:
					return _class;
				}

				// Getting the label:
				LABEL getLabel() const {
					// Returning:
					return _label;
				}
				LABEL getClassLabel() const {
					// Returning:
					return getClass()->getLabel();
				}
			};
		};

	} // namespace DataStructures
} // namespace TVF

#endif //ndef _TVDSEQUIVALENCESET_H_
