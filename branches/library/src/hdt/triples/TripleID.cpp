/*
 * TripleID.cpp
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

#include "TripleID.hpp"

namespace triples {

	TripleID::TripleID(int subject, int object, int predicate) {
		this->subject = subject;
		this->object = object;
		this->predicate = predicate;
	} // TripleID()

	TripleID::~TripleID() {
		// TODO Auto-generated destructor stub
	} // ~TripleID()

	int TripleID::getSubject() {
		return this->subject;
	} // getSubject()

	void TripleID::setSubject(int subject) {
		this->subject = subject;
	} // setSubject()

	int TripleID::getObject() {
		return this->object;
	} // getObject()

	void TripleID::setObject(int object) {
		this->object = object;
	} //setObject()

	int TripleID::getPredicate() {
		return this->predicate;
	} // getPredicate()

	void TripleID::setPredicate(int predicate) {
		this->predicate = predicate;
	} // setPredicate()

} // triples{}
