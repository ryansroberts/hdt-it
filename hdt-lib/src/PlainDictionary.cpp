/*
 * PlainDictionary.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include "PlainDictionary.hpp"
#include "Histogram.h"

namespace hdt {

/* DICTIONARY ENTRY */

string DictionaryEntry::toString() {
	string tmp;
	tmp.append(*prefix);
	tmp.append(*str);
	return tmp;
}

bool DictionaryEntry::cmpLexicographic(DictionaryEntry *c1, DictionaryEntry *c2) {
	return c1->toString().compare(c2->toString())<0;
}

bool DictionaryEntry::cmpID(DictionaryEntry *c1, DictionaryEntry *c2) {
	return c1->id>c2->id;
}


PlainDictionary::PlainDictionary() {
	this->mapping = MAPPING1;
}

PlainDictionary::PlainDictionary(HDTSpecification &spec) {
	this->spec = spec;

	if(spec.get("dictionary.mapping")=="mapping2") {
		this->mapping = MAPPING2;
	} else {
		this->mapping = MAPPING1;
	}
}

PlainDictionary::~PlainDictionary() {

}

std::string PlainDictionary::idToString(unsigned int id, TriplePosition position)
{
	vector<DictionaryEntry*> &vector = getDictionaryEntryVector(id, position);

	unsigned int localid = getLocalId(id, position);

	if(localid<vector.size()) {
		DictionaryEntry *entry = vector[localid];
		return entry->toString();
	}

	return string();
}

unsigned int PlainDictionary::stringToId(std::string &key, TriplePosition position)
{
	unsigned int id = 0;
	DictEntryIt ret;

	switch (position) {
	case SUBJECT:
		ret = hashSubject.find(key.c_str());
		if(ret!=hashSubject.end()) {
			id = ret->second->id;
		}
		break;
	case PREDICATE:
		ret = hashPredicate.find(key.c_str());
		if (ret != hashPredicate.end())
			id = ret->second->id;
		break;
	case OBJECT:
		ret = hashObject.find(key.c_str());
		if (ret != hashObject.end())
			id = ret->second->id;
		break;
	}

	return id;
}

TripleString PlainDictionary::tripleIDtoTripleString(TripleID &tripleID)
{
	std::string s = idToString(tripleID.getSubject(), SUBJECT);
	std::string p = idToString(tripleID.getPredicate(), PREDICATE);
	std::string o = idToString(tripleID.getObject(), OBJECT);

	TripleString ts(s,p,o);
	return ts;
}

TripleID PlainDictionary::tripleStringtoTripleID(TripleString &tripleString)
{

	std::string ss = tripleString.getSubject();
	std::string pp = tripleString.getPredicate();
	std::string oo = tripleString.getObject();
	unsigned int s = stringToId(ss, SUBJECT);
	unsigned int p = stringToId(pp, PREDICATE);
	unsigned int o = stringToId(oo, OBJECT);

	TripleID tid(s,p,o);
	return tid;
}

void PlainDictionary::startProcessing()
{
	subjects_not_shared.clear();
	subjects_shared.clear();
	objects_not_shared.clear();
	predicates.clear();

	hashSubject.clear(); //hashSubject.resize(nsubjects);
	hashObject.clear(); //hashObject.resize(nobjects);
	hashPredicate.clear(); //hashPredicate.resize(npredicates);
	prefixes.clear();
}

void PlainDictionary::stopProcessing()
{
	std::cout << "End processing: " << numberOfElements() << std::endl;
	this->split();
	std::cout << "Splitted" << std::endl;
	this->lexicographicSort();
	std::cout << "Sorted" << std::endl;
}

void PlainDictionary::load(std::istream & input)
{
	std::string line;
	unsigned char region = 1;

	startProcessing();

	while(getline(input, line)) {
		std::cout << line << std::endl;

		if(line!="") {
			if (region == 1) { //shared SO
				addEntry(line, SHARED_SUBJECT);
			} else if (region == 2) { //not shared Subjects
				addEntry(line, NOT_SHARED_SUBJECT);
			} else if (region == 3) { //not shared Objects
				addEntry(line, NOT_SHARED_OBJECT);
			} else if (region == 4) { //predicates
				addEntry(line, NOT_SHARED_PREDICATE);
			}
		}
	}
}

