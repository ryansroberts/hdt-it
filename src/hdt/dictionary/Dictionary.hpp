/*
 * Dictionary.hpp
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

#ifndef DICTIONARY_HPP_
#define DICTIONARY_HPP_

// System includes
#include <string>

// HDT includes
//#include "Basics.h"

namespace dictionary {

	class IDictionary
	{
		public:
			virtual ~Dictionary();
			virtual string idToString(unsigned int id, TriplePosition *position);
			virtual unsigned int stringToId(string *str, TriplePosition *position);
			virtual boolean save(ostream &output);
			virtual void load(istream &input);
			virtual unsigned int insert(string *str, TriplePosition *position);
			virtual unsigned int numberOfElements();
			virtual void startProcessing();
			virtual void stopProcessing();
	}; // IDictionary{}

} // dictionary{}

#endif  /* DICTIONARY_HPP_ */
