/*
 * File: PFCPlusDictionary.cpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Mario Arias
 * All rights reserved.
 *
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
 *   Mario Arias:               mario.arias@gmail.com
 *
 */

#include "PFCPlusDictionary.hpp"
#include <HDTVocabulary.hpp>
#include "../libdcs/CSD_PFC.h"
#include "../libdcs/CSD_HTFC.h"

namespace hdt {

class DictIterator: public csd::IteratorUCharString {
private:
	std::vector<DictionaryEntry *> &vector;
	unsigned int pos;
public:
	DictIterator(std::vector<DictionaryEntry *> &vector) :
			vector(vector), pos(0) {

	}

	virtual bool hasNext() {
		return pos < vector.size();
	}

	virtual unsigned char *next() {
		return (unsigned char*) vector[pos++]->str->c_str();
	}

	virtual unsigned char *prev() {
			return (unsigned char*) vector[--pos]->str->c_str();
		}

	virtual unsigned char *nextTest() {
			return (unsigned char*) vector[pos]->str->c_str();
		}

	virtual unsigned int getNumberOfElements() {
		return vector.size();
	}
};

PFCPlusDictionary::PFCPlusDictionary() :
		blocksize(8) {
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objectsNotLiterals = new csd::CSD_PFC();
	objectsLiterals = new csd::CSD_FMIndex();
	shared = new csd::CSD_PFC();

	stopwordLiterals = (uchar*) malloc(1 * sizeof(uchar));
	stopwordLiterals[0] = '"'; //dgdfsgadfasdfs
}

PFCPlusDictionary::PFCPlusDictionary(HDTSpecification & spec) :
		blocksize(32) {
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objectsNotLiterals = new csd::CSD_PFC();
	objectsLiterals = new csd::CSD_FMIndex();
	shared = new csd::CSD_PFC();

	string blockSizeStr = spec.get("dict.block.size");
	if (blockSizeStr != "") {
		blocksize = atoi(blockSizeStr.c_str());
	}

	stopwordLiterals = (uchar*) malloc(1 * sizeof(uchar));
	stopwordLiterals[0] = '<'; //dgdfsgadfasdfs
}

PFCPlusDictionary::~PFCPlusDictionary() {
	delete subjects;
	delete predicates;
	delete objectsNotLiterals;
	delete objectsLiterals;
	delete shared;
}

csd::CSD *loadSectionPFC(DictIterator *iterator, uint32_t blocksize,
		ProgressListener *listener) {
	return new csd::CSD_PFC(iterator, blocksize, listener);
	//return new csd::CSD_HTFC(iterator, blocksize, listener);
}

/*
 * use_sample = true ; enables to locate substrings.
 */
csd::CSD *loadSectionFMIndex(DictIterator *iterator, uchar *stopword,
		bool sparse_bitsequence = false, int bparam = 40,
		size_t bwt_sample = 64, bool use_sample = true,
		hdt::ProgressListener *listener = NULL) {
	return new csd::CSD_FMIndex(iterator, stopword, sparse_bitsequence, bparam,
			bwt_sample, use_sample, listener);
}

void PFCPlusDictionary::import(PlainDictionary *dictionary,
		ProgressListener *listener) {
	DictIterator itSubj(dictionary->subjects_not_shared);
	DictIterator itPred(dictionary->predicates);
	DictIterator itObj(dictionary->objects_not_shared);
	DictIterator itShared(dictionary->subjects_shared);

	IntermediateListener iListener(listener);
	try {

		//NOTIFY(listener, "DictionaryPFC loading subjects", 0, 100);
		delete subjects;
		iListener.setRange(0, 20);
		subjects = loadSectionPFC(&itSubj, blocksize, &iListener);

		//NOTIFY(listener, "DictionaryPFC loading predicates", 25, 30);
		delete predicates;
		iListener.setRange(20, 21);
		predicates = loadSectionPFC(&itPred, blocksize, &iListener);

//		cout<<"testing: Next literal:"<<itObj.nextTest()<<endl;

		//NOTIFY(listener, "DictionaryPFC loading objects", 30, 90);
		delete objectsLiterals;
		iListener.setRange(21, 50);

		objectsLiterals = loadSectionFMIndex(&itObj,stopwordLiterals, &iListener);

		itObj.prev();
//		cout<<"testing: Next notliteral:"<<itObj.nextTest()<<endl;

		iListener.setRange(50, 90);
		delete objectsNotLiterals;
		objectsNotLiterals = loadSectionPFC(&itObj, blocksize, &iListener);

		//NOTIFY(listener, "DictionaryPFC loading shared", 90, 100);
		delete shared;
		iListener.setRange(90, 100);
		shared = loadSectionPFC(&itShared, blocksize, &iListener);

		this->sizeStrings = dictionary->size();
		this->mapping = dictionary->getMapping();
	} catch (const char *e) {
		delete subjects;
		delete predicates;
		delete objectsLiterals;
		delete objectsNotLiterals;
		delete shared;
		subjects = new csd::CSD_PFC();
		predicates = new csd::CSD_PFC();
		objectsNotLiterals = new csd::CSD_PFC();
		objectsLiterals = new csd::CSD_FMIndex();
		shared = new csd::CSD_PFC();
		throw e;
	}

#if 0
	cout << "Shared: " << shared->getLength() << endl;
	cout << "Subjects: " << subjects->getLength() << endl;
	cout << "Predicates: " << predicates->getLength() << endl;
	cout << "Objects Not Literals: " << objectsNotLiterals->getLength() << endl;
	cout << "Objects Literals: " << objectsLiterals->getLength() << endl;
	/*
	 cout << "Ensure same: " << endl;
	 for(unsigned int i=1;i<getMaxObjectID();i++){
	 string str1 = dictionary->idToString(i, OBJECT);
	 string str2 = this->idToString(i, OBJECT);
	 unsigned int id1 = dictionary->stringToId(str1, OBJECT);
	 unsigned int id2 = this->stringToId(str1, OBJECT);

	 if( (str1!=str2) || (id1!=id2)) {
	 cout << i << " Objects difer: " << endl;
	 cout << "\tPlain: " << id1 << " => " << str1 << endl;
	 cout << "\tPFC__: " << id2 << " => " << str2 << endl;
	 }
	 }
	 */
#endif
}

std::string PFCPlusDictionary::idToString(unsigned int id,
		TripleComponentRole position) {
	csd::CSD *section = getDictionarySection(id, position);
	unsigned int localid = getLocalId(id, position);

	if (localid <= section->getLength()) {
		const char * ptr = (const char *) section->extract(localid);
		if (ptr != NULL) {
			string out = ptr;
			delete[] ptr;
			return out;
		} else {
			//cout << "Not found: " << id << " as " << position << endl;
		}
	}

	return string();
}

unsigned int PFCPlusDictionary::stringToId(std::string &key,
		TripleComponentRole position) {
	unsigned int ret;

	if (key.length() == 0 || key.at(0) == '?') {
		return 0;
	}

	switch (position) {
	case SUBJECT:
		ret = shared->locate((const uchar *) key.c_str(), key.length());
		if (ret != 0) {
			return getGlobalId(ret, SHARED_SUBJECT);
		}
		ret = subjects->locate((const uchar *) key.c_str(), key.length());
		if (ret != 0) {
			return getGlobalId(ret, NOT_SHARED_SUBJECT);
		}
		throw "Subject not found in dictionary";
	case PREDICATE:
		ret = predicates->locate((const uchar *) key.c_str(), key.length());
		if (ret != 0) {
			return getGlobalId(ret, NOT_SHARED_PREDICATE);
		}
		throw "Predicate not found in dictionary";

	case OBJECT:
		if (key.at(0) == '"') {
			ret = objectsLiterals->locate((const uchar *) key.c_str(),
					key.length());
			if (ret != 0) {
				return getGlobalId(ret, NOT_SHARED_OBJECT);
			}
			else{
				throw "Object not found in dictionary";
			}
		} else {
			ret = shared->locate((const uchar *) key.c_str(), key.length());
			if (ret != 0) {
				return getGlobalId(ret, SHARED_OBJECT);
			}
			ret = objectsNotLiterals->locate((const uchar *) key.c_str(), key.length());
			if (ret != 0) {
				return getGlobalId(ret, NOT_SHARED_OBJECT)+	objectsLiterals->getLength();
			}
			throw "Object not found in dictionary";
		}
	}
}

void PFCPlusDictionary::load(std::istream & input, ControlInformation & ci,
		ProgressListener *listener) {
	this->mapping = ci.getUint("$mapping");
	this->sizeStrings = ci.getUint("$sizeStrings");

	ifstream *in = dynamic_cast<ifstream *>(&input);

	IntermediateListener iListener(listener);

	iListener.setRange(0, 25);
	iListener.notifyProgress(0, "Dictionary read shared area.");
	//cout << "Load shared " << in->tellg() << endl;
	delete shared;
	shared = csd::CSD::load(*in);
	if (shared == NULL) {
		shared = new csd::CSD_PFC();
		throw "Could not read shared sectionsss.";
	}

	iListener.setRange(25, 50);
	iListener.notifyProgress(0, "Dictionary read subjects.");
	//cout << "Load subjects " << in->tellg() << endl;
	delete subjects;
	subjects = csd::CSD::load(*in);
	if (subjects == NULL) {
		subjects = new csd::CSD_PFC();
		throw "Could not read subjects.";
	}

	iListener.setRange(50, 75);
	iListener.notifyProgress(0, "Dictionary read predicates.");
	//cout << "Load predicates " << in->tellg() << endl;
	delete predicates;
	predicates = csd::CSD::load(*in);
	if (predicates == NULL) {
		predicates = new csd::CSD_PFC();
		throw "Could not read predicates.";
	}

	iListener.setRange(75, 100);
	iListener.notifyProgress(0, "Dictionary read objects.");
//	cout << "Load objects Literals " << in->tellg() << endl;

	delete objectsLiterals;
	objectsLiterals = csd::CSD::load(*in);
	if (objectsLiterals == NULL) {
		objectsLiterals = new csd::CSD_FMIndex();
		throw "Could not read objects Literals.";
	}

	//uncomment to test locate substring
/*
	uint32_t *occs = 0;
	uchar *subs = (unsigned char*) "alB";
	//uint32_t numresults = ((csd::CSD_FMIndex *) objectsLiterals)->locate_substring(subs,10,&occs);
	uint32_t numresults = this->substringToId(subs,3,&occs);
	cout<<endl<<endl<<"testing: numresults:"<<numresults<<endl<<endl;
	for (int i=0;i<numresults;i++){
		cout<<endl<<endl<<"testing: result id:"<<occs[i]<<"; string:"<<this->idToString(occs[i]+shared->getLength(),OBJECT)<<endl<<endl;
	}
*/

	delete objectsNotLiterals;
	objectsNotLiterals = csd::CSD::load(*in);
	if (objectsNotLiterals == NULL) {
		objectsNotLiterals = new csd::CSD_PFC();
		throw "Could not read objects not Literals.";
	}
	//cout << "Dictionary loaded " << in->tellg() << endl;
}

/** Returns the number of IDs that contain s[1,..len] as a substring. It also
 * return in occs the IDs. Otherwise return 0.
 *  @s: the substring to be located.
 *  @len: the length (in characters) of the string s.
 *  @occs: pointer where the ID located will be stored.
 * */
uint32_t PFCPlusDictionary::substringToId(uchar *s, uint32_t len, uint32_t **occs){

	return ((csd::CSD_FMIndex *) objectsLiterals)->locate_substring(s,len,occs);

}

void PFCPlusDictionary::save(std::ostream & output,
		ControlInformation & controlInformation, ProgressListener *listener) {
	controlInformation.set("codification", HDTVocabulary::DICTIONARY_TYPE_PFCPLUS);
	controlInformation.set("format", "text/plain");
	controlInformation.setUint("$elements", getNumberOfElements());

	controlInformation.setUint("$subjects", getNsubjects());
	controlInformation.setUint("$objects", getNobjects());
	controlInformation.setUint("$predicates", getNpredicates());
	controlInformation.setUint("$sharedso", getSsubobj());

	controlInformation.setUint("$maxid", getMaxID());
	controlInformation.setUint("$maxsubjectid", getMaxSubjectID());
	controlInformation.setUint("$maxpredicateid", getMaxPredicateID());
	controlInformation.setUint("$maxobjectid", getMaxObjectID());

	controlInformation.setUint("$mapping", this->mapping);
	controlInformation.setUint("$sizeStrings", this->sizeStrings);
	controlInformation.setUint("$blockSize", this->blocksize);

	controlInformation.save(output);

	ofstream *out = dynamic_cast<ofstream *>(&output);
	IntermediateListener iListener(listener);

	iListener.setRange(0, 10);
	iListener.notifyProgress(0, "Dictionary save shared area.");
	//cout << "Save shared " << out->tellp() << endl;
	shared->save(*out);

	iListener.setRange(10, 45);
	iListener.notifyProgress(0, "Dictionary save subjects.");
	//cout << "Save subjects " << out->tellp() << endl;
	subjects->save(*out);

	iListener.setRange(45, 60);
	iListener.notifyProgress(0, "Dictionary save predicates.");
	//cout << "Save predicates " << out->tellp() << endl;
	predicates->save(*out);

	iListener.setRange(60, 80);
	iListener.notifyProgress(0, "Dictionary save literal objects.");
	//cout << "Save objects " << out->tellp() << endl;
	objectsLiterals->save(*out);

	iListener.setRange(80, 100);
		iListener.notifyProgress(0, "Dictionary save non literal objects.");
		//cout << "Save objects " << out->tellp() << endl;
		objectsNotLiterals->save(*out);

	//cout << "Dictionary saved " << out->tellp() << endl;
}

void PFCPlusDictionary::populateHeader(Header & header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::DICTIONARY_TYPE,
			HDTVocabulary::DICTIONARY_TYPE_PFCPLUS);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSUBJECTS,
			getNsubjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMPREDICATES,
			getNpredicates());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMOBJECTS,
			getNobjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSHARED, getSsubobj());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXSUBJECTID,
			getMaxSubjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXPREDICATEID,
			getMaxPredicateID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXOBJECTTID,
			getMaxObjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAPPING, this->mapping);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_SIZE_STRINGS, size());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_BLOCK_SIZE,
			this->blocksize);
}

