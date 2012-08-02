/* CSD_FMIndex.cpp
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
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
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#include "CSD_FMIndex.h"
#include <string.h>
#include <vector>

namespace csd {

CSD_FMIndex::CSD_FMIndex() {
	fm_index = NULL;
	separators = NULL;
}

CSD_FMIndex::CSD_FMIndex(char *file_name, bool sparse_bitsequence, int bparam,
		size_t bwt_sample, bool use_sample) {
	type = FMINDEX;
	char *text;
	uint *bitmap = 0;
	ifstream in(file_name);
	if (!in.good())
		abort();

	in.seekg(0, ios_base::end);
	size_t len = in.tellg();
	tlength = len;
	in.seekg(0, ios_base::beg);
	text = new char[len + 2];
	if (use_sample) {
		bitmap = new uint[(len + 1 + W) / W];
		for (uint i = 0; i < (len + 1 + W) / W; i++)
			bitmap[i] = 0;
		bitset(bitmap, 0);
	}
	text[0] = '\1'; //We suppose that \1 is not part of the text
	text[len + 1] = '\0'; //end of the text
	in.read((char *) (text + 1), len);
	in.close();
	maxlength = 0;
	length = 0;
	uint m_l = 0;
	for (size_t i = 1; i <= len; i++) {
		if (text[i] == '\n' || text[i] == '\0') {
			text[i] = '\1';
			if (use_sample)
				bitset(bitmap, i);
			length++;
			if (m_l > maxlength)
				maxlength = m_l;
			m_l = 0;
		} else
			m_l++;
	}
	len = len + 2;
	//just one '\0' at the end
	while (text[len - 2] == text[len - 3]) {
		text[len - 2] = '\0';
		len--;
	}
	build_ssa((uchar *) text, len, sparse_bitsequence, bparam, use_sample,
			bwt_sample);
	if (use_sample) {
		separators = new BitSequenceRRR(bitmap, len);
		delete[] bitmap;
	}
	delete[] text;
}

CSD_FMIndex::CSD_FMIndex(uchar *dict, uint tlength, bool sparse_bitsequence,
		int bparam, size_t bwt_sample, bool use_sample) {
	type = FMINDEX;
	char *text;
	uint *bitmap = 0;
	this->tlength = tlength;
	size_t len = tlength;
	text = new char[len + 2];
	if (use_sample) {
		bitmap = new uint[(len + 1 + W) / W];
		for (uint i = 0; i < (len + 1 + W) / W; i++)
			bitmap[i] = 0;
		bitset(bitmap, 0);
	}
	text[0] = '\1'; //We suppose that \1 is not part of the text
	text[len + 1] = '\0'; //end of the text
	maxlength = 0;
	length = 0;
	uint m_l = 0;
	for (size_t i = 1; i <= len; i++) {
		text[i] = (char) dict[i - 1];
		if (text[i] == '\n' || text[i] == '\0') {
			text[i] = '\1';
			if (use_sample)
				bitset(bitmap, i);
			length++;
			if (m_l > maxlength)
				maxlength = m_l;
			m_l = 0;
		} else
			m_l++;
	}
	len = len + 2;
	//just one '\0' at the end
	while (text[len - 2] == text[len - 3]) {
		text[len - 2] = '\0';
		len--;
	}
	build_ssa((uchar *) text, len, sparse_bitsequence, bparam, use_sample,
			bwt_sample);
	if (use_sample) {
		separators = new BitSequenceRRR(bitmap, len);
		delete[] bitmap;
	}
	delete[] text;
}

CSD_FMIndex::CSD_FMIndex(IteratorUCharString *it, uchar *stopword,
		bool sparse_bitsequence, int bparam, size_t bwt_sample, bool use_sample,
		hdt::ProgressListener *listener) {

	this->type = FMINDEX;
	string element;
	uchar *text;
	uint *bitmap = 0;
	//uint32_t *bitmap = 0;

	size_t len = 0;
	size_t reservedSize = 1024;
	text = (uchar*) malloc(reservedSize * sizeof(uchar));
	std:vector < uint > samplingsPositions;

	text[0] = '\1'; //We suppose that \1 is not part of the text
	maxlength = 0;
	length = 0;
	uint m_l = 0;

	uint total = 1;

	uchar *currentStr = NULL;
	uint currentLength = 0;

	bool stop = false;
	if (it->hasNext()) {
		currentStr = it->next();
//		cout<<"testing: curentStr:"<<currentStr<<endl;
//		cout<<"testing: stopword[0]:"<<stopword[0]<<endl;
		if (currentStr[0] != stopword[0])
		//FIXME only compares the first string with is enough now for '"' prefix
				{
			stop = true;
		}
	}

	while (!stop) {
		length++; //new element

	//	cout<<"testing:current string:"<<currentStr<<endl;

		currentLength = strlen((char*) currentStr);

		if (currentLength > maxlength)
			maxlength = currentLength;

		// Checking the current size of the encoded
		// sequence: realloc if necessary
		if ((total + currentLength + 1) > reservedSize) {
			while (((size_t) total + currentLength + 1) > reservedSize) {
				reservedSize <<= 1;
				if (reservedSize == 0) {
					reservedSize = ((size_t) total + currentLength) * 2;
				}
			}
			text = (uchar*) realloc(text, reservedSize * sizeof(uchar));
		}
		strncpy((char*)(text+total), (char*)currentStr, currentLength);
//		cout<<"testing:total:"<<total<<endl;
		total +=currentLength;
//		cout<<"testing:total post:"<<total<<endl;
		text[total] = '\1';

		if (use_sample)
			samplingsPositions.push_back(total);
		total++;

		stop = false;
		if (it->hasNext()) {
			currentStr = it->next();
			if (currentStr[0] != stopword[0])
			//FIXME only compares the first string with is enough now for '"' prefix
					{
				stop = true;
			}
		}
	}

	this->tlength = total;
	char *textFinal;
	textFinal = new char[total+ 1];
//	cout<<"testing:total cpy:"<<total<<endl;
//	cout<<"testing:text:"<<text<<endl;
	strncpy((char*)(textFinal), (char*)text, total);
	textFinal[total] = '\0'; //end of the text
//	cout<<"testing:textFinal:"<<textFinal<<endl;

	len = tlength;

	len = total + 1;
	//just one '\0' at the end
	while (textFinal[len - 2] == textFinal[len - 3]) {
		textFinal[len - 2] = '\0';
		len--;
	}

	if (use_sample) {
		 bitmap = new uint[(total + 1 + W) / W];
		 for (uint i = 0; i < (total + 1 + W) / W; i++)
		 bitmap[i] = 0;
		 bitset(bitmap, 0);
		 for (uint i=0;i<samplingsPositions.size();i++){
			 bitset(bitmap, samplingsPositions[i]);
		 }
	}
//	cout<<"testing:len:"<<len<<endl;
//	cout<<"testing:textFinal:"<<textFinal<<endl;

	build_ssa((uchar *) textFinal, len, sparse_bitsequence, bparam, use_sample,
			bwt_sample);
	if (use_sample) {
		separators = new BitSequenceRRR(bitmap, len);
		delete[] bitmap;
	}
	delete[] text;

}

CSD_FMIndex::CSD_FMIndex(set<string>::iterator dict_begin,
		set<string>::iterator dict_end, uint tlength, bool sparse_bitsequence,
		int bparam, size_t bwt_sample, bool use_sample) {
	type = FMINDEX;
	set<string>::iterator dict;
	string element;
	char *text;
	uint *bitmap = 0;
	this->tlength = tlength;
	size_t len = tlength;
	text = new char[len + 2];
	if (use_sample) {
		bitmap = new uint[(len + 1 + W) / W];
		for (uint i = 0; i < (len + 1 + W) / W; i++)
			bitmap[i] = 0;
		bitset(bitmap, 0);
	}
	text[0] = '\1'; //We suppose that \1 is not part of the text
	text[len + 1] = '\0'; //end of the text
	maxlength = 0;
	length = 0;
	uint m_l = 0;

	uint total = 1;
	for (dict = dict_begin; dict != dict_end; dict++) {
		element = *dict;
		for (uint i = 0; i < element.size(); i++) {
			text[total] = (char) element.at(i);
			m_l++;
			total++;
		}
		text[total] = '\1';
		if (use_sample)
			bitset(bitmap, total);
		length++;
		if (m_l > maxlength)
			maxlength = m_l;
		m_l = 0;
		total++;
	}

	len = len + 2;
	//just one '\0' at the end
	while (text[len - 2] == text[len - 3]) {
		text[len - 2] = '\0';
		len--;
	}

	build_ssa((uchar *) text, len, sparse_bitsequence, bparam, use_sample,
			bwt_sample);
	if (use_sample) {
		separators = new BitSequenceRRR(bitmap, len);
		delete[] bitmap;
	}
	delete[] text;
}

void CSD_FMIndex::build_ssa(uchar *text, size_t len, bool sparse_bitsequence,
		int bparam, bool use_sample, size_t bwt_sample) {
	use_sampling = use_sample;
	fm_index = new SSA((uchar *) text, len, false, use_sample);
	Mapper * am = new MapperNone();
	wt_coder * wc = new wt_coder_huff((uchar *) text, len, am);
	BitSequenceBuilder * sbb;
	if (sparse_bitsequence)
		sbb = new BitSequenceBuilderRRR(bparam);
	else
		sbb = new BitSequenceBuilderRG(bparam);
	fm_index->set_static_bitsequence_builder(sbb);
	SequenceBuilder * ssb = new SequenceBuilderWaveletTree(sbb, am, wc);
	fm_index->set_static_sequence_builder(ssb);
	fm_index->set_samplesuff(bwt_sample);
	fm_index->build_index();
}

CSD_FMIndex::~CSD_FMIndex() {
	if (fm_index != NULL)
		delete fm_index;
	if (use_sampling)
		if (separators != NULL)
			delete separators;
}

uint32_t CSD_FMIndex::locate(const uchar *s, uint32_t len) {
	uchar *n_s = new uchar[len + 2];
	uint o;
	n_s[0] = '\1';
	for (uint32_t i = 1; i <= len; i++)
		n_s[i] = s[i - 1];
	n_s[len + 1] = '\1';
	o = fm_index->locate_id(n_s, len + 2);
	delete[] n_s;
	if (o != 0)
		return o - 2;
	return 0;
}

uint32_t CSD_FMIndex::locate_substring(uchar *s, uint32_t len,
		uint32_t **occs) {
	if (!use_sampling) {
		*occs = NULL;
		return 0;
	}
	uint num_occ, i;
	uint32_t res = 0;
	uint32_t temp;
	num_occ = fm_index->locate(s, (uint) len, occs);
	if (num_occ == 0) {
		*occs = NULL;
		return 0;
	}
	quicksort((*occs), 0, num_occ - 1);
	i = 1;
	(*occs)[res] = separators->rank1((*occs)[0]);
	while (i < num_occ) {
		temp = separators->rank1((*occs)[i]);
		if (temp != (*occs)[res]) {
			(*occs)[res + 1] = temp;
			res++;
		}
		i++;
	}
	return res + 1;
}

uchar * CSD_FMIndex::extract(uint32_t id) {
	if (id == 0 || id > length)
		return NULL;
	uint i;
	if (id == length)
		i = 2;
	else
		i = id + 3;
	return fm_index->extract_id(i, maxlength);
}

uint CSD_FMIndex::decompress(uchar **dict) {
	uint len = 0;
	uchar *text = new uchar[tlength];
	uchar *str;
	uint j;
	for (uint i = 0; i < length; i++) {
		str = extract(i + 1);
		j = 0;
		while (str[j] != '\0') {
			text[len] = str[j];
			len++;
			j++;
		}
		text[len] = '\n';
		len++;
		delete[] str;
	}
	text[len] = '\0';
	len++;
	*dict = text;
	return len;
}

uint64_t CSD_FMIndex::getSize() {
	uint64_t mem = sizeof(CSD_FMIndex);
	mem += fm_index->size();
	if (use_sampling)
		mem += separators->getSize();
	return mem;
}

void CSD_FMIndex::save(ofstream & fp) {
	saveValue<uint32_t>(fp, type);
	saveValue<uint32_t>(fp, length);
	saveValue<uint32_t>(fp, tlength);
	saveValue<uint32_t>(fp, maxlength);
	saveValue<bool>(fp, use_sampling);
	if (use_sampling)
		separators->save(fp);
	fm_index->save(fp);

}

CSD * CSD_FMIndex::load(ifstream & fp) {
	CSD_FMIndex *fm = new CSD_FMIndex();
//		uint32_t tipo = loadValue<uint32_t>(fp);
//	cout<<"testing:type:"<<tipo<<";"<<endl;

//	cout<< "testing length:"<<loadValue<uint32_t>(fp)<<endl;
	fflush(stdout);

//	if (type != FMINDEX)
//		abort();
	fm->type = FMINDEX;

	fm->length = loadValue<uint32_t>(fp);
	fm->tlength = loadValue<uint32_t>(fp);
	fm->maxlength = loadValue<uint32_t>(fp);
	fm->use_sampling = loadValue<bool>(fp);
	if (fm->use_sampling)
		fm->separators = BitSequence::load(fp);
	fm->fm_index = SSA::load(fp);

	return fm;
}

size_t CSD_FMIndex::get_pivot(size_t *occs, size_t ini, size_t end) {
	size_t izq, der;
	size_t pivot;
	size_t temp;
	pivot = occs[ini];
	izq = ini;
	der = end;
	while (izq < der) {
		while (occs[der] > pivot)
			der--;
		while ((izq < der) && (occs[izq] <= pivot))
			izq++;
		if (izq < der) {
			temp = occs[izq];
			occs[izq] = occs[der];
			occs[der] = temp;
		}
	}
	temp = occs[der];
	occs[der] = occs[ini];
	occs[ini] = temp;
	return der;
}

void CSD_FMIndex::quicksort(size_t *occs, size_t ini, size_t end) {
	size_t pivot;
	if (ini < end) {
		pivot = get_pivot(occs, ini, end);
		if (pivot == ini)
			return;
		quicksort(occs, ini, pivot - 1);
		quicksort(occs, pivot + 1, end);
	}
}

void CSD_FMIndex::dumpAll() {
	//FIXME: To be completed

}

void csd::CSD_FMIndex::fillSuggestions(const char *base,
		vector<std::string> &out, int maxResults) {
	//FIXME: To be completed
}

}
;