unsigned int PlainDictionary::numberOfElements()
{
	return subjects_shared.size() + subjects_not_shared.size() + objects_not_shared.size() + predicates.size();
}

unsigned int PlainDictionary::insert(std::string & str, TriplePosition position)
{
	addEntry(str, position);
}

bool PlainDictionary::save(std::ostream &output)
{
	unsigned int i = 0;
	const char marker = '\n';

	//shared subjects-objects from subjects
	for (i = 0; i < subjects_shared.size(); i++) {
		output << *subjects_shared[i]->prefix;
		output << *subjects_shared[i]->str;
		output.put(marker); //character to split file
	}

	output.put(marker); //extra line to set the beggining of next part of dictionary

	//not shared subjects
	for (i = 0; i < subjects_not_shared.size(); i++) {
		output << *subjects_not_shared[i]->prefix;
		output << *subjects_not_shared[i]->str;
		output.put(marker); //character to split file
	}

	output.put(marker); //extra line to set the beggining of next part of dictionary

	//not shared objects
	for (i = 0; i < objects_not_shared.size(); i++) {
		output << *objects_not_shared[i]->prefix;
		output << *objects_not_shared[i]->str;
		output.put(marker); //character to split file
	}

	output.put(marker); //extra line to set the beggining of next part of dictionary

	//predicates
	for (i = 0; i < predicates.size(); i++) {
		output << *predicates[i]->prefix;
		output << *predicates[i]->str;
		output.put(marker); //character  to split file
	}
}




// PRIVATE

void PlainDictionary::addEntry(string str, DictionarySection pos) {

	if(str=="") return;

	DictionaryEntry *entry = new DictionaryEntry;
	setPrefixAndString(entry, str);

	switch(pos) {
	case SHARED_SUBJECT:
		subjects_shared.push_back(entry);
		hashSubject[str.c_str()] = entry;
		hashObject[str.c_str()] = entry;
		break;
	case NOT_SHARED_SUBJECT:
		subjects_not_shared.push_back(entry);
		hashSubject[str.c_str()] = entry;
		break;
	case NOT_SHARED_OBJECT:
		objects_not_shared.push_back(entry);
		hashObject[str.c_str()] = entry;
		break;
	case NOT_SHARED_PREDICATE:
		predicates.push_back(entry);
		hashPredicate[str.c_str()] = entry;
	}
}

void PlainDictionary::addEntry(string str, TriplePosition pos) {

	if(str=="") return;

	if(pos==PREDICATE) {
		DictEntryIt it = hashPredicate.find(str.c_str());
		if(it==hashPredicate.end()) {
			DictionaryEntry *entry = new DictionaryEntry;
			setPrefixAndString(entry, str);
			hashPredicate[str.c_str()] = entry;
			predicates.push_back(entry);
		}

		return;
	}

	DictEntryIt subjectIt = hashSubject.find(str.c_str());
	DictEntryIt objectIt = hashObject.find(str.c_str());

	if(pos==SUBJECT) {
		if(subjectIt==hashSubject.end() && objectIt==hashSubject.end()) {
			// Did not exist, create new.
			DictionaryEntry *entry = new DictionaryEntry;
			setPrefixAndString(entry, str);

			hashSubject[str.c_str()] = entry;
			cout << " Add new subject: " << str << endl;
		}

		if(objectIt!=hashObject.end() && subjectIt==hashSubject.end()) {
			// Already exists in objects.
			hashSubject[str.c_str()] = objectIt->second;
			cout << "   existing subject: " << str << endl;
		}
	} else if(pos==OBJECT) {
		if(subjectIt==hashSubject.end() && objectIt==hashSubject.end()) {
			// Did not exist, create new.
			DictionaryEntry *entry = new DictionaryEntry;
			setPrefixAndString(entry, str);

			hashObject[str.c_str()] = entry;
			cout << " Add new object: " << str << endl;
		}

		if(subjectIt!=hashSubject.end() && objectIt==hashObject.end()) {
			// Already exists in subjects.
			hashObject[str.c_str()] = subjectIt->second;
			cout << "     existing object: " << str << endl;
		}
	}

}

