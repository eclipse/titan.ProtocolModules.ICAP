/******************************************************************************
* Copyright (c) 2000-2019 Ericsson Telecom AB
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v2.0
* which accompanies this distribution, and is available at
* https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
*
* Contributors:
* Gabor Szalai
* Janos Kovesdi
* Eduard Czimbalmos
******************************************************************************/
//
//  File:               ICAP_parse.h
//  Rev:                R2A
//  Prodnr:             CNL 113 779 
//  Reference:          RFC3507, RFC 2616 
#ifndef ICAP_parse_parser_H
#define ICAP_parse_parser_H
#include "ICAP_Types.hh"
int ICAP_parse_parse();

int ICAP_parse_lex();

int ICAP_parse_error(const char *s);

#endif

