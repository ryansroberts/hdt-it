/*
 * TripleString.cpp
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

#include "TripleString.hpp"


namespace triples {

	TripleString::TripleString(string subject, string object, string predicate) {
		this->subject = subject;
		this->object = object;
		this->predicate = predicate;
	} // TripleString()

	TripleString::~TripleString() {
		// TODO Auto-generated destructor stub
	} // ~TripleString()

	string TripleString::getSubject() {
		return this->subject;
	} // getSubject()

	void TripleString::setSubject(string subject) {
		this->subject = subject;
	} // setSubject()

	string TripleString::getObject() {
		return this->object;
	} // getObject()

	void TripleString::setObject(string object) {
		this->object = object;
	} //setObject()

	string TripleString::getPredicate() {
		return this->predicate;
	} // getPredicate()

	void TripleString::setPredicate(string predicate) {
		this->predicate = predicate;
	} // setPredicate()

} // triples{}
