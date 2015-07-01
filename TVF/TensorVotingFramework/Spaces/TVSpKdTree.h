
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

#ifndef _TVSPKDTREE_H_
#define _TVSPKDTREE_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace DataStructures {
		namespace Spaces {

			// Usings:
			using namespace std;

			/*
			*  This class represents a space data structure similar to the well-known
			* kd-tree: this kd-tree variation selects the splitting axis for each node
			* as the axis where there is the nearest splitting axis of a pre-existent
			* point, this can be computed during the traversal of the tree. Another
			* variation is the balanced reinsertion of three nodes unbalanced when a
			* new node is inserted: if a new node is inserted, its final position is
			* found traversing the tree; id its father and its grandfather are both
			* single-child-nodes the three nodes can be rearranged to obtain a locally
			* balanced tree, this alows to obtain a lower final height of the tree.
			*/

			template <class T>
			class TVSpKdTree : public TVSpVector<T> {
			protected:
				/*********************
				* Class prototypes: *
				*********************/

				// The tree node:
				class TVSpKdTreeNode;

				/***************
				* Attributes: *
				***************/

				// The tree:
				TVSpKdTreeNode *_tree;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVSpKdTree(vector_dim_t dim=0) : 
					TVSpVector<T>(dim), _tree(NULL) {}

				// Destructor:
				~TVSpKdTree() {
					// Deleting the tree:
					delete _tree;
				}

				// Generating a new (empty) instance:
				virtual TVSpace<T> *newInstance(vector_dim_t dim=0) const {
					// Creating a new vector:
					if (dim==0) dim = _dim;
					return new TVSpKdTree<T>(dim);
				}

				/***************************
				* Structure manipulation: *
				***************************/

				// Adding an object:
				virtual void add(TVObject<T> *obj) {
					// Adding the object using the superclass:
					TVSpVector<T>::add(obj);

					// Adding it to the tree:
					if (_tree) {
						// Adding:
						_tree->add(obj);
					}
					else {
						// Creating a new tree:
						_tree = new TVSpKdTreeNode(obj,_dim);
					}
				}

				// Extracting an object:
				virtual TVObject<T> *extract(TVObject<T> *obj) {
					// Removing the object from the tree:
					_tree->remove(obj,&_tree);

					// The superclass remove:
					return TVSpVector<T>::extract(obj);
				}

				// Extracting all the objects:
				virtual void empty() {
					// Cleaning the tree:
					delete _tree;
					_tree = NULL;

					// The clean for the super class:
					TVSpVector<T>::empty();
				}

				// Cleaning (removing the objects):
				virtual void clean() {
					// Cleaning the tree:
					delete _tree;
					_tree = NULL;

					// The clean for the super class:
					TVSpVector<T>::clean();
				}

				// Getting a range of objects:
				virtual typename TVObject<T>::iterator_t *range(T *center, T radius=-1) const {
					// Managing the radius:
					if (radius<0) {
						// Getting the default one:
						radius = TVParameters<T>::getInstance()->getParameter(TVPAR_RADIUS);
					}

					// Creating the vector for objects:
					vector<TVObject<T>*> *vec = new vector<TVObject<T>*>;

					// Collecting objects with the range query:
					if (_tree) {
						// Collecting:
						_tree->rangeCollect(vec,center,radius);
					}

					// The iterator:
					return new TVContainerIterator<vector<TVObject<T>*>,TVObject<T>*>(vec);
				}

			protected:
				/********************************************************************
				*                       The nodes' structure:                      *
				********************************************************************/

				/*
					*  This class represents a node of the tree.
					*/
				class TVSpKdTreeNode {
				protected:
					/***************
					* Attributes: *
					***************/

					// The object contained in this node:
					TVObject<T> *_obj;

					// The dimension of the split:
					vector_dim_t _split;

					// The dimensionality of the space:
					vector_dim_t _dim;

					// Greater or equal pointer:
					TVSpKdTreeNode *_geLink;

					// Lower pointer:
					TVSpKdTreeNode *_lLink;

				public:
					/********************************
					* Constructors and destructor: *
					********************************/

					/*
					 * Default constructor for a new node without the split direction selected.
					 */
					TVSpKdTreeNode(TVObject<T> *obj,vector_dim_t dim) : 
						_obj(obj), _split(0), _dim(dim), _geLink(NULL), _lLink(NULL) {}

					/*
					 *  Deleting a node corresponds to detatching an object without deletion (it can be
					 * done from outside) and deleting all the sub-nodes recursively:
					 */
					~TVSpKdTreeNode() {
						// Cleaning:
						clean();
					}

					/***************************
					 * Structure manipulation: *
					 ***************************/

					// Adding an object:
					void add(TVObject<T> *obj) {
						add(new TVSpKdTreeNode(obj,_dim));
					}

					// Adding a node:
					void add(TVSpKdTreeNode *node) {
						// The pointer to the next pointer:
						TVSpKdTreeNode **next;
						const T *yourPos = node->getPos(),
								*myPos = _obj->getPos();

						// Checking if this is a leaf:
						if (_geLink==NULL && _lLink==NULL) {
							// This is a leaf, selecting the split direction:
							vector_dim_t i;
							T diff = ABS(yourPos[0]-myPos[0]),dif1;
							for (_split=0,i=1; i<_dim; i++) {
								// Is this better?
								dif1 = ABS(yourPos[i]-myPos[i]);
								if (dif1>diff) {
									// Changing:
									_split = i;
									diff = dif1;
								}
							}
						}

						// TODO: Add the code for the auto-balancing rotations.

						// Checking the direction:
						next = yourPos[_split]>=myPos[_split]? &_geLink: &_lLink;

						// Is this the destination?
						if (*next==NULL) {
							// Inserting it:
							*next = node;
						}
						else {
							// Inserting recursively:
							(*next)->add(node);
						}
					}

					// Locating the nearest node along an axis:
					TVSpKdTreeNode  *findNearestAlongAxis(TVObject<T> *obj,vector_dim_t axis) {
						// The result:
						TVSpKdTreeNode  *res = this, *tmp;
						const T *pos = obj->getPos(),
								*pos1 = res->getPos(),
								*pos2;

						// Cheking on the l side:
						if (_lLink!=NULL) {
							// Getting the nearest on this subtree:
							tmp = _lLink->findNearestAlongAxis(obj,axis);
							pos2 = tmp->getPos();

							// Selecting:
							if (ABS(pos2[axis]-pos[axis]) < ABS(pos1[axis]-pos[axis])) {
								// Updating:
								res = tmp;
								pos1 = res->getPos();
							}
						}

						// Cheking on the ge side:
						if (_geLink!=NULL) {
							// Getting the nearest on this subtree:
							tmp = _geLink->findNearestAlongAxis(obj,axis);
							pos2 = tmp->getPos();

							// Selecting:
							if (ABS(pos2[axis]-pos[axis]) < ABS(pos1[axis]-pos[axis])) {
								// Updating:
								res = tmp;
							}
						}

						// Returning:
						return res;
					}

					// Extracting a node with a certain object in it:
					TVSpKdTreeNode *extract(TVObject<T> *obj,TVSpKdTreeNode **parent) {
						// If this is the correct node:
						if (obj==_obj) {
							// This is the node to be extracted, checking the case:
							if (_lLink==NULL) {
								// Parent linked to child:
								*parent = _geLink;
							}
							else if (_geLink==NULL) {
								// Parent linked to child:
								*parent = _lLink;
							}
							else {
								// Worst case, both not NULL:
								// - Finding and extracting the node nearest to the actual:
								TVSpKdTreeNode	*node = _lLink->findNearestAlongAxis(obj,_split),
												*node2 = _geLink->findNearestAlongAxis(obj,_split);
								// - Selecting:
								if (	ABS(node2->getPos()[_split]-_obj->getPos()[_split]) <
										ABS(node->getPos()[_split]-_obj->getPos()[_split])) {
									// Selecting it:
									node = node2;
								}
								node2 = extract(node->getObject(),parent);

								// This existing node can be the new splitting node:
								*parent = node;
								node->setLLink(_lLink);
								node->setGELink(_geLink);
								node->setSplit(_split);
							}

							// Returning:
							return this;
						}
						else {
							// This is not the correct one, searching again:
							if (_lLink!=NULL || _geLink!=NULL) {
								if (obj->getPos()[_split]>=_obj->getPos()[_split]) {
									// The ge case:
									if (_geLink) return _geLink->extract(obj,&_geLink);
									else return NULL;
								}
								else {
									// The l case:
									if (_lLink) return _lLink->extract(obj,&_lLink);
									else return NULL;
								}
							}
							else {
								// This leaf node is not the correct one:
								return NULL;
							}
						}
					}

					// Removing from this node the given object (given the parent):
					void remove(TVObject<T> *obj,TVSpKdTreeNode **parent) {
						// Extracting the node:
						TVSpKdTreeNode *node = extract(obj,parent);

						// Isolation and removal:
						if (node) {
							// Isolating it:
							node->setLLink(NULL);
							node->setGELink(NULL);

							// Deleting it:
							delete node;
						}
					}

					// Removing all the subtree:
					void clean() {
						// Deleting recursively the sub-nodes:
						if (_geLink) {
							// Delete and set to NULL:
							delete _geLink;
							_geLink = NULL;
						}
						if (_lLink) {
							// Delete and set to NULL:
							delete _lLink;
							_lLink = NULL;
						}
					}

					// Collecting objects for the range query:
					void rangeCollect(vector<TVObject<T>*> *vec,T *center,T radius) {
						// Position:
						const T *pos = _obj->getPos();
						bool interior = true;

						// Selecting the subnodes:
						if (pos[_split]<center[_split]-radius) {
							// Collecting:
							interior = false;
							if (_geLink) {
								// Collect and return:
								_geLink->rangeCollect(vec,center,radius);
								return;
							}
						}
						else if (pos[_split]>center[_split]+radius) {
							// Collecting:
							interior = false;
							if (_lLink) {
								// Collect and return:
								_lLink->rangeCollect(vec,center,radius);
								return;
							}
						}

						// Checking this node:
						if ((_lLink==NULL && _geLink==NULL) || interior) {
							// This node must be checked:
							if (vec_dist_L2_leq<T>(pos,center,radius*radius,_dim)) {
								// Saving the object:
								vec->push_back(_obj);
							}

							// Checking both the sub-nodes:
							if (_lLink) _lLink->rangeCollect(vec,center,radius);
							if (_geLink) _geLink->rangeCollect(vec,center,radius);
						}
					}

					/**************
					* Accessors: *
					**************/

					// Getter for the number of objects:
					vector_ind_t getSize() const {
						// Computing the size of the subtree:
						vector_ind_t res = 1;
						if (_geLink) res += _geLink->getSize();
						if (_lLink) res += _lLink->getSize();

						// Returning:
						return res;
					}

					// Getter for the dimensionality:
					vector_dim_t getDim() const {
						// Returning:
						return _dim;
					}

					// Getter and setter for the split position:
					vector_dim_t getSplit() const {
						// Returning:
						return _split;
					}
					void setSplit(const vector_dim_t split) {
						// Setting:
						_split = split;
					}

					// Getter for the position:
					const T *getPos() const {
						// Returning:
						return _obj->getPos();
					}

					// Getter for the object:
					TVObject<T> *getObject() const {
						// Returning:
						return _obj;
					}

					// Getters and setters for the links:
					TVSpKdTreeNode *getLLink() {
						// Returning:
						return _lLink;
					}
					TVSpKdTreeNode *getGELink() {
						// Returning:
						return _geLink;
					}
					void setLLink(TVSpKdTreeNode *link) {
						// Setting:
						_lLink = link;
					}
					void setGELink(TVSpKdTreeNode *link) {
						// Setting:
						_geLink = link;
					}

					// The link to the link to a specified node:
					TVSpKdTreeNode **getDirectLinkToNode(TVSpKdTreeNode *node) {
						// Check:
						if (node==_lLink) return &_lLink;
						if (node==_geLink) return &_geLink;

						// Bad:
						return NULL;
					}
				};
			};

		} // namespace Spaces
	} // namespace DataStructures
} // namespace TVF

#endif //ndef _TVSPKDTREE_H_