/** Split
 * @return void
 */
void PlainDictionary::split() {
	subjects_not_shared.clear();
	subjects_shared.clear();
	objects_not_shared.clear();

	for(DictEntryHash::const_iterator subj_it = hashSubject.begin(); subj_it!=hashSubject.end(); subj_it++) {
		cout << "Check Subj: " << subj_it->first << endl;
		DictEntryIt other = hashObject.find(subj_it->first);

		if(other==hashObject.end()) {
			// Only subject
			subjects_not_shared.push_back(subj_it->second);
		} else {
			// Exist in both
			subjects_shared.push_back(subj_it->second);
		}
	}
	cout << "Hash Size Object: " << hashObject.size() << endl;

	for(DictEntryHash::iterator obj_it = hashObject.begin(); obj_it!=hashObject.end(); ++obj_it) {
		cout << "Check Obj: " << obj_it->first << endl;
		/*DictEntryIt other = hashSubject.find(obj_it->first);

		if(other==hashSubject.end()) {
			// Only object
			objects_not_shared.push_back(obj_it->second);
		}*/
	}
}

/** Lexicographic Sort
 * @param mapping Description of the param.
 * @return void
 */
void PlainDictionary::lexicographicSort() {

	//sort shared and not shared subjects
	sort(subjects_shared.begin(), subjects_shared.end(), DictionaryEntry::cmpLexicographic);
	sort(subjects_not_shared.begin(), subjects_not_shared.end(), DictionaryEntry::cmpLexicographic);

	//sort not shared objects
	sort(objects_not_shared.begin(), objects_not_shared.end(), DictionaryEntry::cmpLexicographic);

	//sort predicates
	sort(predicates.begin(), predicates.end(), DictionaryEntry::cmpLexicographic);

	updateIDs();
}

void PlainDictionary::idSort() {
	//sort shared and not shared subjects
	sort(subjects_shared.begin(), subjects_shared.end(), DictionaryEntry::cmpID);
	sort(subjects_not_shared.begin(), subjects_not_shared.end(), DictionaryEntry::cmpID);

	//sort not shared objects
	sort(objects_not_shared.begin(), objects_not_shared.end(), DictionaryEntry::cmpID);

	//sort predicates
	sort(predicates.begin(), predicates.end(), DictionaryEntry::cmpID);

	updateIDs();
}


/** Map
 * @param mapping Description of the param.
 * @return void
 */
void PlainDictionary::updateIDs() {
	int i;

	for (i = 0; i < subjects_shared.size(); i++) {
		subjects_shared[i]->id = getGlobalId(i, SHARED_SUBJECT);
	}

	for (i = 0; i < subjects_not_shared.size(); i++) {
		subjects_not_shared[i]->id = getGlobalId(i, NOT_SHARED_SUBJECT);
	}

	for (i = 0; i < objects_not_shared.size(); i++) {
		objects_not_shared[i]->id = getGlobalId(i, NOT_SHARED_OBJECT);
	}

	for (i = 0; i < predicates.size(); i++) {
		predicates[i]->id = getGlobalId(i, NOT_SHARED_PREDICATE);
	}
}



// OLD

















void PlainDictionary::convertMapping(unsigned int mapping) {
	switch(mapping) {
	case MAPPING1:
		this->mapping = mapping;
		updateIDs();
		break;
	case MAPPING2:
		this->mapping = mapping;
		updateIDs();
		break;
	default:
		break;
	}
}

