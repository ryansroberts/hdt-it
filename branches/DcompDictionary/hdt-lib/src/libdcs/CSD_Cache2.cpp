/* CSD_Cache2.h
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

#include <stdlib.h>

#include <string.h>
#include "CSD_Cache2.h"

namespace csd
{
CSD_Cache2::CSD_Cache2(CSD *child) : child(child)
{
	assert(child);
	length = child->getLength();

	array.resize(child->getLength(), NULL);
}


CSD_Cache2::~CSD_Cache2()
{
	delete child;
}

uint32_t CSD_Cache2::locate(const uchar *s, uint32_t len)
{
	// FIXME: Not implemented
	return child->locate(s, len);
}


uchar* CSD_Cache2::extract(uint32_t id)
{
	if(id<1 || id>array.size()) {
		return NULL;
	}

	if(array[id-1]!=NULL) {
		return array[id-1];
	}

	// Not found, fetch and add
	uchar *value = child->extract(id);

	array[id-1] = value;

	return value;
}

uint64_t CSD_Cache2::getSize()
{
	return child->getSize();
}

void CSD_Cache2::save(ofstream & fp)
{
	child->save(fp);
}

CSD* CSD_Cache2::load(ifstream & fp)
{
	throw "Not imlemented";
}

}
