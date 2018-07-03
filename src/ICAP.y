%{
/******************************************************************************
* Copyright (c) 2005, 2015  Ericsson AB
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
//  File:               ICAP.y
//  Rev:                R2A
//  Prodnr:             CNL 113 779 
//  Reference:          RFC3507, RFC 2616 
/*C declarations*/

#include "ICAP_parse.h"
#include <stdarg.h>
#include <string.h>
#define YYDEBUG 1
using namespace ICAP__Types;
using namespace HTTPmsg__Types;


extern int ICAP_parse_lex();
extern int ICAP_parse_error(const char *s);
extern int error_occured;
extern int product_count;
extern bool isRequest;
bool isUnbound;

PDU__ICAP*       ICAP_parse_parsed_message;
RequestHeaders*  RequestHeaders_parsed;
ResponseHeaders* ResponseHeaders_parsed;
Encapsulated*    encapsulated_parsed;

//Inserts the given value into the Content-Length header
HTTPMessage f_insert_length_value(HTTPMessage& msg, CHARSTRING& value){
  HeaderLines headers;
  CHARSTRING contentLength="Content-Length";
  if(msg.ischosen(HTTPMessage::ALT_request)) headers = msg.request().header();
  if(msg.ischosen(HTTPMessage::ALT_response)) headers = msg.response().header();
  
  for(int i=0; i<headers.size_of();i++){
      if(headers[i].header__name()==contentLength)headers[i].header__value()=value;
  };
  
  if(msg.ischosen(HTTPMessage::ALT_request)) msg.request().header()=headers;
  if(msg.ischosen(HTTPMessage::ALT_response)) msg.response().header()=headers;
  return msg; 
};


//Decodes the HTTP headers with changing Content-Length to 0 and then back to the original value
HTTPMessage f_decode_HTTPHeaders(CHARSTRING& pl__msg){
  const char* msg = pl__msg;
  int retv;
  CHARSTRING contentLength="Content-Length: ";
  CHARSTRING rn="\r\n";
  CHARSTRING length;
  HTTPMessage http;
  
  /*Search for Content-Length field*/
  const char* pt = strstr(msg,contentLength);
  if(pt == NULL){
      retv = dec__HTTPMessage(char2oct(pl__msg),http,false);
      if(retv<0)TTCN_warning("Unable to decode HTTP header! %s",(const char*)pl__msg); 
      return http;
  };
  
  /*Seasrch for \r\n after the Content-Length*/
  pt+=16;
  const char* pt2 = strstr(pt,rn);
  if(pt2 == NULL){
      retv = dec__HTTPMessage(char2oct(pl__msg),http,false);
      if(retv<0)TTCN_warning("Unable to decode HTTP header! %s",(const char*)pl__msg); 
      return http;
  };
  
  /*modifying the pl__msg*/
  length = CHARSTRING((pt2-msg)-(pt-msg),pt);
  retv = dec__HTTPMessage(char2oct(CHARSTRING(pt-msg,msg)+CHARSTRING(1,"0")+CHARSTRING(pl__msg.lengthof()-(pt2-msg),pt2)),http,false);
  if(retv<0){TTCN_warning("Unable to decode HTTP header! %s",(const char*)pl__msg);}
  else{
      http = f_insert_length_value(http,length);
  };
  return http;
}

//Decodes the HTTP body part
HTTP__Body f_decode_HTTPBody(CHARSTRING& pl__msg){
  HTTP__Body body=OCTETSTRING(0,(const unsigned char*)"");
  const char* ptr = pl__msg;
  const char* ptr2 = pl__msg;
  char* temp;
  CHARSTRING rn="\r\n";
  int len;
  bool reading=true;
  
  while(reading){
    //getting length of body chunk
    ptr2 = strstr(ptr,rn);
    len=strtol(ptr,&temp,16);
    ptr=ptr2+2;
    
    if(len!=0){
      //getting body chunk
      body=body+OCTETSTRING(len,(const unsigned char*)ptr);
      ptr+=(len+2);
    } else reading=false;
  };
  
  return body;
}

%}
%union {
        CHARSTRING* charstring;
        INTEGER*    integer;
	ICAP__Types::RequestLine*         requestline;
        ICAP__Types::StatusLine*          statusline;
        ICAP__Types::RequestHeaders*      requestheaders;
        ICAP__Types::ResponseHeaders*     responseheaders;
        ICAP__Types::Method*              method;
        ICAP__Types::Uri*                 uri;
        ICAP__Types::Query*               query;
        ICAP__Types::Encapsulated*        encapsulated;
        ICAP__Types::Upgrade*             upgrade;
        ICAP__Types::User__Agent*         user_agent;
        ICAP__Types::Server*              server;
        ICAP__Types::Transfer__Complete*  transfer_complete;
        ICAP__Types::HTTPMessage__List*   httpmsg_list;
        ICAP__Types::HTTP__Body*          httpbody;
}