void PlainDictionary::setPrefixAndString(DictionaryEntry *entry, string &str) {
	static string empty("");

	size_t pos = str.find_last_of('/');
	if(pos!=string::npos) {
		string prefix = str.substr(0, pos);

		PrefixIt prefixIt = prefixes.find(prefix.c_str());

		string *foundPrefix;

		if(prefixIt==prefixes.end()) {
			foundPrefix = new string(prefix);
			prefixes[prefix.c_str()] = foundPrefix;
		} else {
			foundPrefix = prefixIt->second;
		}

		entry->prefix = foundPrefix;
		entry->str = new string(str.substr(pos));
	} else {
		entry->prefix = &empty;
		entry->str = new string(str);
	}
}

vector<DictionaryEntry*> &PlainDictionary::getDictionaryEntryVector(unsigned int id, TriplePosition position) {
	switch (position) {
	case SUBJECT:
		if(id<= subjects_shared.size()) {
			return subjects_shared;
		} else {
			return subjects_not_shared;
		}

	case PREDICATE:
		return predicates;

	case OBJECT:
		if(id<= subjects_shared.size()) {
			return subjects_shared;
		} else {
			return objects_not_shared;
		}
	}

	throw "Item not found";
}

unsigned int PlainDictionary::getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position) {
	switch (position) {
		case NOT_SHARED_SUBJECT:
			return subjects_shared.size()+id+1;

		case NOT_SHARED_PREDICATE:
			return id+1;

		case NOT_SHARED_OBJECT:
			if(mapping==MAPPING2) {
				return subjects_shared.size()+id+1;
			} else {
				return subjects_shared.size()+subjects_not_shared.size()+id+1;
			}

		case SHARED_SUBJECT:
		case SHARED_OBJECT:
			return id+1;
		}

		throw "Item not found";
}


