/*
 * Utilities.cpp
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

#include "Utilities.hpp"

namespace utils {

	class Utilities : public IUtilities
	{
		public:
			/*
			 * Function with behaviour like `mkdir -p'
			 *
			 * @param s Description of the param.
			 * @param mode Description of the param.
			 * @return The expected result
			 */
			static int mkpath(const char *s, mode_t mode) {

				if (strcmp(s, ".") == 0 || strcmp(s, "/") == 0)
					return (0);

				if (index(s, '/') != NULL) {
					char *copy = strdup(s);
					char *base = strdup(dirname(copy));

					mkpath(base, mode);

					//printf("Creating %s\n", s);
					mkdir(s, mode);

					free(copy);
					free(base);
				} else {
					//printf("Creating %s\n", s);
					mkdir(s, mode);
				}

				return 0;

			}

			/**
			 * Creates full path except for the last one, which is considered a file
			 *
			 * @param s Description of the param.
			 * @param mode Description of the param.
			 * @return The expected result
			 */
			static int mkpathfile(const char *s, mode_t mode) {

				int len = strlen(s);

				if (s[len - 1] == '/') {
					mkpath(s, mode);
				} else {
					char *copy = strdup(s);
					char *base = strdup(dirname(copy));

					mkpath(base, mode);

					free(copy);
					free(base);
				}

			}

			/**
			 * Checks whether a string has an ending or not
			 *
			 * @param full Description of the param.
			 * @param ending Description of the param.
			 * @return The expected result
			 */
			static bool stringHasEnding(string const &full, string const &ending) {
				return (full.length() > ending.length()) && (!full.compare(full.length() - ending.length(), ending.length(), ending));
			}

	};
}
