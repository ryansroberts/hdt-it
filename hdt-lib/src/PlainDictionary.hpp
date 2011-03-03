/*
 * PlainDictionary.h
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef PLAINDICTIONARY_H_
#define PLAINDICTIONARY_H_

#include <HDTSpecification.hpp>
#include <Dictionary.hpp>

#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ext/hash_map>

namespace std { using namespace __gnu_cxx; }

namespace hdt {

struct DictionaryEntry {
public:
	unsigned int id;
	const std::string *prefix;
	const std::string *str;

	std::string toString();
	bool static cmpLexicographic(DictionaryEntry *c1, DictionaryEntry *c2);
	bool static cmpID(DictionaryEntry *c1, DictionaryEntry *c2);
};

struct str_cmp {
	bool operator()(const char* s1, const char* s2) const {
	    return strcmp(s1,s2)==0;
	}
};

typedef std::pair<const char*, DictionaryEntry *> DictEntryPair;
typedef std::hash_map<const char *, DictionaryEntry *, std::hash<const char *>, str_cmp> DictEntryHash;
typedef DictEntryHash::iterator DictEntryIt;
typedef std::hash_map<const char *, std::string *, std::hash<const char*>, str_cmp> PrefixHash;
typedef PrefixHash::iterator PrefixIt;


class PlainDictionary : public Dictionary {
// Private attributes
private:
	std::vector<DictionaryEntry*> predicates;
	std::vector<DictionaryEntry*> subjects_shared;
	std::vector<DictionaryEntry*> subjects_not_shared;
	std::vector<DictionaryEntry*> objects_not_shared;
	DictEntryHash hashSubject;
	DictEntryHash hashPredicate;
	DictEntryHash hashObject;
	PrefixHash prefixes;
	unsigned int mapping;

	HDTSpecification spec;

// Public Interface
public:
	PlainDictionary();
	PlainDictionary(HDTSpecification &spec);
	~PlainDictionary();

	std::string idToString(unsigned int id, TriplePosition position);
	unsigned int stringToId(std::string &str, TriplePosition position);

	TripleID tripleStringtoTripleID(TripleString &tripleString);
	TripleString tripleIDtoTripleString(TripleID &tripleID);

	bool save(std::ostream &output);
	void load(std::istream &input);
	unsigned int insert(std::string &str, TriplePosition position);
	unsigned int numberOfElements();
	void startProcessing();
	void stopProcessing();

// Private methods
private:
	void addEntry(std::string entry, DictionarySection pos);
	void addEntry(std::string entry, TriplePosition pos);

	void split();
	void lexicographicSort();
	void idSort();
	void updateIDs();

	void setPrefixAndString(DictionaryEntry *entry, std::string &str);


	std::vector<DictionaryEntry*> &getDictionaryEntryVector(unsigned int id, TriplePosition position);

public:
	unsigned int getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position);
	unsigned int getGlobalId(unsigned int id, DictionarySection position);
	unsigned int getLocalId(unsigned int mapping, unsigned int id, TriplePosition position);
	unsigned int getLocalId(unsigned int id, TriplePosition position);

	void convertMapping(unsigned int mapping);
	void updateID(unsigned int oldid, unsigned int newid, DictionarySection position);


	unsigned int getMaxID();
	unsigned int getMaxSubjectID();
	unsigned int getMaxPredicateID();
	unsigned int getMaxObjectID();
	unsigned int getMapping();
	unsigned int getNsubjects();
	unsigned int getNpredicates();
	unsigned int getNobjects();
	unsigned int getSsubobj();

	void dumpStats(std::string &path);
	void dumpSizes(std::ostream &out);
};

}

#endif /* PLAINDICTIONARY_H_ */
