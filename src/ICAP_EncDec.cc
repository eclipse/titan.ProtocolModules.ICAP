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
//  File:               ICAP_EncDec.cc
//  Rev:                R2A
//  Prodnr:             CNL 113 779 
//  Reference:          RFC3507, RFC 2616     
 
#include "ICAP_Types.hh"
#include "memory.h"
#include <string.h>

using namespace HTTPmsg__Types;

namespace ICAP__Types {

//Encoding the Request Headers
void f_encode_RequestHeaders(const ICAP__Types::RequestHeaders& headers, TTCN_Buffer& buff){
  if(headers.authorization().ispresent()){
     buff.put_s(15,(const unsigned char*)"Authorization: ");
     buff.put_string(headers.authorization());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.allow().ispresent()){
     buff.put_s(7,(const unsigned char*)"Allow: ");
     buff.put_string(int2str(headers.allow()()));
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.cache__control().ispresent()){
     buff.put_s(15,(const unsigned char*)"Cache-Control: ");
     buff.put_string(headers.cache__control());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.connection().ispresent()){
     buff.put_s(12,(const unsigned char*)"Connection: ");
     buff.put_string(headers.connection());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.date().ispresent()){
     buff.put_s(6,(const unsigned char*)"Date: ");
     buff.put_string(headers.date());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.expires().ispresent()){
     buff.put_s(9,(const unsigned char*)"Expires: ");
     buff.put_string(headers.expires());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.fRom().ispresent()){
     buff.put_s(6,(const unsigned char*)"From: ");
     buff.put_string(headers.fRom());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.host().ispresent()){
     buff.put_s(6,(const unsigned char*)"Host: ");
     buff.put_string(headers.host());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.pragma().ispresent()){
     buff.put_s(8,(const unsigned char*)"Pragma: ");
     buff.put_string(headers.pragma());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.preview().ispresent()){
     buff.put_s(9,(const unsigned char*)"Preview: ");
     buff.put_string(int2str(headers.preview()()));
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.referer().ispresent()){
     buff.put_s(9,(const unsigned char*)"Referer: ");
     buff.put_string(headers.referer()());
     buff.put_s(2,(const unsigned char*)"\r\n");
  };
  
  if(headers.trailer().ispresent()){
     buff.put_s(9,(const unsigned char*)"Trailer: ");
     buff.put_string(headers.trailer()());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.upgrade().ispresent()){
     buff.put_s(9,(const unsigned char*)"Upgrade: ");
     const Upgrade* ptr = &headers.upgrade()();
     int a = ptr->size_of();
     for(int i=0;i<a;i++){
        if(i>0)buff.put_s(2,(const unsigned char*)", ");
        buff.put_string(headers.upgrade()()[i]);
     }
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.user__agent().ispresent()){
     buff.put_s(12,(const unsigned char*)"User-Agent: ");
     const User__Agent* ptr = &headers.user__agent()();
     int a = ptr->size_of();
     for(int i=0;i<a;i++){
        if(i>0)buff.put_s(1,(const unsigned char*)" ");
        buff.put_string(headers.user__agent()()[i]);
     }
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  //Encapsulated header SHOULD be near to the body and MUST  be present
  buff.put_s(14,(const unsigned char*)"Encapsulated: ");
  bool need_comma = false;
  if(headers.encapsulated().req__hdr().ispresent() && headers.encapsulated().res__hdr().ispresent()){
    int req = headers.encapsulated().req__hdr()();
    int res = headers.encapsulated().res__hdr()();
    //the values must be increasing
    if(req < res){
      buff.put_s(8,(const unsigned char*)"req-hdr=");
      buff.put_string(int2str(headers.encapsulated().req__hdr()()));
      
      buff.put_s(2,(const unsigned char*)", ");
      buff.put_s(8,(const unsigned char*)"res-hdr=");
      buff.put_string(int2str(headers.encapsulated().res__hdr()()));
      need_comma = true;
    } else {
      buff.put_s(8,(const unsigned char*)"res-hdr=");
      buff.put_string(int2str(headers.encapsulated().res__hdr()()));
      
      buff.put_s(2,(const unsigned char*)", ");
      buff.put_s(8,(const unsigned char*)"req-hdr=");
      buff.put_string(int2str(headers.encapsulated().req__hdr()()));
      need_comma = true;
    };
  } else {
    if(headers.encapsulated().req__hdr().ispresent()){
      buff.put_s(8,(const unsigned char*)"req-hdr=");
      buff.put_string(int2str(headers.encapsulated().req__hdr()()));
      need_comma = true;
    };
    if(headers.encapsulated().res__hdr().ispresent()){
      if(need_comma)buff.put_s(2,(const unsigned char*)", ");
      buff.put_s(8,(const unsigned char*)"res-hdr=");
      buff.put_string(int2str(headers.encapsulated().res__hdr()()));
      need_comma = true;
    };
  }; 
  if(headers.encapsulated().res__body().ispresent()){
     if(need_comma)buff.put_s(2,(const unsigned char*)", ");
     buff.put_s(9,(const unsigned char*)"res-body=");
     buff.put_string(int2str(headers.encapsulated().res__body()()));
     need_comma = true;
  };
  if(headers.encapsulated().req__body().ispresent()){
     if(need_comma)buff.put_s(2,(const unsigned char*)", ");
     buff.put_s(9,(const unsigned char*)"req-body=");
     buff.put_string(int2str(headers.encapsulated().req__body()()));
     need_comma = true;
  };
  if(headers.encapsulated().null__body().ispresent()){
     if(need_comma)buff.put_s(2,(const unsigned char*)", ");
     buff.put_s(10,(const unsigned char*)"null-body=");
     buff.put_string(int2str(headers.encapsulated().null__body()()));
     need_comma = true;
  };
  if(headers.encapsulated().opt__body().ispresent()){
     if(need_comma)buff.put_s(2,(const unsigned char*)", ");
     buff.put_s(9,(const unsigned char*)"opt-body=");
     buff.put_string(int2str(headers.encapsulated().opt__body()()));
//     need_comma = true;
  };
  buff.put_s(2,(const unsigned char*)"\r\n");      
  
  if(headers.undefinedHeader__List().ispresent()){
     const UndefinedHeader__List* ptr = &headers.undefinedHeader__List()();
     int a = ptr->size_of();
     for(int i=0;i<a;i++){
        buff.put_string(headers.undefinedHeader__List()()[i].headerName());
        buff.put_s(2,(const unsigned char*)": ");
        buff.put_string(headers.undefinedHeader__List()()[i].headerValue());
        buff.put_s(2,(const unsigned char*)"\r\n");
     }    
  };
};

//Encoding the Response Headers
void f_encode_ResponseHeaders(const ICAP__Types::ResponseHeaders& headers, TTCN_Buffer& buff){
  if(headers.allow().ispresent()){
     buff.put_s(7,(const unsigned char*)"Allow: ");
     buff.put_string(int2str(headers.allow()()));
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.cache__control().ispresent()){
     buff.put_s(15,(const unsigned char*)"Cache-Control: ");
     buff.put_string(headers.cache__control());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.connection().ispresent()){
     buff.put_s(12,(const unsigned char*)"Connection: ");
     buff.put_string(headers.connection());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.date().ispresent()){
     buff.put_s(6,(const unsigned char*)"Date: ");
     buff.put_string(headers.date());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.expires().ispresent()){
     buff.put_s(9,(const unsigned char*)"Expires: ");
     buff.put_string(headers.expires());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.istag().ispresent()){
     buff.put_s(7,(const unsigned char*)"ISTag: ");
     buff.put_string(headers.istag());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };

  if(headers.max__connections().ispresent()){
     buff.put_s(17,(const unsigned char*)"Max-Connections: ");
     buff.put_string(int2str(headers.max__connections()()));
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.methods().ispresent()){
     buff.put_s(9,(const unsigned char*)"Methods: ");
     buff.put_string(headers.methods());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.opt__body__type().ispresent()){
     buff.put_s(15,(const unsigned char*)"Opt-body-type: ");
     buff.put_string(headers.opt__body__type());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.options__ttl().ispresent()){
     buff.put_s(13,(const unsigned char*)"Options-TTL: ");
     buff.put_string(int2str(headers.options__ttl()()));
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.pragma().ispresent()){
     buff.put_s(8,(const unsigned char*)"Pragma: ");
     buff.put_string(headers.pragma());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.preview().ispresent()){
     buff.put_s(9,(const unsigned char*)"Preview: ");
     buff.put_string(int2str(headers.preview()()));
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.server().ispresent()){
     buff.put_s(8,(const unsigned char*)"Server: ");
     const Server* ptr = &headers.server()();
     int a = ptr->size_of();
     for(int i=0;i<a;i++){
        if(i>0)buff.put_s(1,(const unsigned char*)" ");
        buff.put_string(headers.server()()[i]);
     }
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.service().ispresent()){
     buff.put_s(9,(const unsigned char*)"Service: ");
     buff.put_string(headers.service()());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.service__id().ispresent()){
     buff.put_s(12,(const unsigned char*)"Service-ID: ");
     buff.put_string(headers.service__id()());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.trailer().ispresent()){
     buff.put_s(9,(const unsigned char*)"Trailer: ");
     buff.put_string(headers.trailer()());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.transfer__preview().ispresent()){
     buff.put_s(18,(const unsigned char*)"Transfer-Preview: ");
     buff.put_string(headers.transfer__preview()());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.transfer__ignore().ispresent()){
     buff.put_s(17,(const unsigned char*)"Transfer-Ignore: ");
     buff.put_string(headers.transfer__ignore()());
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.transfer__complete().ispresent()){
     buff.put_s(19,(const unsigned char*)"Transfer-Complete: ");
     const Transfer__Complete* ptr = &headers.transfer__complete()();
     int a = ptr->size_of();
     for(int i=0;i<a;i++){
        if(i>0)buff.put_s(2,(const unsigned char*)", ");
        buff.put_string(headers.transfer__complete()()[i]);
     }
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  if(headers.upgrade().ispresent()){
     buff.put_s(9,(const unsigned char*)"Upgrade: ");
     const Upgrade* ptr = &headers.upgrade()();
     int a = ptr->size_of();
     for(int i=0;i<a;i++){
        if(i>0)buff.put_s(2,(const unsigned char*)", ");
        buff.put_string(headers.upgrade()()[i]);
     }
     buff.put_s(2,(const unsigned char*)"\r\n");      
  };
  
  //Encapsulated header SHOULD be near to the body and MUST  be present
  buff.put_s(14,(const unsigned char*)"Encapsulated: ");
  bool need_comma = false;
  if(headers.encapsulated().req__hdr().ispresent() && headers.encapsulated().res__hdr().ispresent()){
    int req = headers.encapsulated().req__hdr()();
    int res = headers.encapsulated().res__hdr()();
    //the values must be increasing
    if(req < res){
      buff.put_s(8,(const unsigned char*)"req-hdr=");
      buff.put_string(int2str(headers.encapsulated().req__hdr()()));
      
      buff.put_s(2,(const unsigned char*)", ");
      buff.put_s(8,(const unsigned char*)"res-hdr=");
      buff.put_string(int2str(headers.encapsulated().res__hdr()()));
      need_comma = true;
    } else {
      buff.put_s(8,(const unsigned char*)"res-hdr=");
      buff.put_string(int2str(headers.encapsulated().res__hdr()()));
      
      buff.put_s(2,(const unsigned char*)", ");
      buff.put_s(8,(const unsigned char*)"req-hdr=");
      buff.put_string(int2str(headers.encapsulated().req__hdr()()));
      need_comma = true;
    };
  } else {
    if(headers.encapsulated().req__hdr().ispresent()){
      buff.put_s(8,(const unsigned char*)"req-hdr=");
      buff.put_string(int2str(headers.encapsulated().req__hdr()()));
      need_comma = true;
    };
    if(headers.encapsulated().res__hdr().ispresent()){
      if(need_comma)buff.put_s(2,(const unsigned char*)", ");
      buff.put_s(8,(const unsigned char*)"res-hdr=");
      buff.put_string(int2str(headers.encapsulated().res__hdr()()));
      need_comma = true;
    };
  }; 
  if(headers.encapsulated().res__body().ispresent()){
     if(need_comma)buff.put_s(2,(const unsigned char*)", ");
     buff.put_s(9,(const unsigned char*)"res-body=");
     buff.put_string(int2str(headers.encapsulated().res__body()()));
     need_comma = true;
  };
  if(headers.encapsulated().req__body().ispresent()){
     if(need_comma)buff.put_s(2,(const unsigned char*)", ");
     buff.put_s(9,(const unsigned char*)"req-body=");
     buff.put_string(int2str(headers.encapsulated().req__body()()));
     need_comma = true;
  };
  if(headers.encapsulated().null__body().ispresent()){
     if(need_comma)buff.put_s(2,(const unsigned char*)", ");
     buff.put_s(10,(const unsigned char*)"null-body=");
     buff.put_string(int2str(headers.encapsulated().null__body()()));
     need_comma = true;
  };
  if(headers.encapsulated().opt__body().ispresent()){
     if(need_comma)buff.put_s(2,(const unsigned char*)", ");
     buff.put_s(9,(const unsigned char*)"opt-body=");
     buff.put_string(int2str(headers.encapsulated().opt__body()()));
//     need_comma = true;
  };
  buff.put_s(2,(const unsigned char*)"\r\n");      
  
  if(headers.undefinedHeader__List().ispresent()){
     const UndefinedHeader__List* ptr = &headers.undefinedHeader__List()();
     int a = ptr->size_of();
     for(int i=0;i<a;i++){
        buff.put_string(headers.undefinedHeader__List()()[i].headerName());
        buff.put_s(2,(const unsigned char*)": ");
        buff.put_string(headers.undefinedHeader__List()()[i].headerValue());
        buff.put_s(2,(const unsigned char*)"\r\n");
     }    
  };
};

OCTETSTRING encoding_HTTP_Header(const HTTPMessage& msg){
	HTTPMessage http = msg;
  //checking the http header's body part
  if(http.ischosen(HTTPMessage::ALT_response)){
    if(http.response().body()!=""){
      TTCN_warning("The http_header field's body part is not empty! It won't be encoded into the ICAP message. To add an HTTP body, use the http_body field in the ICAP_PDU!");
      http.response().body()="";
    };
  };
  if(http.ischosen(HTTPMessage::ALT_request)){
    if(http.request().body()!=""){
      TTCN_warning("The http_header field's body part is not empty! It won't be encoded into the ICAP message. To add an HTTP body, use the http_body field in the ICAP_PDU!");
      http.request().body()="";
    };
  };
  if(http.ischosen(HTTPMessage::ALT_response__binary)){
    if(http.response__binary().body()!=OCTETSTRING(0,(const unsigned char*)"")){
      TTCN_warning("The http_header field's body part is not empty! It won't be encoded into the ICAP message. To add an HTTP body, use the http_body field in the ICAP_PDU!");
      http.response__binary().body()=OCTETSTRING(0,(const unsigned char*)"");
    };
  };
  if(http.ischosen(HTTPMessage::ALT_request__binary)){
    if(http.request__binary().body()!=OCTETSTRING(0,(const unsigned char*)"")){
      TTCN_warning("The http_header field's body part is not empty! It won't be encoded into the ICAP message. To add an HTTP body, use the http_body field in the ICAP_PDU!");
      http.request__binary().body()=OCTETSTRING(0,(const unsigned char*)"");
    };
  };
  
  return enc__HTTPMessage(http);      
};
    
//Basic encoder function
TTCN_Buffer f_encoding(const PDU__ICAP& pl__msg){
  TTCN_Buffer buff;	
  if(pl__msg.ischosen(PDU__ICAP::ALT_raw)){
    buff.put_string(pl__msg.raw());
    return buff;
  };

  if(pl__msg.ischosen(PDU__ICAP::ALT_request)){
    
    //RequestLine
    switch(pl__msg.request().requestline().method()){
        case 0: buff.put_s(7,(const unsigned char*)"REQMOD "); break;
        case 1: buff.put_s(8,(const unsigned char*)"RESPMOD "); break;
        case 2: buff.put_s(8,(const unsigned char*)"OPTIONS "); break;
        default: break;
    };    
    buff.put_string(pl__msg.request().requestline().uri().scheme());
    buff.put_s(3,(const unsigned char*)"://");
    buff.put_string(pl__msg.request().requestline().uri().net__path());
    if(pl__msg.request().requestline().uri().query().ispresent()){
        buff.put_s(1,(const unsigned char*)"?");
        buff.put_string(pl__msg.request().requestline().uri().query()());
    };
    buff.put_s(1,(const unsigned char*)" ");
    buff.put_string(pl__msg.request().requestline().version());
    buff.put_s(2,(const unsigned char*)"\r\n");
    
    //ICAP RequestHeaders
    f_encode_RequestHeaders(pl__msg.request().headers(),buff);
    buff.put_s(2,(const unsigned char*)"\r\n");
    
    //HTTP headres
    if(pl__msg.request().http__headers().ispresent()){
        const HTTPMessage__List* ptr = &pl__msg.request().http__headers()();
        int a = ptr->size_of();
        OCTETSTRING header;
        for(int i=0;i<a;i++){        
          header = encoding_HTTP_Header(pl__msg.request().http__headers()()[i]);
          buff.put_s(header.lengthof(),(const unsigned char*)header);
        };
    };
    
    //HTTP body
    if(pl__msg.request().http__body().ispresent()){
        int len=pl__msg.request().http__body()().lengthof();
        char num[10];
        int n=sprintf(num,"%X",len);
        buff.put_s(n,(const unsigned char*)num);
        buff.put_s(2,(const unsigned char*)"\r\n");
        buff.put_string(pl__msg.request().http__body()());
        buff.put_s(2,(const unsigned char*)"\r\n");
        buff.put_s(5,(const unsigned char*)"0\r\n\r\n");
    };
  };
  
  
  if(pl__msg.ischosen(PDU__ICAP::ALT_response)){
    
    //StatusLine
    buff.put_string(pl__msg.response().statusline().version());  
    buff.put_s(1,(const unsigned char*)" ");
    buff.put_string(int2str(pl__msg.response().statusline().statuscode())); 
    buff.put_s(1,(const unsigned char*)" ");
    buff.put_string(pl__msg.response().statusline().reasonPhrase()); 
    buff.put_s(2,(const unsigned char*)"\r\n");
    
    //ICAP ResponseHeaders
    f_encode_ResponseHeaders(pl__msg.response().headers(),buff);
    buff.put_s(2,(const unsigned char*)"\r\n");
    
    //HTTP headres
    if(pl__msg.response().http__headers().ispresent()){
        const HTTPMessage__List* ptr = &pl__msg.response().http__headers()();
        int a = ptr->size_of();
        OCTETSTRING header;
        for(int i=0;i<a;i++){
          header = encoding_HTTP_Header(pl__msg.response().http__headers()()[i]);
          buff.put_s(header.lengthof(),(const unsigned char*)header);
        };
    };
    
    //HTTP body
    if(pl__msg.response().http__body().ispresent()){
        int len=pl__msg.response().http__body()().lengthof();
        char num[10];
        int n=sprintf(num,"%X",len);
        buff.put_s(n,(const unsigned char*)num);
        buff.put_s(2,(const unsigned char*)"\r\n");
        buff.put_string(pl__msg.response().http__body()());
        buff.put_s(2,(const unsigned char*)"\r\n");
        buff.put_s(5,(const unsigned char*)"0\r\n\r\n");
    };
  };
  
  return buff;
};


OCTETSTRING f__ICAP__Enc__binary(const PDU__ICAP& pl__msg)
{
  TTCN_Buffer buff = f_encoding(pl__msg);
  return OCTETSTRING(buff.get_len(), buff.get_data());
};

const CHARSTRING reqmod="REQMOD";
const CHARSTRING respmod="RESPMOD";
const CHARSTRING options="OPTIONS";
const CHARSTRING icap="ICAP";
const CHARSTRING rn="\r\n"; 
const CHARSTRING rnrn="\r\n\r\n"; 
const CHARSTRING zero_rnrn="0\r\n\r\n";
const CHARSTRING ieof_rnrn="0; ieof\r\n\r\n";

const CHARSTRING encap="Encapsulated: ";
const CHARSTRING reqh="req-hdr=";
const CHARSTRING reqb="req-body=";
const CHARSTRING resh="res-hdr=";
const CHARSTRING resb="res-body=";
const CHARSTRING ob="opt-body=";
const CHARSTRING nb="null-body=";
  
INTEGER f__ICAP__MessageLength(const OCTETSTRING& pl__stream){
  int len = -1;
  bool reading = true;
  int step=0;
//  int req_hdr=-1;
  int req_body=-1;
//  int res_hdr=-1;
  int res_body=-1;
  int opt_body=-1;
  int null_body=-1;   
  const unsigned char* msg=pl__stream;
  const unsigned char* ptr=pl__stream;
  char* pt;
  int msg_len=pl__stream.lengthof();
  
  while(reading){
    switch(step){
        //Searching for ICAP message
        case 0:{
                if(memcmp(ptr,(const char*)reqmod,6 )==0) {step=1;ptr+=5;};
                if(memcmp(ptr,(const char*)respmod,7)==0) {step=1;ptr+=6;};
                if(memcmp(ptr,(const char*)options,7)==0) {step=1;ptr+=6;};
                if(memcmp(ptr,(const char*)icap,4   )==0) {step=1;ptr+=3;};
                break;
               };
        //Found the begining of an ICAP message, searching for Encapsulated header
        case 1:{
                if(memcmp(ptr,(const char*)encap,14)==0){step=2; ptr+=13;};
                break;
               };
        //Getting data from Encapsulated header
        case 2:{
                if(memcmp(ptr,(const char*)reqh,8)==0){ptr+=8;/*req_hdr=strtol((const char*)ptr,&pt,10);*/};
                if(memcmp(ptr,(const char*)reqb,9)==0){ptr+=9;req_body=strtol((const char*)ptr,&pt,10);};
                if(memcmp(ptr,(const char*)resh,8)==0){ptr+=8;/*res_hdr=strtol((const char*)ptr,&pt,10);*/};
                if(memcmp(ptr,(const char*)resb,9)==0){ptr+=9;res_body=strtol((const char*)ptr,&pt,10);};
                if(memcmp(ptr,(const char*)ob,9)  ==0){ptr+=9;opt_body=strtol((const char*)ptr,&pt,10);};
                if(memcmp(ptr,(const char*)nb,10) ==0){ptr+=10;null_body=strtol((const char*)ptr,&pt,10);};
                if(memcmp(ptr,(const char*)rnrn,4)==0){ptr+=3;step=4;}; //eof ICAP header found
                if(step!=4 && memcmp(ptr,(const char*)rn,2)==0){ptr+=1;step=3;}; //eof Encapsulated header found
               };
        //Encapsulated header parsed, searching for end of ICAP header
        case 3:{
                if(memcmp(ptr,(const char*)rnrn,4)==0){ptr+=3;step=4;}; //eof ICAP header found
                break;
               };
        //End of ICAP header found, searching for body
        case 4:{
                //has no header nor body
                if(null_body==0){return ptr-msg;};
                
                //has headers and no body
                if(null_body>=0){return ptr-msg+null_body;}
                else{
                  //has body
                  if(res_body>=0||req_body>=0){
                    ptr+=(req_body>res_body?req_body:res_body);
                    step=5;
                  };                      
                };
                
                //has OPTIONS body
                if(opt_body>=0){step=5;};
                break;
               };
        //Searching for end of HTTP body
        case 5:{
                if(memcmp(ptr,(const char*)zero_rnrn,5)==0){
                  return ptr-msg+5;
                };
                if(memcmp(ptr,(const char*)ieof_rnrn,11)==0){
                  return ptr-msg+11;
                };
                break;
               };
        default: return -1;
    };

    ptr++;
    if(ptr-msg>msg_len)reading=false;
  };
  return len;
}

}//namespace


