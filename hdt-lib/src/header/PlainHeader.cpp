/*
 * PlainHeader.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#include <HDTVocabulary.hpp>
#include <HDTFactory.hpp>
#include "../rdf/RDFParserN3.hpp"

#include "PlainHeader.hpp"

namespace hdt {

PlainHeader::PlainHeader() {
}

PlainHeader::PlainHeader(HDTSpecification &specification) : spec(specification) {
}

PlainHeader::~PlainHeader() {
}

void PlainHeader::load(std::istream & input, ControlInformation &controlInformation, ProgressListener *listener)
{
	std::string codification = controlInformation.get("codification");
	if(codification != HDTVocabulary::HEADER_PLAIN) {
		throw "Unexpected PlainHeader format";
	}

	RDFParserN3 parser(input);

	while(parser.hasNext()) {
		TripleString *ts = parser.next();
		triples.push_back(*ts);
	}
}

bool PlainHeader::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
{
	controlInformation.clear();
	controlInformation.set("codification", HDTVocabulary::HEADER_PLAIN);
	controlInformation.save(output);

	for(vector<TripleString>::iterator it = triples.begin(); it!=triples.end(); it++){
		output << *it << " ." << endl;
	}

	output << endl;
}

unsigned int PlainHeader::getNumberOfElements() {
	return triples.size();
}

void PlainHeader::insert(TripleString & triple)
{
	triples.push_back(triple);
}

void PlainHeader::remove(TripleString & triple)
{
	throw "Not implemented";
}

void PlainHeader::insert(IteratorTripleString *tripit)
{
	while(tripit->hasNext()){
		TripleString *next = tripit->next();
		triples.push_back(*next);
	}

}

void PlainHeader::remove(IteratorTripleString *triples)
{
	throw "Not implemented";
}

IteratorTripleString *PlainHeader::search(const char *subject, const char *predicate, const char *object)
{
    TripleString pattern(subject, predicate, object);

    return new PlainHeaderIteratorTripleString(this, pattern);
}



/// ITERATOR
PlainHeaderIteratorTripleString::PlainHeaderIteratorTripleString(PlainHeader *header, TripleString &pattern)
    : header(header), pos(0), pattern(pattern)
{
    doFetch();
}

void PlainHeaderIteratorTripleString::doFetch() {
	do {
		getNextTriple();
	} while(hasMoreTriples && (!nextTriple.match(pattern)));
}

void PlainHeaderIteratorTripleString::getNextTriple()
{
	if(pos<header->triples.size())
		nextTriple = header->triples[pos];

	pos++;

    hasMoreTriples = pos <= header->triples.size();
}

bool PlainHeaderIteratorTripleString::hasNext()
{
    return hasMoreTriples;
}

hdt::TripleString * PlainHeaderIteratorTripleString::next()
{
	returnTriple = nextTriple;
	doFetch();
    return &returnTriple;
}

}