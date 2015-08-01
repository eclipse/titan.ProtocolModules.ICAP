%{
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
//  File:               ICAP.l
//  Rev:                R2A
//  Prodnr:             CNL 113 779 
//  Reference:          RFC3507, RFC 2616 

#include "ICAP_parse.h"
#include "ICAP_parse_.tab.h"

using namespace ICAP__Types;

extern PDU__ICAP* ICAP_parse_parsed_message;
extern RequestHeaders* RequestHeaders_parsed;
extern ResponseHeaders* ResponseHeaders_parsed;
extern Encapsulated* encapsulated_parsed;

extern int ICAP_parse_debug;
extern int ICAP_parse_parse();
int chr_pos;
int error_occured;
int product_count;
bool isRequest;
int ICAP_parse_error(const char *s) {
	if (ICAP_parse_text[0]=='\n') {
      	TTCN_warning("ICAP EncDec:  %s in \"\\n\" at position %d",s,chr_pos);
  }
	else if (ICAP_parse_text[0]=='\r') {
       	TTCN_warning("ICAP EncDec:  %s in \"\\r\" at position %d",s,chr_pos);
  }
 	else {
        TTCN_warning("ICAP EncDec:  %s in \"%s\" -> unexpected token at position %d",s,ICAP_parse_text,chr_pos);
  }
  error_occured=1;
  return 0;
}

%}

%option noyywrap
%option nounput

%x REQUESTLINE
%x REQUESTLINE2
%x REQUESTLINE3
%x QUERY
%x STATUSLINE
%x GENHEADER
%x REQUESTHEADER
%x RESPONSEHEADER
%x SP_STR
%x SP_STR2
%x ENCAP
%x MAIL
%x URI
%x TOKENS
%x ISTAG
%x OHEADER
%x HTTP_HEADER
%x HTTP_BODIES

CR      [\r]
LF      [\n]
A       [Aa]
B       [Bb]
C       [Cc]
D       [Dd]
E       [Ee]
F       [Ff]
G       [Gg]
H       [Hh]
I       [Ii]
J       [Jj]
K       [Kk]
L       [Ll]
M       [Mm]
N       [Nn]
O       [Oo]
P       [Pp]
Q       [Qq]
R       [Rr]
S       [Ss]
T       [Tt]
U       [Uu]
V       [Vv]
W       [Ww]
X       [Xx]
Y       [Yy]
Z       [Zz]
DIGIT   [0-9]
COLON   [:]
CRLF    {CR}{LF}

