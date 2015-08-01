/******************************************************************************
* Copyright (c) 2005, 2015  Ericsson AB
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
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

