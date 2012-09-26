/* CSD_LiteralSplit.h
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

#ifndef _CSDLITERALSPLIT_H
#define _CSDLITERALSPLIT_H

#include "CSD.h"

#include <Iterator.hpp>
#include <algorithm>

#include <set>
#include <hash_map>

namespace std {
using namespace __gnu_cxx;
}

namespace csd {

struct LiteralDictionaryEntry {
public:
	std::vector<char *> strs;
	int previousIDs;

	bool static cmpLexicographic(LiteralDictionaryEntry *c1,
			LiteralDictionaryEntry *c2);
	bool static cmpID(LiteralDictionaryEntry *c1, LiteralDictionaryEntry *c2);
};
struct str_cmp {
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) == 0;
	}
};
typedef std::hash_map<const char *, LiteralDictionaryEntry *,
		hash<const char *>, str_cmp> LitDictEntryHash;
typedef LitDictEntryHash::const_iterator LitDictEntryIt;

typedef std::hash_map<const char *, CSD *, hash<const char *>, str_cmp> CSDDictEntryHash;
typedef CSDDictEntryHash::const_iterator CSDDictEntryIt;

class CSD_LiteralSplit: public CSD {

private:

	/* Auxiliars in the construction */
	LitDictEntryHash litIdioms;
	LitDictEntryHash litTypes;
	std::vector<char *> litArbitrary;

	/* Auxiliars to Id computation */
	std::vector<char *> difIdioms;
	std::vector<char *> difTypes;

	/* Dictionaries */
	CSDDictEntryHash CSDIdioms;
	CSDDictEntryHash CSDTypes;
	csd::CSD *CSDArbitrary;

	/* Id Auxiliar Structure */
	std::vector<int> idiomIds;
	std::vector<int> typeIds;
	std::vector<CSD *> idiomIdsDict;
	std::vector<CSD *> typeIdsDict;

public:
	/** General constructor **/
	CSD_LiteralSplit();

	/** Constructor receiving Tdict as a sequence of 'tlength' uchars. Tdict
	 */
	CSD_LiteralSplit(csd::CSD *Idioms, csd::CSD *Types, csd::CSD *Arbitrary,
			hdt::IteratorUCharString *it, hdt::ProgressListener *listener);

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

	/** Stores a CSD_LiteralSplit structure given a file pointer.
	 * @fp: pointer to the file saving a CSD_LiteralSplit structure.
	 * */
	void save(ostream & fp);

	size_t load(unsigned char *ptr, unsigned char *ptrMax);

	/** Loads a CSD_LiteralSplit structure from a file pointer.
	 * @fp: pointer to the file storing a CSD_LiteralSplit structure. */
	static CSD * load(istream & fp);

	void fillSuggestions(const char *base, vector<string> &out, int maxResults);

	hdt::IteratorUCharString *listAll() {
		throw "Not implemented";
	}

	/** General destructor. */
	~CSD_LiteralSplit();

	class VectCharIterator: public hdt::IteratorUCharString {
	private:
		std::vector<char *> &vector;
		unsigned int pos;
	public:
		VectCharIterator(std::vector<char *> &vector) :
				vector(vector), pos(0) {

		}
		virtual ~VectCharIterator() {
		}

		virtual bool hasNext() {
			return pos < vector.size();
		}

		virtual unsigned char *next() {
			return (unsigned char*) vector[pos++];
		}

		virtual unsigned int getNumberOfElements() {
			return vector.size();
		}
	};
protected:
	uint32_t limitArbitraryID; //lastID of the first CSD container
	uint32_t limitIdiomsID; //lastID of the second CSD container

	/*
	 *  Returns the position of the element, starting from the given node,
	 *   returns -1 if "don't exists"
	 */
	int find_unique(vector<char *> vocabularies, char * value) {
		bool exists = false;
		int l = 0, r = vocabularies.size() - 1, c = (l + r) / 2;

		while (l <= r) {
			if (strcmp(vocabularies[c], value) < 0)
				l = c + 1;
			else if (strcmp(vocabularies[c], value) == 0) {
				exists = true;
				break;
			} else
				r = c - 1;
			c = (l + r) / 2;
		}
		if (exists)
			return (l);
		else
			return -1;
	}

	/*
	 *  Returns the position equal or bigger to the given node
	 */
	int up_bound(vector<int> vocabularies, int value) {
		int l = 0, r = vocabularies.size() - 1, c = (l + r) / 2;
		bool exists = false;
		while (l <= r) {
			if (vocabularies[c] < value)
				l = c + 1;
			else if (vocabularies[c] == value) {
				exists = true;
				break;
			} else
				r = c - 1;
			c = (l + r) / 2;
		}
		if (exists)
			return (l);
		else
			return min((l), (int) vocabularies.size()-1);

	}
};
}
;
#endif  /* _CSDLITERALSPLIT_H */