unsigned int PFCPlusDictionary::getNsubjects() {
	return shared->getLength() + subjects->getLength();
}
unsigned int PFCPlusDictionary::getNpredicates() {
	return predicates->getLength();
}
unsigned int PFCPlusDictionary::getNobjects() {
	return shared->getLength() + objectsNotLiterals->getLength() + objectsLiterals->getLength();
}
unsigned int PFCPlusDictionary::getSsubobj() {
	return shared->getLength();
}

unsigned int PFCPlusDictionary::getMaxID() {
	unsigned int s = subjects->getLength();
	unsigned int o = objectsLiterals->getLength()+objectsNotLiterals->getLength();
	unsigned int sh = shared->getLength();
	unsigned int max = s > o ? s : o;

	if (mapping == MAPPING2) {
		return sh + max;
	} else {
		return sh + s + o;
	}
}

unsigned int PFCPlusDictionary::getMaxSubjectID() {
	return getNsubjects();
}

unsigned int PFCPlusDictionary::getMaxPredicateID() {
	return predicates->getLength();
}

unsigned int PFCPlusDictionary::getMaxObjectID() {
	unsigned int s = subjects->getLength();
	unsigned int o = objectsLiterals->getLength()+objectsNotLiterals->getLength();
	unsigned int sh = shared->getLength();

	if (mapping == MAPPING2) {
		return sh + o;
	} else {
		return sh + s + o;
	}
}

