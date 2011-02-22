/*
 * PlainTriples.cpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
 * All rights reserved.
 *
 * TODO: Define me
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
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *   Guillermo Rodriguez-Cano:  wileeam@acm.org
 *   Alejandro Andres:          fuzzy.alej@gmail.com
 *   Mario Arias:               mario.arias@gmail.com
 *
 * @version $Id$
 *
 */

#include "PlainTriples.hpp"

namespace triples {

	PlainTriples::PlainTriples() {
		// TODO Auto-generated constructor stub
	} // PlainTriples()

	PlainTriples::~PlainTriples() {
		// TODO Auto-generated destructor stub
	} //PlainTriples()

	PlainTriples::insert(TripleID *triple) {

		// TODO : Check if element is already added
		this.listOfTriples.push_back(triple);

		return true;

	} // insert()

	PlainTriples::insert(vector<TripleID> triples) {

		int vectorSize = triples.size();

		// TODO : Check if any of the elements are already added
		for (int i = 0; i < vectorSize; i++) {
			this.listOfTriples.push_back(triples.at(i));
		}

		return true;

	} // insert()

	PlainTriples::retrieve(TripleID *triple) {
		//TODO : Complete...
		vector<TripleID>::iterator res;
		res = this.listOfTriples().begin();

		return res;

	} // retrieve()

} // triples{}
