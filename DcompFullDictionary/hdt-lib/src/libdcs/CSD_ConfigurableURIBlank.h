/* CSD_ConfigurableURIBlank.h
 * Copyright (C) 2012 Javier D. Fernández & Mario Arias & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class for implementing Compressed String Dictionaries following:
 *
 *   ==========================================================================
 *     "Compressed String Dictionaries"
 *     Nieves R. Brisaboa, Rodrigo Canovas, Francisco Claude, 
 *     Miguel A. Martinez-Prieto and Gonzalo Navarro.
 *     10th Symposium on Experimental Algorithms (SEA'2011), p.136-147, 2011.
 *   ==========================================================================
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 */

#ifndef _CSDCONFIGURABLEURIBLANK_H
#define _CSDCONFIGURABLEURIBLANK_H

#include "CSD.h"

#include <Iterator.hpp>

#include <set>
using namespace std;

namespace csd {

class CSD_ConfigurableURIBlank: public CSD {



private:

	csd::CSD *URIs;
	csd::CSD *Blanks;

public:
	/** General constructor **/
	CSD_ConfigurableURIBlank();

	/** Constructor receiving Tdict as a sequence of 'tlength' uchars. Tdict
	 */
	CSD_ConfigurableURIBlank(csd::CSD *URIs, csd::CSD *Blanks,
			hdt::IteratorUCharString *it,
			hdt::ProgressListener *listener = NULL);

	/** Returns the ID that identify s[1..length]. If it does not exist,
	 * returns 0.
	 * @s: the string to be located.
	 * @len: the length (in characters) of the string s.
	 * */
	uint32_t locate(const unsigned char *s, uint32_t len);

	/** Returns the string identified by id.
	 * @id: the identifier to be extracted.
	 **/
	unsigned char * extract(uint32_t id);

	void freeString(const unsigned char *str);

	unsigned int decompress(unsigned char **dict);

	void dumpAll();

	/** Returns the size of the structure in bytes. */
	uint64_t getSize();

	/** Stores a CSD_ConfigurableURIBlank structure given a file pointer.
	 * @fp: pointer to the file saving a CSD_ConfigurableURIBlank structure.
	 * */
	void save(ostream & fp);

	size_t load(unsigned char *ptr, unsigned char *ptrMax);

	/** Loads a CSD_ConfigurableURIBlank structure from a file pointer.
	 * @fp: pointer to the file storing a CSD_ConfigurableURIBlank structure. */
	static CSD * load(istream & fp);

	void fillSuggestions(const char *base, vector<string> &out, int maxResults);

	hdt::IteratorUCharString *listAll() {
		throw "Not implemented";
	}

	/** General destructor. */
	~CSD_ConfigurableURIBlank();

protected:
	uint32_t limitID; //lastID of the first CSD container

};

}
;
#endif  /* _CSDCONFIGURABLEURIBLANK_H */