unsigned int PFCPlusDictionary::getNumberOfElements() {
	return shared->getLength() + subjects->getLength() + predicates->getLength()
			+ objectsLiterals->getLength()+objectsNotLiterals->getLength();
}

unsigned int PFCPlusDictionary::size() {
	return shared->getSize() + subjects->getSize() + predicates->getSize()
			+ objectsLiterals->getSize()+objectsNotLiterals->getSize();
}

void PFCPlusDictionary::startProcessing(ProgressListener *listener) {
}

void PFCPlusDictionary::stopProcessing(ProgressListener *listener) {
	cout << "************ SHARED ***********" << endl;
	shared->dumpAll();

	cout << "************ SUBJECTS ***********" << endl;
	subjects->dumpAll();

	cout << "************ PREDS ***********" << endl;
	predicates->dumpAll();

	cout << "************ OBJS Literals ***********" << endl;
	objectsLiterals->dumpAll();

	cout << "************ OBJS NOT Literals ***********" << endl;
		objectsNotLiterals->dumpAll();
}

unsigned int PFCPlusDictionary::insert(std::string & str,
		TripleComponentRole position) {
	throw "This dictionary does not support insertions.";
}

string PFCPlusDictionary::getType() {
	return HDTVocabulary::DICTIONARY_TYPE_PFCPLUS;
}