%destructor { delete $$;} <*>

%token <charstring> METHOD SCHEME NET_PATH QUERY_TOKEN VERSION_TOKEN
%token <charstring> REASON CHR_TOKEN H_OTHER V_OTHER HTTP_BODY_TOKEN
%token <integer> INT_TOKEN
%token SP DSLASH QM CRLF COMMA_SP REQ_HDR RES_HDR RES_BODY REQ_BODY NULL_BODY OPT_BODY
%token AUTHORIZATION ALLOW C_CONTROL CONNECTION DATE_TOKEN ENCAPSULATED EXPIRES FROM
%token HOST ISTAG_TOKEN MAX_CONN METHODS OPT_BTYPE TTL PRAGMA PREVIEW REFERER TRAILER
%token UPGRADE USER_A SERVER SERVICE SERVICE_ID TR_PRE TR_IG TR_COMP HEX_LEN
%token EOF_BODY

%type <requestline>       RequestLine
%type <statusline>        StatusLine
%type <requestheaders>    RequestHeaders
%type <responseheaders>   ResponseHeaders
%type <method>            Method
%type <uri>               Uri
%type <query>             Query
%type <encapsulated>      Encapsulated
%type <upgrade>           Upgrade
%type <user_agent>        User_Agent
%type <server>            Server
%type <transfer_complete> Transfer_Complete
%type <httpmsg_list>      HTTPMessage_List
%type <httpbody>          HTTP_Body
%start icap_message 

%%
icap_message: request {YYACCEPT;}
             |response{YYACCEPT;}

request: RequestLine RequestHeaders HTTPMessage_List HTTP_Body{
        ICAP_parse_parsed_message->request().requestline()= *$1;
        ICAP_parse_parsed_message->request().headers()= *$2;
        if(isUnbound)ICAP_parse_parsed_message->request().http__headers()=OMIT_VALUE;
        else ICAP_parse_parsed_message->request().http__headers()()=*$3;
        if($4)ICAP_parse_parsed_message->request().http__body()()=*$4;
        else ICAP_parse_parsed_message->request().http__body()=OMIT_VALUE;
        delete $1;
        delete $2;
        delete $3;
        delete $4;
}

response: StatusLine ResponseHeaders HTTPMessage_List HTTP_Body{
        ICAP_parse_parsed_message->response().statusline()= *$1;
        ICAP_parse_parsed_message->response().headers()= *$2;
        if(isUnbound)ICAP_parse_parsed_message->response().http__headers()=OMIT_VALUE;
        else ICAP_parse_parsed_message->response().http__headers()()=*$3;
        if($4)ICAP_parse_parsed_message->response().http__body()()=*$4;
        else ICAP_parse_parsed_message->response().http__body()=OMIT_VALUE;
        delete $1;
        delete $2;
        delete $3;
        delete $4;
}
             
RequestLine: Method Uri SP VERSION_TOKEN CRLF{
        $$=new RequestLine(*$1, *$2, *$4);
        delete $1;
        delete $2;
        delete $4;
}

StatusLine: VERSION_TOKEN SP INT_TOKEN SP REASON CRLF{
        $$=new StatusLine(*$1, *$3, *$5);
        delete $1;
        delete $3;
        delete $5;
}


HTTPMessage_List: 
        /* empty */{
        $$ = new HTTPMessage__List;
        isUnbound = true;
      } 
      | HTTPMessage_List CHR_TOKEN{
        int a=0;
        CHARSTRING value;
        if(isUnbound){
          (*$$)[0] = f_decode_HTTPHeaders(*$2);
          delete $2;
          isUnbound = false;
        }
        else{
          a=$1->size_of();
          $$=$1;
          (*$$)[a] = f_decode_HTTPHeaders(*$2);
          delete $2;
        }; 
      }
      
HTTP_Body: /*empty*/{$$ = NULL;}
        |  HTTP_BODY_TOKEN{ 
          $$ = new HTTP__Body(f_decode_HTTPBody(*$1)); 
          delete $1;
        }
        
        
        
Method: METHOD {
          $$=new Method(Method::str_to_enum((const char*)(*$1)));
          delete $1;
}

Uri: SP SCHEME DSLASH NET_PATH Query{
        if($5){
          $$=new Uri(*$2, *$4, *$5);
          delete $2;
          delete $4;
          delete $5;
        } else {
          $$=new Uri(*$2, *$4, OMIT_VALUE);
          delete $2;
          delete $4;
        }
}

