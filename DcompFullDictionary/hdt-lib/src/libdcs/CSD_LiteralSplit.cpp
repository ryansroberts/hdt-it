/* CSD_LiteralSplit.cpp
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

#include <HDTListener.hpp>

#include "CSD_LiteralSplit.h"
#include "../libdcs/CSD_PFC.h"
#include "../libdcs/CSD_FMIndex.h"
#include <string.h>
#include <vector>

namespace csd {

class CSD_LiteralSplitIterator: public hdt::IteratorUCharString {
private:
	hdt::IteratorUCharString *child;
	unsigned char *previous, *nextItem;
	unsigned int readElements;
	bool goon;

public:
	CSD_LiteralSplitIterator(hdt::IteratorUCharString *child) :
			child(child), previous(NULL), nextItem(NULL), goon(false) {
		if (child->hasNext()) {
			nextItem = child->next();
		}
		readElements = 0;
	}

	virtual ~CSD_LiteralSplitIterator() {
		// Attention: Does not delete child.
	}

	bool hasNext() {
		if (goon) {
			return nextItem != NULL;
		} else {
			return nextItem != NULL && nextItem[0] == '_' && nextItem[1] == ':';
		}
	}

	unsigned char *next() {
		if (previous) {
			child->freeStr(previous);
		}
		previous = nextItem;
		if (child->hasNext()) {
			nextItem = child->next();
		} else {
			nextItem = NULL;
		}
		readElements++;
		return previous;
	}

	unsigned int getNumberOfElements() {
		return child->getNumberOfElements();
	}

	void doContinue() {
		goon = true;
	}
	unsigned int getReadElements() {
		return readElements;
	}
};

CSD_LiteralSplit::CSD_LiteralSplit() {
	this->type = LITERALSPLIT;
	this->limitArbitraryID = 0;
	this->limitIdiomsID = 0;
	this->numstrings = 0;
}

CSD_LiteralSplit::CSD_LiteralSplit(csd::CSD *Idioms, csd::CSD *Types,
		csd::CSD *Arbitrary, hdt::IteratorUCharString *it,
		hdt::ProgressListener *listener) {

	this->type = LITERALSPLIT;

	//split literals by type
	unsigned char *currentStr = NULL;
	size_t currentLength = 0;
	string previousStr;
	int quotesPos = 0, atPos = 0, caratPos = 0;
	char * quotesPos_c;
	char * atPos_c;
	char * caratPos_c;

	while (it->hasNext()) {
		currentStr = it->next();
		cout << "currentSTR:" << currentStr << endl;
		currentLength = strlen((char*) currentStr);
		quotesPos_c = strrchr((char*) currentStr, '"');
		atPos_c = strrchr((char*) currentStr, '@');
		caratPos_c = strrchr((char*) currentStr, '^');

		if (atPos_c == NULL && caratPos_c == NULL) {
			//insert arbitrary
			cout << "insert Arbitrary" << endl;
			litArbitrary.push_back((char*) currentStr);
		} else if (atPos_c > quotesPos_c) {
			atPos = atPos_c - (char*) currentStr;
			//literal has an idiom
			char * idiom = new char[currentLength - atPos];
			char * literal = new char[atPos + 1];
			strcpy(idiom, (char*) currentStr + atPos + 1);
			strncpy(literal, (char*) currentStr, atPos);
			literal[atPos] = '\0';
			cout << "literal has an idiom:" << idiom << ", and the literal is:"
					<< literal << ";" << endl;
			LitDictEntryIt it = litIdioms.find(idiom);
			if (it != litIdioms.end()) {
				//insert literal in existing idiom
				cout << "insert literal in existing idiom" << endl;
				it->second->strs.push_back(literal);
			} else {
				//insert new idiom and then insert literal
				cout << "insert new idiom and then insert literal" << endl;
				LiteralDictionaryEntry *entry = new LiteralDictionaryEntry;
				entry->strs.push_back(literal);
				entry->previousIDs = 0;
				litIdioms[idiom] = entry;
				difIdioms.push_back(idiom);
			}
		} else if (caratPos_c > quotesPos_c) {
			caratPos = caratPos_c - (char*) currentStr;
			//literal has a type

			char * type = new char[currentLength - caratPos];
			char * literal = new char[caratPos];
			strcpy(type, (char*) currentStr + caratPos + 1);
			strncpy(literal, (char*) currentStr, caratPos - 1);
			literal[caratPos - 1] = '\0';
			cout << "literal has a type:" << type << ", and a literal:"
					<< literal << ";" << endl;
			LitDictEntryIt it = litTypes.find(type);
			if (it != litTypes.end()) {
				//insert literal in existing type
				cout << "insert literal in existing type" << endl;
				it->second->strs.push_back(literal);
			} else {
				//insert new type and then insert literal
				cout << "insert new type and then insert literal" << endl;
				LiteralDictionaryEntry *entry = new LiteralDictionaryEntry;
				entry->strs.push_back(literal);
				entry->previousIDs = 0;
				litTypes[type] = entry;
				difTypes.push_back(type);
			}

		} else {
			//insert arbitrary
			cout << "insert arbitrary" << endl;
			litArbitrary.push_back((char*) currentStr);
		}
	}

	//Create Dictionary of Arbitrary elements
	VectCharIterator *vectIt = new VectCharIterator(litArbitrary);
	if (Arbitrary->type == PFC) {
		CSDArbitrary = new csd::CSD_PFC((csd::CSD_PFC *) Arbitrary);

		((csd::CSD_PFC *) CSDArbitrary)->CSD_initialize(vectIt, listener);
	} else if (Arbitrary->type == FMINDEX) {
		CSDArbitrary = new csd::CSD_FMIndex((csd::CSD_FMIndex *) Arbitrary);
		((csd::CSD_FMIndex *) CSDArbitrary)->CSD_initialize(vectIt, listener);
	}

	// Sort different idioms, iterate while creating Dictionary and compute Ids of each part (as its number of elements)
	sort(difIdioms.begin(), difIdioms.end());
	int currentID = 0;
	int totalIdiomsID = 0;
	LitDictEntryIt currentEntry;

	cout << endl << "Iterando sobre los diferentes idiomas" << endl;
	for (int i = 0; i < difIdioms.size(); i++) {
		cout << "Idioma:" << difIdioms[i] << endl;
		currentEntry = litIdioms.find(difIdioms[i]);
		currentEntry->second->previousIDs = totalIdiomsID;
		cout << ">>>>idiomsPushBack:" << currentEntry->second->strs.size()
				<< endl;
		idiomIds.push_back(currentEntry->second->strs.size() + totalIdiomsID);
		totalIdiomsID += currentEntry->second->strs.size();

		//create dictionary of the current subpart
		VectCharIterator *vectIt = new VectCharIterator(
				currentEntry->second->strs);
		csd::CSD * newDict;
		if (Idioms->type == PFC) {
			newDict = new csd::CSD_PFC((csd::CSD_PFC *) Idioms);

			((csd::CSD_PFC *) newDict)->CSD_initialize(vectIt, listener);
		} else if (Idioms->type == FMINDEX) {
			newDict = new csd::CSD_FMIndex((csd::CSD_FMIndex *) Idioms);
			((csd::CSD_FMIndex *) newDict)->CSD_initialize(vectIt, listener);
		}
		CSDIdioms[difIdioms[i]] = newDict;
		idiomIdsDict.push_back(newDict);
		// As the idioms are sorted, idiomIdsDict follow the same order as CSDIdioms

	}
	// Sort different types, iterate while creating Dictionary and compute Ids of each part (as its number of elements)
	sort(difTypes.begin(), difTypes.end());
	currentID = 0;
	int totalTypesID = 0;
	cout << endl << "Iterando sobre los diferentes tipos" << endl;
	for (int i = 0; i < difTypes.size(); i++) {
		cout << "tipo:" << difTypes[i] << endl;
		currentEntry = litTypes.find(difTypes[i]);
		currentEntry->second->previousIDs = totalTypesID;
		typeIds.push_back(currentEntry->second->strs.size() + totalTypesID);
		totalTypesID += currentEntry->second->strs.size();

		//create dictionary of the current subpart
		VectCharIterator *vectIt = new VectCharIterator(
				currentEntry->second->strs);
		csd::CSD * newDict;
		if (Types->type == PFC) {
			newDict = new csd::CSD_PFC((csd::CSD_PFC *) Types);

			((csd::CSD_PFC *) newDict)->CSD_initialize(vectIt, listener);
		} else if (Types->type == FMINDEX) {
			newDict = new csd::CSD_FMIndex((csd::CSD_FMIndex *) Types);
			((csd::CSD_FMIndex *) newDict)->CSD_initialize(vectIt, listener);
		}
		CSDTypes[difTypes[i]] = newDict;
		typeIdsDict.push_back(newDict);
		// As the types are sorted, typeIdsDict follow the same order as CSDTypes

	}

	this->limitArbitraryID = this->CSDArbitrary->getLength();
	this->limitIdiomsID = this->limitArbitraryID + totalIdiomsID;

	this->numstrings = this->CSDArbitrary->getLength() + totalIdiomsID
			+ totalTypesID;

	//erase auxiliars
	litIdioms.clear();
	litTypes.clear();
	litArbitrary.clear();
}

CSD_LiteralSplit::~CSD_LiteralSplit() {
	//delete this->URIs;
	//delete this->Blanks;
}

uint32_t CSD_LiteralSplit::locate(const unsigned char *s, uint32_t len) {

	int quotesPos = 0, atPos = 0, caratPos = 0;
	char * quotesPos_c;
	char * atPos_c;
	char * caratPos_c;
	uint32_t localId = 0;

	cout << "locateSTR:" << s << ";" << endl;
	quotesPos_c = strrchr((char*) s, '"');
	atPos_c = strrchr((char*) s, '@');
	caratPos_c = strrchr((char*) s, '^');

	if (atPos_c == NULL && caratPos_c == NULL) {
		// arbitrary
		cout << "it is Arbitrary, there are in arbs:"
				<< CSDArbitrary->getLength() << endl;
		cout << "and locate is:" << CSDArbitrary->locate(s, len) << endl;
		cout << "     return is:" << CSDArbitrary->locate(s, len) << endl;
		return CSDArbitrary->locate(s, len);
	} else if (atPos_c > quotesPos_c) {
		atPos = atPos_c - (char*) s;
		//literal has an idiom
		char * idiom = new char[len - atPos];
		cout << "atPos:" << atPos << endl;
		char * literal = new char[atPos + 1];
		strcpy(idiom, (char*) s + atPos + 1);
		strncpy(literal, (char*) s, atPos);
		literal[atPos] = '\0';
		cout << "it is idiom:" << idiom << ",and literal:" << literal << ";"
				<< endl;
		cout << "it is idiom:" << idiom << ",and literal:" << literal << ";"
				<< endl;
		CSDDictEntryIt it = CSDIdioms.find(idiom);
		if (it != CSDIdioms.end()) {
			localId = it->second->locate((unsigned char*) literal,
					strlen(literal));
			if (localId == 0)
				return 0; //not exists
			int posIdiom; //know the position of the idiom to get the previous ids;
			posIdiom = find_unique(difIdioms, idiom);
			int prevIds = 0;
			if (posIdiom != 0) {
				prevIds = idiomIds[posIdiom - 1];
			}
			cout << "     return is:" << localId + prevIds + limitArbitraryID
					<< endl;
			return localId + prevIds + limitArbitraryID;
		} else { //idiom not exists
			cout << "     NO return idiom!!" << endl;
			return 0;
		}
	} else if (caratPos_c > quotesPos_c) {
		caratPos = caratPos_c - (char*) s;
		//literal has a type

		char * type = new char[len - caratPos];
		char * literal = new char[caratPos];
		strcpy(type, (char*) s + caratPos + 1);
		strncpy(literal, (char*) s, caratPos - 1);
		literal[caratPos - 1] = '\0';
		cout << "literal has a type:" << type << ", and a literal:" << ";"
				<< literal << endl;
		CSDDictEntryIt it = CSDTypes.find(type);
		if (it != CSDTypes.end()) {
			localId = it->second->locate((unsigned char*) literal,
					strlen(literal));
			if (localId == 0)
				return 0; //not exists
			int posType; //know the position of the type to get the previous ids;
			posType = find_unique(difTypes, type);
			int prevIds = 0;
			if (posType != 0) {
				prevIds = typeIds[posType - 1];
			}
			cout << "     return is:" << localId + prevIds + limitIdiomsID
					<< endl;
			;
			return localId + prevIds + limitIdiomsID;
		} else { //type not exists
			cout << "     NO return type!!" << endl;
			return 0;
		}
	}
}

unsigned char * CSD_LiteralSplit::extract(uint32_t id) {

	cout << "Extracting id:" << id << endl;
	if (id <= this->limitArbitraryID) {
		cout << "id arbitrary" << endl;
		cout << "return" << this->CSDArbitrary->extract(id) << endl;
		return (this->CSDArbitrary->extract(id));
	} else {
		if (id <= limitIdiomsID) { //idiom literal
			uint32_t iid = id - limitArbitraryID;
			cout << "id idiom:" << iid << endl;
			int subdicIndex = up_bound(idiomIds, iid);
			cout << "subdicIndex:" << subdicIndex << endl;
			if (subdicIndex > 0) //transform id to local
				iid = iid - idiomIds[subdicIndex - 1];
			cout << "return" << idiomIdsDict[subdicIndex]->extract(iid) << endl;
			char * ext = (char*) idiomIdsDict[subdicIndex]->extract(iid);
			char * idiom = difIdioms[subdicIndex];
			char * ret = new char[strlen(ext) + strlen(idiom) + 2];
			strcpy(ret, ext);
			ret[strlen(ext)] = '@';
			cout << "idiom=" << idiom << endl;
			strcpy(ret + strlen(ext) + 1, idiom);
			ret[strlen(ext) + strlen(idiom) + 1] = '\0';
			return (unsigned char *) ret;
		} else if (id <= numstrings) { //typed literal
			uint32_t iid = id - limitIdiomsID;
			cout << "id type:" << iid << endl;

			int subdicIndex = up_bound(typeIds, iid);
			cout << "subdicIndex:" << subdicIndex << endl;
			if (subdicIndex > 0) //transform id to local
				iid = iid - typeIds[subdicIndex - 1];
			cout << "return" << typeIdsDict[subdicIndex]->extract(iid) << endl;

			char * ext = (char*) typeIdsDict[subdicIndex]->extract(iid);
			char * type = difTypes[subdicIndex];
			char * ret = new char[strlen(ext) + strlen(type) + 3];
			strcpy(ret, ext);
			ret[strlen(ext)] = '^';
			ret[strlen(ext) + 1] = '^';
			cout << "type=" << type << endl;
			strcpy(ret + strlen(ext) + 2, type);
			ret[strlen(ext) + strlen(type) + 2] = '\0';
			return (unsigned char *) ret;

		} else
			//error
			return NULL;
	}

	//return (this->idioms->extract(iid));

}

void CSD_LiteralSplit::freeString(const unsigned char *str) {
	delete[] str;
}

unsigned int CSD_LiteralSplit::decompress(unsigned char **dict) {
	//FIXME needed?
}

uint64_t CSD_LiteralSplit::getSize() {

	uint32_t ret = 0;
	ret = CSDArbitrary->getSize();
	for (int i = 0; i < idiomIdsDict.size(); i++) {
		ret += idiomIdsDict[i]->getSize();
	}
	for (int i = 0; i < typeIdsDict.size(); i++) {
		ret += typeIdsDict[i]->getSize();
	}
	return ret;
}

void CSD_LiteralSplit::save(ostream &fp) {
	saveValue<unsigned char>(fp, this->type);
	saveValue<uint32_t>(fp, numstrings);
	saveValue<uint32_t>(fp, limitArbitraryID);
	saveValue<uint32_t>(fp, limitIdiomsID);
	saveValue<uint32_t>(fp, difIdioms.size()); //different idioms
	saveValue<uint32_t>(fp, difTypes.size()); //different types
	this->CSDArbitrary->save(fp);
	for (int i = 0; i < difIdioms.size(); i++) {
		fp << difIdioms[i] << endl;
		idiomIdsDict[i]->save(fp);
	}
	for (int i = 0; i < difTypes.size(); i++) {
		fp << difTypes[i] << endl;
		typeIdsDict[i]->save(fp);
	}
	//FIXME  ¿meter info adicional como el número de ids en cada componente?
	//saveValue<unsigned char>(fp, type);
	//saveValue<uint32_t>(fp, numstrings);
	//saveValue<uint32_t>(fp, tlength);
	//saveValue<uint32_t>(fp, maxlength);
	//fm_index->save(fp);

}

size_t CSD_LiteralSplit::load(unsigned char *ptr, unsigned char *ptrMax) {
	throw "Not implemented";
}

CSD * CSD_LiteralSplit::load(istream & fp) {
	CSD_LiteralSplit *newC = new CSD_LiteralSplit();
	newC->numstrings = loadValue<uint32_t>(fp);
	cout << "numstrings:" << newC->numstrings << endl;
	newC->limitArbitraryID = loadValue<uint32_t>(fp);
	cout << "limitArbitraryID:" << newC->limitArbitraryID << endl;
	newC->limitIdiomsID = loadValue<uint32_t>(fp);
	cout << "limitIdiomsID:" << newC->limitIdiomsID << endl;
	uint numIdioms = loadValue<uint32_t>(fp);
	cout << "numIdioms:" << numIdioms << endl;
	uint numTypes = loadValue<uint32_t>(fp);
	cout << "numTypes:" << numTypes << endl;
	newC->CSDArbitrary = csd::CSD::load(fp);

	int totalIdiomsID = 0;

	for (int i = 0; i < numIdioms; i++) {
		string currIdiom = "";
		getline(fp, currIdiom);
		cout << "currIdiom:" << currIdiom << endl;
		char * newidiom = new char[currIdiom.length() + 1];
		strcpy(newidiom, (char*) currIdiom.c_str());
		newidiom[currIdiom.length()] = '\0';
		newC->difIdioms.push_back(newidiom);
		cout << "insertado currIdiom:" << newC->difIdioms[i] << endl;
		if (i > 0)
			cout << "previous currIdiom:" << newC->difIdioms[i - 1] << endl;
		csd::CSD *newIdiom = csd::CSD::load(fp);
		newC->idiomIdsDict.push_back(newIdiom);
		newC->CSDIdioms[(char*) currIdiom.c_str()] = newIdiom;
		newC->idiomIds.push_back(newIdiom->getLength() + totalIdiomsID);
		totalIdiomsID += newIdiom->getLength();
	}

	int totalTypesID = 0;
	for (int i = 0; i < numTypes; i++) {
		string currType = "";
		getline(fp, currType);
		cout << "currType:" << currType << endl;
		char * newtype = new char[currType.length() + 1];
		strcpy(newtype, (char*) currType.c_str());
		newtype[currType.length()] = '\0';

		newC->difTypes.push_back(newtype);
		csd::CSD *newType = csd::CSD::load(fp);
		newC->typeIdsDict.push_back(newType);
		newC->CSDTypes[(char*) currType.c_str()] = newType;
		newC->typeIds.push_back(newType->getLength() + totalTypesID);
		totalTypesID += newType->getLength();
	}
	cout << "fin load" << endl;
	return newC;

}

void CSD_LiteralSplit::dumpAll() {
	//FIXME: To be completed

}

void CSD_LiteralSplit::fillSuggestions(const char *base,
		vector<std::string> &out, int maxResults) {
	/*	if (base[0] != '_' || base[1] != ':') {
	 this->idioms->fillSuggestions(base, out, maxResults);
	 } else {
	 this->types->fillSuggestions(base, out, maxResults);
	 }
	 */
}

}
