/* CSD_PCF.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * This class implements a VByte-oriented Front Coding technique for 
 * compression of string dictionaries.
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
 * Contacting the author:
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */


#ifndef _CSDCACHE2_H
#define _CSDCACHE2_H

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <string.h>
#include <set>

using namespace std;

#include <HDTListener.hpp>

#include <libcdsBasics.h>
using namespace cds_utils;

#include "CSD.h"

namespace csd
{

class CSD_Cache2 : public CSD
{
private:
	CSD *child;
	vector<uchar *> array;

  public:		
    /** General constructor **/
	CSD_Cache2(CSD *child);

    /** General destructor. */
    ~CSD_Cache2();
    
    /** Returns the ID that identify s[1..length]. If it does not exist,
	returns 0.
	@s: the string to be located.
	@len: the length (in characters) of the string s.
    */
    uint32_t locate(const uchar *s, uint32_t len);

    /** Returns the string identified by id.
	@id: the identifier to be extracted.
    */
    uchar * extract(uint32_t id);

    /** Obtains the original Tdict from its CSD_PFC representation. Each string is
	separated by '\n' symbols.
	@dict: the plain uncompressed dictionary.
	@return: number of total symbols in the dictionary.
    */
    uint decompress(uchar **dict);

    /** Returns the size of the structure in bytes. */
    uint64_t getSize();

    /** Stores a CSD_PFC structure given a file pointer.
	@fp: pointer to the file saving a CSD_PFC structure.
    */
    void save(ofstream & fp);

    /** Loads a CSD_PFC structure from a file pointer.
	@fp: pointer to the file storing a CSD_PFC structure. */
    static CSD * load(ifstream & fp);

    void fillSuggestions(const char *base, vector<string> &out, int maxResults) {
    	child->fillSuggestions(base, out, maxResults);
    }

    void dumpAll() {
    	child->dumpAll();
    }

    CSD *getChild() {
    	return child;
    }
  };
};

#endif  