Query: /* empty */{$$=NULL;} 
      |QM QUERY_TOKEN{
        $$=new Query(*$2);
        delete $2;
      }
      
RequestHeaders: RequestHeader_list CRLF{
        $$ = new RequestHeaders(*RequestHeaders_parsed);
}

RequestHeader_list: Header_list{ /* empty */ }
           | RequestHeader_list Header_list { /* empty */ }
 

           
ResponseHeaders: ResponseHeader_list CRLF{
        $$ = new ResponseHeaders(*ResponseHeaders_parsed);
}

ResponseHeader_list: Header_list{ /* empty */ }
           | ResponseHeader_list Header_list { /* empty */ }           


Header_list:
       AUTHORIZATION SP CHR_TOKEN CRLF{
        RequestHeaders_parsed->authorization()()=*$3;
        delete $3;
      }
      |ALLOW SP INT_TOKEN CRLF{
        if(isRequest)RequestHeaders_parsed->allow()()=*$3;
        else ResponseHeaders_parsed->allow()()=*$3;
        delete $3;
      }
      |C_CONTROL SP CHR_TOKEN CRLF{
        if(isRequest)RequestHeaders_parsed->cache__control()()=*$3;
        else ResponseHeaders_parsed->cache__control()()=*$3;
        delete $3;
      }
      |CONNECTION SP CHR_TOKEN CRLF{
        if(isRequest)RequestHeaders_parsed->connection()()=*$3;
        else ResponseHeaders_parsed->connection()()=*$3;
        delete $3;
      }
      |DATE_TOKEN SP CHR_TOKEN CRLF{
        if(isRequest)RequestHeaders_parsed->date()()=*$3;
        else ResponseHeaders_parsed->date()()=*$3;
        delete $3;
      }
      |ENCAPSULATED SP Encapsulated CRLF{
        if(isRequest)RequestHeaders_parsed->encapsulated()=*$3;
        else ResponseHeaders_parsed->encapsulated()=*$3;
        delete $3;
      }
      |EXPIRES SP CHR_TOKEN CRLF{
        if(isRequest)RequestHeaders_parsed->expires()()=*$3;
        else ResponseHeaders_parsed->expires()()=*$3;
        delete $3;
      }
      |FROM SP CHR_TOKEN CRLF{
        RequestHeaders_parsed->fRom()()=*$3;
        delete $3;
      }
      |HOST SP CHR_TOKEN CRLF{
        RequestHeaders_parsed->host()()=*$3;
        delete $3;
      }
      |ISTAG_TOKEN SP CHR_TOKEN CRLF{
        ResponseHeaders_parsed->istag()()=*$3;
        delete $3;
      }
      |MAX_CONN SP INT_TOKEN CRLF{
        ResponseHeaders_parsed->max__connections()()=*$3;
        delete $3;
      }
      |METHODS SP CHR_TOKEN CRLF{
        ResponseHeaders_parsed->methods()()=*$3;
        delete $3;
      }
      |OPT_BTYPE SP CHR_TOKEN CRLF{
        ResponseHeaders_parsed->opt__body__type()()=*$3;
        delete $3;
      }
      |TTL SP INT_TOKEN CRLF{
        ResponseHeaders_parsed->options__ttl()()=*$3;
        delete $3;
      }
      |PRAGMA SP CHR_TOKEN CRLF{
        if(isRequest)RequestHeaders_parsed->pragma()()=*$3;
        else ResponseHeaders_parsed->pragma()()=*$3;
        delete $3;
      }
      |PREVIEW SP INT_TOKEN CRLF{
        if(isRequest)RequestHeaders_parsed->preview()()=*$3;
        else ResponseHeaders_parsed->preview()()=*$3;
        delete $3;
      }
      |REFERER SP CHR_TOKEN CRLF{
        RequestHeaders_parsed->referer()()=*$3;
        delete $3;
      }
      |SERVER SP Server CRLF{
        ResponseHeaders_parsed->server()()=*$3;
        delete $3;
      }
      |SERVICE SP CHR_TOKEN CRLF{
        ResponseHeaders_parsed->service()()=*$3;
        delete $3;
      }
      |SERVICE_ID SP CHR_TOKEN CRLF{
        ResponseHeaders_parsed->service__id()()=*$3;
        delete $3;
      }
      |TRAILER SP CHR_TOKEN CRLF{
        if(isRequest)RequestHeaders_parsed->trailer()()=*$3;
        else ResponseHeaders_parsed->trailer()()=*$3;
        delete $3;
      }
      |TR_PRE SP CHR_TOKEN CRLF{
        ResponseHeaders_parsed->transfer__preview()()=*$3;
        delete $3;
      }
      |TR_IG SP CHR_TOKEN CRLF{
        ResponseHeaders_parsed->transfer__ignore()()=*$3;
        delete $3;
      }
      |TR_COMP SP Transfer_Complete CRLF{
        ResponseHeaders_parsed->transfer__complete()()=*$3;
        delete $3;
      }
      |UPGRADE SP Upgrade CRLF{
        if(isRequest)RequestHeaders_parsed->upgrade()()=*$3;
        else ResponseHeaders_parsed->upgrade()()=*$3;
        delete $3;
      }
      |USER_A SP User_Agent CRLF{
        RequestHeaders_parsed->user__agent()()=*$3;
        delete $3;
      }
      |H_OTHER SP V_OTHER CRLF {
        int a=0;
        if(isRequest){
          if(RequestHeaders_parsed->undefinedHeader__List().ispresent()){
            a=RequestHeaders_parsed->undefinedHeader__List()().size_of();
          }                         
          RequestHeaders_parsed->undefinedHeader__List()()[a].headerName()=*$1;
          RequestHeaders_parsed->undefinedHeader__List()()[a].headerValue()=*$3;
        } else {
          if(ResponseHeaders_parsed->undefinedHeader__List().ispresent()){
            a=ResponseHeaders_parsed->undefinedHeader__List()().size_of();
          }                         
          ResponseHeaders_parsed->undefinedHeader__List()()[a].headerName()=*$1;
          ResponseHeaders_parsed->undefinedHeader__List()()[a].headerValue()=*$3;
        }
        delete $1;
        delete $3;
      }

      