TOKEN   ([[:alnum:]]|[-]|[.]|[!]|[%]|[\*]|[_]|[\+]|[`]|[']|[~])+
HEX     [A-Fa-f]|{DIGIT}

METHOD  ({R}{E}{Q}{M}{O}{D})|({R}{E}{S}{P}{M}{O}{D})|({O}{P}{T}{I}{O}{N}{S})
VERSION {I}{C}{A}{P}[/]{DIGIT}[.]{DIGIT}
URI     ({TOKEN}|[/]|{COLON})+
QUERY   ({TOKEN}|[=])+
REASON  ([[:alnum:]]|[_])+
TOKEN_SP({TOKEN}|[,]|[ ]|{COLON})+
MAIL    ({TOKEN}|[@])+
PRODUCT ({TOKEN}|[/])+

%%
<INITIAL>{METHOD}                           {chr_pos+=ICAP_parse_leng;isRequest = true;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);BEGIN(REQUESTLINE);return METHOD;}

<REQUESTLINE>{TOKEN}                        {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return SCHEME;}
<REQUESTLINE>"://"                          {chr_pos+=ICAP_parse_leng;BEGIN(REQUESTLINE2);return DSLASH;}
<REQUESTLINE2>{URI}                         {chr_pos+=ICAP_parse_leng;BEGIN(REQUESTLINE3);ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return NET_PATH;}
<REQUESTLINE3>"?"                           {chr_pos+=ICAP_parse_leng;BEGIN(QUERY);return QM;}
<REQUESTLINE3>{VERSION}                     {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return VERSION_TOKEN;}
<REQUESTLINE3>{CRLF}                        {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}

<QUERY>{QUERY}                              {chr_pos+=ICAP_parse_leng;BEGIN(REQUESTLINE3);ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return QUERY_TOKEN;}


<INITIAL>{VERSION}                          {chr_pos+=ICAP_parse_leng;BEGIN(STATUSLINE);ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return VERSION_TOKEN;}

<STATUSLINE>{DIGIT}+                        {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.integer = new INTEGER(ICAP_parse_text);return INT_TOKEN;}
<STATUSLINE>{REASON}                        {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return REASON;}
<STATUSLINE>{CRLF}                          {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}


<INITIAL>{A}{U}{T}{H}{O}{R}{I}{Z}{A}{T}{I}{O}{N}{COLON}     {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return AUTHORIZATION;}
<INITIAL>{A}{L}{L}{O}{W}{COLON}                             {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return ALLOW;}
<INITIAL>{C}{A}{C}{H}{E}[-]{C}{O}{N}{T}{R}{O}{L}{COLON}     {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return C_CONTROL;}
<INITIAL>{C}{O}{N}{N}{E}{C}{T}{I}{O}{N}{COLON}              {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return CONNECTION;}
<INITIAL>{D}{A}{T}{E}{COLON}                                {chr_pos+=ICAP_parse_leng;BEGIN(SP_STR);return DATE_TOKEN;}
<INITIAL>{E}{N}{C}{A}{P}{S}{U}{L}{A}{T}{E}{D}{COLON}        {chr_pos+=ICAP_parse_leng;BEGIN(ENCAP);return ENCAPSULATED;}
<INITIAL>{E}{X}{P}{I}{R}{E}{S}{COLON}                       {chr_pos+=ICAP_parse_leng;BEGIN(SP_STR);return EXPIRES;}
<INITIAL>{F}{R}{O}{M}{COLON}                                {chr_pos+=ICAP_parse_leng;BEGIN(MAIL);return FROM;}
<INITIAL>{H}{O}{S}{T}{COLON}                                {chr_pos+=ICAP_parse_leng;BEGIN(URI);return HOST;}
<INITIAL>{I}{S}{T}{A}{G}{COLON}                             {chr_pos+=ICAP_parse_leng;BEGIN(ISTAG);return ISTAG_TOKEN;}
<INITIAL>{M}{A}{X}[-]{C}{O}{N}{N}{E}{C}{T}{I}{O}{N}{S}{COLON}       {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return MAX_CONN;}
<INITIAL>{M}{E}{T}{H}{O}{D}{S}{COLON}                       {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return METHODS;}
<INITIAL>{O}{P}{T}[-]{B}{O}{D}{Y}[-]{T}{Y}{P}{E}{COLON}     {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return OPT_BTYPE;}
<INITIAL>{O}{P}{T}{I}{O}{N}{S}[-]{T}{T}{L}{COLON}           {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return TTL;}
<INITIAL>{P}{R}{A}{G}{M}{A}{COLON}                          {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return PRAGMA;}
<INITIAL>{P}{R}{E}{V}{I}{E}{W}{COLON}                       {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return PREVIEW;}
<INITIAL>{R}{E}{F}{E}{R}{E}{R}{COLON}                       {chr_pos+=ICAP_parse_leng;BEGIN(URI);return REFERER;}
<INITIAL>{S}{E}{R}{V}{E}{R}{COLON}                          {chr_pos+=ICAP_parse_leng;BEGIN(TOKENS);return SERVER;}
<INITIAL>{S}{E}{R}{V}{I}{C}{E}{COLON}                       {chr_pos+=ICAP_parse_leng;BEGIN(SP_STR);return SERVICE;}
<INITIAL>{S}{E}{R}{V}{I}{C}{E}[-]{I}{D}{COLON}              {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return SERVICE_ID;}
<INITIAL>{T}{R}{A}{I}{L}{E}{R}{COLON}                       {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return TRAILER;}
<INITIAL>{T}{R}{A}{N}{S}{F}{E}{R}[-]{P}{R}{E}{V}{I}{E}{W}{COLON}    {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return TR_PRE;}
<INITIAL>{T}{R}{A}{N}{S}{F}{E}{R}[-]{I}{G}{N}{O}{R}{E}{COLON}       {chr_pos+=ICAP_parse_leng;BEGIN(GENHEADER);return TR_IG;}
<INITIAL>{T}{R}{A}{N}{S}{F}{E}{R}[-]{C}{O}{M}{P}{L}{E}{T}{E}{COLON} {chr_pos+=ICAP_parse_leng;BEGIN(TOKENS);return TR_COMP;}
<INITIAL>{U}{P}{G}{R}{A}{D}{E}{COLON}                       {chr_pos+=ICAP_parse_leng;BEGIN(TOKENS);return UPGRADE;}
<INITIAL>{U}{S}{E}{R}[-]{A}{G}{E}{N}{T}{COLON}              {chr_pos+=ICAP_parse_leng;BEGIN(TOKENS);return USER_A;}

<INITIAL>{CRLF}                                             {chr_pos+=ICAP_parse_leng;BEGIN(HTTP_HEADER);return CRLF;}
<INITIAL>([a-zA-z]+){TOKEN}{COLON}                          {chr_pos+=ICAP_parse_leng;BEGIN(OHEADER);ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng-1,ICAP_parse_text);return H_OTHER;}
<OHEADER>[^ \r\n][^\r\n]*                                   {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return V_OTHER;}
<OHEADER>{CRLF}                                             {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}



<SP_STR>" "                                 {chr_pos+=ICAP_parse_leng;BEGIN(SP_STR2);return SP;}
<SP_STR2>{TOKEN_SP}                         {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return CHR_TOKEN;}
<SP_STR2>{CRLF}                             {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}


<ENCAP>{R}{E}{Q}[-]{H}{D}{R}[=]             {chr_pos+=ICAP_parse_leng;return REQ_HDR;}
<ENCAP>{R}{E}{S}[-]{H}{D}{R}[=]             {chr_pos+=ICAP_parse_leng;return RES_HDR;}
<ENCAP>{R}{E}{S}[-]{B}{O}{D}{Y}[=]          {chr_pos+=ICAP_parse_leng;return RES_BODY;}
<ENCAP>{R}{E}{Q}[-]{B}{O}{D}{Y}[=]          {chr_pos+=ICAP_parse_leng;return REQ_BODY;}
<ENCAP>{N}{U}{L}{L}[-]{B}{O}{D}{Y}[=]       {chr_pos+=ICAP_parse_leng;return NULL_BODY;}
<ENCAP>{O}{P}{T}[-]{B}{O}{D}{Y}[=]          {chr_pos+=ICAP_parse_leng;return OPT_BODY;}
<ENCAP>{DIGIT}+                             {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.integer = new INTEGER(ICAP_parse_text);return INT_TOKEN;}
<ENCAP>", "                                 {chr_pos+=ICAP_parse_leng;return COMMA_SP;}
<ENCAP>{CRLF}                               {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}


<MAIL>{MAIL}                                {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return CHR_TOKEN;}
<MAIL>{CRLF}                                {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}


<URI>{URI}                                  {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return CHR_TOKEN;}
<URI>{CRLF}                                 {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}


<TOKENS>{PRODUCT}                           {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return CHR_TOKEN;}
<TOKENS>", "                                {chr_pos+=ICAP_parse_leng;product_count++;return COMMA_SP;}
<TOKENS>{CRLF}                              {chr_pos+=ICAP_parse_leng;product_count=0;BEGIN(INITIAL);return CRLF;}


<ISTAG>[\"]{TOKEN}[\"]                      {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return CHR_TOKEN;}
<ISTAG>{CRLF}                               {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}


<GENHEADER>{DIGIT}+                         {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.integer = new INTEGER(ICAP_parse_text);return INT_TOKEN;}
<GENHEADER>{TOKEN}                          {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return CHR_TOKEN;}
<GENHEADER>{CRLF}                           {chr_pos+=ICAP_parse_leng;BEGIN(INITIAL);return CRLF;}

<HTTP_HEADER>((.+){CRLF})*{CRLF}*"0; ieof"{CRLF}{CRLF}    {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return HTTP_BODY_TOKEN;}
<HTTP_HEADER>((.+){CRLF})*{CRLF}*"0"{CRLF}{CRLF}          {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return HTTP_BODY_TOKEN;}
<HTTP_HEADER>((.+){CRLF})*{CRLF}                          {chr_pos+=ICAP_parse_leng;ICAP_parse_lval.charstring = new CHARSTRING(ICAP_parse_leng,ICAP_parse_text);return CHR_TOKEN;}

<*>" "                                      {chr_pos+=ICAP_parse_leng;return SP;}
%%

void Initialize(){
  chr_pos=0;
  error_occured=0;
  product_count=0;
  ICAP_parse_debug=0;
  isRequest = false;
  encapsulated_parsed = new ICAP__Types::Encapsulated(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE);
  
  RequestHeaders_parsed = new ICAP__Types::RequestHeaders(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, 
                                                          *encapsulated_parsed, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, 
                                                          OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
                                                          OMIT_VALUE);
                                                         
  ResponseHeaders_parsed = new ICAP__Types::ResponseHeaders(OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, *encapsulated_parsed, 
                                                            OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, 
                                                            OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
                                                            OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE, OMIT_VALUE,
                                                            OMIT_VALUE, OMIT_VALUE);

};


PDU__ICAP ICAP__Types::f__ICAP__Dec__binary(const OCTETSTRING& pl__stream)
{		
  PDU__ICAP retv;
  ICAP_parse_parsed_message = &retv;

  Initialize();
  
  ICAP_parse__scan_bytes ((const char*)(const unsigned char*)pl__stream, pl__stream.lengthof());      
  ICAP_parse_parse();
  ICAP_parse_lex_destroy();
  
  delete RequestHeaders_parsed;
  delete ResponseHeaders_parsed;
  delete encapsulated_parsed;

  if(error_occured){    
    retv.raw()=(const char*)(const unsigned char*)pl__stream;
  };
  return retv;
}