unsigned int PlainDictionary::getGlobalId(unsigned int id, DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

unsigned int PlainDictionary::getLocalId(unsigned int mapping, unsigned int id, TriplePosition position) {
	switch (position) {
		case SUBJECT:
			if(id<=subjects_shared.size()) {
				return id-1;
			} else {
				return id-subjects_shared.size()-1;
			}
			break;
		case OBJECT:
			if(id<=subjects_shared.size()) {
				return id-1;
			} else {
				if(mapping==MAPPING2) {
					return id-subjects_shared.size()-1;
				} else {
					return id-subjects_shared.size()-subjects_not_shared.size()-1;
				}
			}
			break;
		case PREDICATE:
			return id-1;
		}

		throw "Item not found";
}

unsigned int PlainDictionary::getLocalId(unsigned int id, TriplePosition position) {
	return getLocalId(mapping,id,position);
}



/** Get Max Id
 * @return The expected result
 */
unsigned int PlainDictionary::getMaxID() {
	unsigned int s = subjects_not_shared.size();
	unsigned int o = objects_not_shared.size();
	unsigned int shared = subjects_shared.size();
	unsigned int max = s>o ? s : o;

	if(mapping ==MAPPING2) {
		return shared+max;
	} else {
		return shared+s+o;
	}
}

unsigned int PlainDictionary::getMaxSubjectID() {
	unsigned int shared = subjects_shared.size();
	unsigned int s = subjects_not_shared.size();

	return shared+s;
}

unsigned int PlainDictionary::getMaxPredicateID() {
	return predicates.size();
}

unsigned int PlainDictionary::getMaxObjectID() {
	unsigned int shared = subjects_shared.size();
	unsigned int s = subjects_not_shared.size();
	unsigned int o = objects_not_shared.size();

	if(mapping ==MAPPING2) {
		return shared+o;
	} else {
		return shared+s+o;
	}
}

/** Get Mapping
 * @return The expected result
 */
unsigned int PlainDictionary::getMapping() {
	return mapping;
}

/** Get N Subjects
 * @return The expected result
 */
unsigned int PlainDictionary::getNsubjects() {
	return subjects_shared.size()+subjects_not_shared.size();
}

/** Get N Predicates
 * @return The expected result
 */
unsigned int PlainDictionary::getNpredicates() {
	return predicates.size();
}

/** Get N Objects
 * @return The expected result
 */
unsigned int PlainDictionary::getNobjects() {
	return subjects_shared.size()+objects_not_shared.size();
}

/** Get S Subobj
 * @return The expected result
 */
unsigned int PlainDictionary::getSsubobj() {
	return subjects_shared.size();
}



void PlainDictionary::updateID(unsigned int oldid, unsigned int newid, DictionarySection position) {
	switch (position) {
	case SHARED_SUBJECT:
	case SHARED_OBJECT:
			subjects_shared[oldid]->id = newid;
			break;
	case NOT_SHARED_SUBJECT:
		subjects_not_shared[oldid]->id = newid;
		break;
	case NOT_SHARED_PREDICATE:
		predicates[oldid]->id = newid;
		break;
	case NOT_SHARED_OBJECT:
		objects_not_shared[oldid]->id = newid;
		break;
	}
}

void PlainDictionary::dumpSizes(ostream &out) {
	cout << "\n\t [Dictionary stats:\n";
	cout << "\t   shared subjects-objects:" << getSsubobj() << "\n";
	cout << "\t   not shared subjects:" << getNsubjects() - getSsubobj() << "\n";
	cout << "\t   not shared objects:" << getNobjects() - getSsubobj() << "\n";
	cout << "\t total subjects:" << getNsubjects() << "\n";
	cout << "\t total objects:" << getNobjects() << "\n";
	cout << "\t total predicates:" << getNpredicates() << " ]\n\n";
}

/** Dump Stats
 * @param output Description of the param.
 * @return void
 */
void PlainDictionary::dumpStats(string &output) {
	unsigned int i = 0;

	const int maxval = 50000;
	const int nbins = 50000;

	Histogram histoURI(0, maxval, nbins);
	Histogram histoLiteral(0, maxval, nbins);
	Histogram histoBlank(0, maxval, nbins);

	string tmp;
	//shared subjects-objects from subjects
	for (i = 0; i < subjects_shared.size(); i++) {
		tmp.clear();

		tmp.append(*subjects_shared[i]->prefix);
		tmp.append(*subjects_shared[i]->str);

		if (tmp[0] == '<') {
			histoURI.Add(tmp.length());
		} else if (tmp[0] == '"') {
			histoLiteral.Add(tmp.length());
		} else if (tmp[0] == '_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}

	//not shared subjects
	for (i = 0; i < subjects_not_shared.size(); i++) {
		tmp.clear();
		tmp.append(*subjects_not_shared[i]->prefix);
		tmp.append(*subjects_not_shared[i]->str);

		if (tmp[0] == '<') {
			histoURI.Add(tmp.length());
		} else if (tmp[0] == '"') {
			histoLiteral.Add(tmp.length());
		} else if (tmp[0] == '_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}

	//not shared objects
	for (i = 0; i < objects_not_shared.size(); i++) {
		tmp.clear();
		tmp.append(*objects_not_shared[i]->prefix);
		tmp.append(*objects_not_shared[i]->str);

		if (tmp[0] == '<') {
			histoURI.Add(tmp.length());
		} else if (tmp[0] == '"') {
			histoLiteral.Add(tmp.length());
		} else if (tmp[0] == '_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}

	//predicates
	for (i = 0; i < predicates.size(); i++) {
		tmp.clear();
		tmp.append(*predicates[i]->prefix);
		tmp.append(*predicates[i]->str);

		if (tmp[0] == '<') {
			histoURI.Add(tmp.length());
		} else if (tmp[0] == '"') {
			histoLiteral.Add(tmp.length());
		} else if (tmp[0] == '_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}

	histoURI.end();
	histoURI.dump(output.c_str(), "URI");

	histoLiteral.end();
	histoLiteral.dump(output.c_str(), "Literal");

	histoBlank.end();
	histoBlank.dump(output.c_str(), "Blank");
}


}
