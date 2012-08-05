#!/bin/bash

perl -ne 'chomp; print "SELECT ?s ?p ?o WHERE { ?s <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> $_ . ?s ?p ?o . }\n"' ../query2.txt > query2.txt