csd::CSD *PFCPlusDictionary::getDictionarySection(unsigned int id,
		TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if (id <= shared->getLength()) {
			//cout << "Section SHARED" << endl;
			return shared;
		} else {
			//cout << "Section SUBJECTS" << endl;
			return subjects;
		}

	case PREDICATE:
		return predicates;

	case OBJECT:

		if (id <= shared->getLength()) {
			//cout << "Section SHARED" << endl;
			return shared;
		} else {
			unsigned int localId = 0;
			if (mapping == MAPPING2) {
				localId = id - shared->getLength();
			} else {
				localId = 2+ id - shared->getLength() - subjects->getLength();
			}

			if (localId <= objectsLiterals->getLength()) {
				return objectsLiterals;
			} else {
				return objectsNotLiterals;
			}
		}
	}

	throw "Item not found";
}

unsigned int PFCPlusDictionary::getGlobalId(unsigned int mapping,
		unsigned int id, DictionarySection position) {
	switch (position) {
	case NOT_SHARED_SUBJECT:
		return shared->getLength() + id;

	case NOT_SHARED_PREDICATE:
		return id;

	case NOT_SHARED_OBJECT:
		if (mapping == MAPPING2) {
			return shared->getLength() + id;
		} else {
			return shared->getLength() + subjects->getLength() + id;
		}

	case SHARED_SUBJECT:
	case SHARED_OBJECT:
		return id;
	}

	throw "Item not found";
}

