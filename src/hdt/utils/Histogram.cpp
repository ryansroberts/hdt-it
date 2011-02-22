/*
 * Histogram.cpp
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

#include "Histogram.hpp"

namespace utils {

	class Histogram : public IHistogram
	{
		private:
			double start, nBinsByInterval;
			unsigned int nBins;
			unsigned int number;
			unsigned int* freq;
			double minValue, maxValue, mean, deviation, total;

		public:
			/**
			 * Creates a histogram that can count within a range of values. All bins of the histogram are set to zero.
			 *
			 * @param start Description of the param.
			 * @param End Description of the param.
			 * @param nBins Description of the param.
			 */
			Histogram(const double& start, const double& end, const unsigned int& nBins) :
				start(start), nBinsByInterval(nBins / (end - start)), nBins(nBins),
						freq(new unsigned int[nBins]) {
				reset();
			}

			/**
			 * Creates a histogram by copying from another one.
			 *
			 * @param other Description of the param.
			 */
			Histogram(const Histogram& other) :
				start(other.start), nBinsByInterval(other.nBinsByInterval), nBins(
						other.nBins), freq(new unsigned int[nBins]) {
				for (unsigned int i(0); i < nBins; ++i)
					freq[i] = other.freq[i];
			}

			/**
			 * Destructor for Histogram object
			 */
			~Histogram() {
				delete[] freq;
			}

			/**
			 * Sets this histogram equal to another.
			 *
			 * @param other Description of the param.
			 * @return The expected result.
			 */
			Histogram& operator=(const Histogram& other) {
				if (this != &other) {
					start = other.start;
					nBinsByInterval = other.nBinsByInterval;
					if (nBins != other.nBins) {
						nBins = other.nBins;
						delete[] freq;
						freq = new unsigned int[nBins];
					}
					for (unsigned int i(0); i < nBins; ++i)
						freq[i] = other.freq[i];
				}
				return *this;
			}

			/**
			 * Add
			 *
			 * @param x Description of the param.
			 * @param ntimes Description of the param.
			 * @return void
			 */
			void add(const double& x, int ntimes) {
				number += ntimes;
				total += (x * ntimes);
				mean += (x * ntimes);
				deviation += (x * x) * ntimes;
				minValue = min(minValue, x);
				maxValue = max(maxValue, x);

				const unsigned int i(static_cast<unsigned int> ((x - start)
						* nBinsByInterval));

				if (i < nBins) {
					freq[i] += ntimes;
				}
			}

			/**
			 * Increases the count for the bin that holds a value that is in range for this histogram.
			 *
			 * @param x Description of the param
			 * @return void
			 */
			void add(const double& x) {
				add(x, 1);
			}

			/**
			 * End
			 *
			 * @return void
			 */
			void end() {
				mean = mean / number;
				deviation = deviation / number - mean * mean;
				deviation = sqrt(deviation);
			}

			/**
			 * Dumps the histogram as a string
			 *
			 * @param outfile Description of the param.
			 * @return void
			 */
			void dumpString(ostream &outfile) {
				int maxfreq = 0;
				for (unsigned int i = 0; i < nBins && i <= maxValue; i++) {
					outfile << i << "  " << freq[i] << endl;
					maxfreq = freq[i] > maxfreq ? freq[i] : maxfreq;
				}

				outfile << "# Number: " << number << endl;
				outfile << "# Mean: " << mean << endl;
				outfile << "# Deviation: " << deviation << endl;
				outfile << "# Min: " << minValue << endl;
				outfile << "# Max: " << maxValue << endl;

				unsigned int max = 15;
				for (unsigned int i = maxValue < nBins ? maxValue : nBins; i > 15; i--) {
					if (freq[i] > maxfreq / 10000) {
						max = i;
						break;
					}
				}
				outfile << "# Latest: " << max << endl;
			}

			/**
			 * Dump
			 *
			 * @param name Description of the param.
			 * @param suffix Description of the param.
			 * @return void
			 */
			void dump(const char *name, const char *suffix) {
				string fileName(name);
				fileName.append(suffix);
				ofstream outfile;

				cout << "Writing histogram to: " << fileName << endl;

				outfile.open(fileName.c_str());

				dumpString(outfile);

				outfile.close();
			}

			/**
			 * Returns the sum of all counts in the histogram.
			 *
			 * @return The expected result.
			 */
			const unsigned int getTotalCount() const {
				unsigned int c(0U);
				for (unsigned int i(0); i < nBins; ++i)
					c += freq[i];
				return c;
			}

	};

}
