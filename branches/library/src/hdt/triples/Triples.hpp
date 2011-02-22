/*
 * Triples.hpp
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

#ifndef TRIPLES_HPP_
#define TRIPLES_HPP_

#include <iostream>
#include <iterator>
#include <vector>

#include "TripleID.hpp"


using namespace std;

namespace triples {

	class ITriples
	{
		public:
			/**
		     * Returns a vector of triples matching the pattern
		     *
		     * @param triple
		     * @return
		     */
			virtual iterator<TripleID> retrieve(TripleID *triple);

			/**
			 * Adds a single triple to
		     *
		     * @param triple
		     * @return
		     */
			virtual boolean insert(TripleID *triple);

			/**
			 * Adds a collection of triples
			 *
			 * @param triples
			 * @return
			 */
			virtual boolean insert(vector<TripleID> *triples);

			/**
			 * Calculates the cost to retrieve a specific pattern
		     *
		     * @param triple
		     * @return
		     */
			virtual float cost(TripleID *triple);

			/**
		     * Returns the number of triples
		     *
		     * @return
		     */
			virtual int numberOfTriples();

			/**
		     * Saves the triples
		     *
		     * @param output
		     * @return
		     */
			virtual boolean save(ostream output);

			/**
		     * Loads triples from a file
		     *
		     * @param input
		     * @return
		     */
			virtual void load(istream input);
	}; // ITriples{}

} // triples{}

#endif /* TRIPLES_HPP_ */
