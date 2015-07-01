
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

#ifndef _TVTERMWRITEMFILE_H_
#define _TVTERMWRITEMFILE_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Visitors {

			/*
			*  This class represent a general visitor for the remitter hierarchy.
			*/
			template <class T>
			class TVTermWriteMFile : public ITVTerminal<T> {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The filename (composition):
				char *_filename;

				// The varname (composition):
				char *_varname;

				// The file:
				FILE *_fp;

				// Actual remitter index:
				vector_ind_t _i;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVTermWriteMFile(char *filename=NULL,char *varname=NULL) : 
					_filename(strdup(filename)), _varname(strdup(varname)), _fp(NULL) {}

				// Deleting all:
				~TVTermWriteMFile() {
					// The names:
					if (_filename) delete _filename;
					if (_varname) delete _varname;
					if (_fp) fclose(_fp);
				}

				/************************************
				 * Implementation of the interface: *
				 ************************************/

				// Initialization:
				virtual void init() {
					// Initializing vars:
					_i = 1;

					// Opening the file:
					_fp = fopen(_filename,"w+");

					// Writing the header:
					fprintf(_fp,"%s = struct;\n\n",_varname);
				}

				// Initialization of the object section:
				virtual void initObjectSection(TVObject<T> *obj) {
					// Creating the object:
					fprintf(_fp,"%s(%d).P = [",_varname,_i);
					vec_fprint(_fp,obj->getPos(),obj->getDim());
					fprintf(_fp,"]';\n");
				}

				// Plot of the remitter:
				virtual void plotRemitter(TVRemitter<T> *rem) {
					// Checking:
					if (dynamic_cast<TVRETensor1<T> *>(rem)!=NULL) {
						// Calling:
						this->plotRemitter(dynamic_cast<TVRETensor1<T> *>(rem));
					}
					else if (dynamic_cast<TVRETensor2<T> *>(rem)!=NULL) {
						// Calling:
						this->plotRemitter(dynamic_cast<TVRETensor2<T> *>(rem));
					}
					// Ignoring other types of remitters.
				}

				// Plot of the T1 remitter:
				virtual void plotRemitter(TVRETensor1<T> *rem) {
					// Plotting:
					fprintf(_fp,"%s(%d).T{1} = [",_varname,_i);
					vec_fprint(_fp,rem->getTensor(),rem->getDim());
					fprintf(_fp,"]';\n");
				}

				// Plot of the T2 remitter:
				virtual void plotRemitter(TVRETensor2<T> *rem) {
					// Plotting:
					fprintf(_fp,"%s(%d).T{2} = [",_varname,_i);
					tns_fprint(_fp,rem->getTensor(),rem->getDim());
					fprintf(_fp,"];\n");
				}

				// Finalization of the object section:
				virtual void finalizeObjectSection() {
					// Next object:
					_i++;
				}

				// Finalization:
				virtual void finalize() {
					// Writing the footer:

					// Closing the file:
					fclose(_fp);
				}

				/**************
				 * Accessors: *
				 **************/

				// Getter and setter for the filename:
				const char *getFilename() const {
					// Returning:
					return _filename;
				}
				TVTermWriteMFile<T> *setFilename(char *filename) {
					// Chcking and saving:
					if (_filename) delete _filename;
					_filename = strdup(filename);

					// Returning:
					return this;
				}

				// Getter and setter for the varname:
				const char *getVarname() const {
					// Returning:
					return _varname;
				}
				TVTermWriteMFile<T> *setVarname(char *varname) {
					// Chcking and saving:
					if (_varname) delete _varname;
					_varname = strdup(varname);

					// Returning:
					return this;
				}
			};

		} // namespace Visitors
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVTERMWRITEMFILE_H_