Encapsulated: Encapsulated_list{
        $$ = new Encapsulated(*encapsulated_parsed);       
}

Encapsulated_list: Params{ /* empty */ } 
                  |Encapsulated_list Params{ /* empty */ }

Params: 
      REQ_HDR INT_TOKEN{
        encapsulated_parsed->req__hdr()()=*$2;
        delete $2;
      }
      | COMMA_SP REQ_HDR INT_TOKEN{
        encapsulated_parsed->req__hdr()()=*$3;
        delete $3;
      }
      | RES_HDR INT_TOKEN{
        encapsulated_parsed->res__hdr()()=*$2;
        delete $2;
      }
      | COMMA_SP RES_HDR INT_TOKEN{
        encapsulated_parsed->res__hdr()()=*$3;
        delete $3;
      }
      | RES_BODY INT_TOKEN{
        encapsulated_parsed->res__body()()=*$2;
        delete $2;
      }
      | COMMA_SP RES_BODY INT_TOKEN{
        encapsulated_parsed->res__body()()=*$3;
        delete $3;
      }
      | REQ_BODY INT_TOKEN{
        encapsulated_parsed->req__body()()=*$2;
        delete $2;
      }
      | COMMA_SP REQ_BODY INT_TOKEN{
        encapsulated_parsed->req__body()()=*$3;
        delete $3;
      }
      | NULL_BODY INT_TOKEN{
        encapsulated_parsed->null__body()()=*$2;
        delete $2;
      }
      | COMMA_SP NULL_BODY INT_TOKEN{
        encapsulated_parsed->null__body()()=*$3;
        delete $3;
      }
      | OPT_BODY INT_TOKEN{
        encapsulated_parsed->opt__body()()=*$2;
        delete $2;
      }
      | COMMA_SP OPT_BODY INT_TOKEN{
        encapsulated_parsed->opt__body()()=*$3;
        delete $3;
      }
     
Server:
      CHR_TOKEN{
        $$ = new Server;
        (*$$)[0] = *$1;
        delete $1;
      }
      | Server SP CHR_TOKEN {
        int a=$1->size_of();
        $$=$1;
        (*$$)[a] = *$3;
        delete $3;
      }
      
Transfer_Complete:
      CHR_TOKEN{
        $$ = new Transfer__Complete;
        (*$$)[0] = *$1;
        delete $1;
      }
      | Transfer_Complete COMMA_SP CHR_TOKEN {
        int a=$1->size_of();
        $$=$1;
        (*$$)[a] = *$3;
        delete $3;
      }
     
Upgrade: 
      CHR_TOKEN{
        $$ = new Upgrade;
        (*$$)[0] = *$1;
        delete $1;
      }
      | Upgrade COMMA_SP CHR_TOKEN {
        int a=$1->size_of();
        $$=$1;
        (*$$)[a] = *$3;
        delete $3;
      }
      
User_Agent: 
      CHR_TOKEN{
        $$ = new User__Agent;
        (*$$)[0] = *$1;
        delete $1;
      }
      | User_Agent SP CHR_TOKEN {
        int a=$1->size_of();
        $$=$1;
        (*$$)[a] = *$3;
        delete $3;
      } 
         
%%

/*Additional C code*/
