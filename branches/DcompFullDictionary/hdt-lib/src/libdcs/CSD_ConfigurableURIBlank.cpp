/* CSD_ConfigurableURIBlank.cpp
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

#include "CSD_ConfigurableURIBlank.h"
#include "../libdcs/CSD_PFC.h"
#include "../libdcs/CSD_FMIndex.h"
#include <string.h>
#include <vector>

namespace csd {

class ConfigurableURIBlankIterator: public hdt::IteratorUCharString {
private:
	hdt::IteratorUCharString *child;
	unsigned char *previous, *nextItem;
	unsigned int readElements;
	bool goon;

public:
	ConfigurableURIBlankIterator(hdt::IteratorUCharString *child) :
			child(child), previous(NULL), nextItem(NULL), goon(false) {
		if (child->hasNext()) {
			nextItem = child->next();
		}
		readElements=0;
	}

	virtual ~ConfigurableURIBlankIterator() {
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
	unsigned int getReadElements(){
		return readElements;
	}
};

CSD_ConfigurableURIBlank::CSD_ConfigurableURIBlank() {
	this->type = CONFURIBLANK;
	this->URIs = new CSD_PFC();
	this->Blanks = new CSD_PFC();
	this->limitID=0;
	this->numstrings=0;
}

CSD_ConfigurableURIBlank::CSD_ConfigurableURIBlank(csd::CSD *URIs,
		csd::CSD *Blanks, hdt::IteratorUCharString *it,
		hdt::ProgressListener *listener) {

	this->type = CONFURIBLANK;
	this->URIs = URIs;
	this->Blanks = Blanks;
	ConfigurableURIBlankIterator ucharIt(it);

	if (this->Blanks->type == PFC) {
			((csd::CSD_PFC *) this->Blanks)->CSD_initialize(&ucharIt, listener);
		} else if (Blanks->type == FMINDEX) {
			((csd::CSD_FMIndex *) this->Blanks)->CSD_initialize(&ucharIt, listener);
		}

	this->limitID=ucharIt.getReadElements();

	ucharIt.doContinue();

	if (this->URIs->type == PFC) {
			((csd::CSD_PFC *) this->URIs)->CSD_initialize(&ucharIt, listener);
		} else if (URIs->type == FMINDEX) {
			((csd::CSD_FMIndex *) this->URIs)->CSD_initialize(&ucharIt, listener);
		}

	this->numstrings = this->URIs->getLength() + this->Blanks->getLength();
}

CSD_ConfigurableURIBlank::~CSD_ConfigurableURIBlank() {
	//delete this->URIs;
	//delete this->Blanks;
}

uint32_t CSD_ConfigurableURIBlank::locate(const unsigned char *s,
		uint32_t len) {
	if (s[0] != '_' || s[1] != ':'){
		return (this->limitID+this->URIs->locate(s,len));
	}
	else{
		return (this->Blanks->locate(s,len));
	}
}

unsigned char * CSD_ConfigurableURIBlank::extract(uint32_t id) {
	if (id<=this->limitID){
		return (this->Blanks->extract(id));
	}
	else{
		uint32_t iid = id-limitID;
		return (this->URIs->extract(iid));
	}
}

void CSD_ConfigurableURIBlank::freeString(const unsigned char *str) {
	this->URIs->freeString(str);
	this->Blanks->freeString(str);
}

unsigned int CSD_ConfigurableURIBlank::decompress(unsigned char **dict) {
	//FIXME needed?
}

uint64_t CSD_ConfigurableURIBlank::getSize() {
	return (this->URIs->getSize()+this->Blanks->getSize());
}

void CSD_ConfigurableURIBlank::save(ostream &fp) {
	saveValue<unsigned char>(fp, this->type);
	this->Blanks->save(fp);
	this->URIs->save(fp);
	//FIXME  ¿meter info adicional como el número de ids en cada componente?
	//saveValue<unsigned char>(fp, type);
	//saveValue<uint32_t>(fp, numstrings);
	//saveValue<uint32_t>(fp, tlength);
	//saveValue<uint32_t>(fp, maxlength);
	//fm_index->save(fp);

}

size_t CSD_ConfigurableURIBlank::load(unsigned char *ptr,
		unsigned char *ptrMax) {
	throw "Not implemented";
}

CSD * CSD_ConfigurableURIBlank::load(istream & fp) {
	//FIXME llamar a los load de los contenedores?
	CSD_ConfigurableURIBlank *newC = new CSD_ConfigurableURIBlank();
	newC->Blanks = csd::CSD::load(fp);
	newC->limitID = newC->Blanks->getLength();
	newC->URIs = csd::CSD::load(fp);
	newC->numstrings = newC->URIs->getLength() + newC->Blanks->getLength();
	return newC;
}

void CSD_ConfigurableURIBlank::dumpAll() {
	//FIXME: To be completed

}

void CSD_ConfigurableURIBlank::fillSuggestions(const char *base,
		vector<std::string> &out, int maxResults) {
	if (base[0] != '_' || base[1] != ':'){
		this->URIs->fillSuggestions(base,out,maxResults);
		}
		else{
			this->Blanks->fillSuggestions(base,out,maxResults);
		}
}

}