unsigned int PFCPlusDictionary::getGlobalId(unsigned int id,
		DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

unsigned int PFCPlusDictionary::getLocalId(unsigned int mapping,
		unsigned int id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if (id <= shared->getLength()) {
			return id;
		} else {
			return id - shared->getLength();
		}
		break;
	case OBJECT:
		if (id <= shared->getLength()) {
			return id;
		} else {
			unsigned int localId = 0;
			if (mapping == MAPPING2) {
				localId = id - shared->getLength();
			} else {
				localId =  2 + id - shared->getLength() - subjects->getLength();
			}

			if (localId <= objectsLiterals->getLength()) {
							return localId;
			} else {
				return localId-objectsLiterals->getLength();
			}


		}
		break;
	case PREDICATE:
		return id;
	}

	throw "Item not found";
}

unsigned int PFCPlusDictionary::getLocalId(unsigned int id,
		TripleComponentRole position) {
	return getLocalId(mapping, id, position);
}

void PFCPlusDictionary::getSuggestions(const char *base,
		hdt::TripleComponentRole role, std::vector<std::string> &out,
		int maxResults) {
	if (role == PREDICATE) {
		predicates->fillSuggestions(base, out, maxResults);
		return;
	}

	vector<string> v1, v2;
	shared->fillSuggestions(base, v1, maxResults);
	if (role == SUBJECT) {
		subjects->fillSuggestions(base, v2, maxResults);
	} else if (role == OBJECT) {
	//FIXME	objects->fillSuggestions(base, v2, maxResults);
	}

	// Merge results from shared and subjects/objects keeping order
	merge(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(out));

	// Remove possible extra items
	if (out.size() > maxResults) {
		out.resize(maxResults);
	}
}

}

