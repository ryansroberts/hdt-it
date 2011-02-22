/*
 * Histogram.hpp
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

#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_

#include <math.h>
#include <limits>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace utils {

	class IHistogram {
		public:
			virtual void reset();
			virtual IHistogram& operator=(const IHistogram& other);
			virtual void add(const double& x);
			virtual void add(const double& x, int ntimes);
			virtual unsigned int getTotalCount();
			virtual void dump(const char *name, const char *suffix);
			virtual void dumpString(ostream &outfile);
	}; // IHistogram{}

} // utils{}

#endif /* HISTOGRAM_HPP_ */
