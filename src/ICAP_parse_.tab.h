/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     METHOD = 258,
     SCHEME = 259,
     NET_PATH = 260,
     QUERY_TOKEN = 261,
     VERSION_TOKEN = 262,
     REASON = 263,
     CHR_TOKEN = 264,
     H_OTHER = 265,
     V_OTHER = 266,
     HTTP_BODY_TOKEN = 267,
     INT_TOKEN = 268,
     SP = 269,
     DSLASH = 270,
     QM = 271,
     CRLF = 272,
     COMMA_SP = 273,
     REQ_HDR = 274,
     RES_HDR = 275,
     RES_BODY = 276,
     REQ_BODY = 277,
     NULL_BODY = 278,
     OPT_BODY = 279,
     AUTHORIZATION = 280,
     ALLOW = 281,
     C_CONTROL = 282,
     CONNECTION = 283,
     DATE_TOKEN = 284,
     ENCAPSULATED = 285,
     EXPIRES = 286,
     FROM = 287,
     HOST = 288,
     ISTAG_TOKEN = 289,
     MAX_CONN = 290,
     METHODS = 291,
     OPT_BTYPE = 292,
     TTL = 293,
     PRAGMA = 294,
     PREVIEW = 295,
     REFERER = 296,
     TRAILER = 297,
     UPGRADE = 298,
     USER_A = 299,
     SERVER = 300,
     SERVICE = 301,
     SERVICE_ID = 302,
     TR_PRE = 303,
     TR_IG = 304,
     TR_COMP = 305,
     HEX_LEN = 306,
     EOF_BODY = 307
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 121 "ICAP.y"

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



/* Line 2068 of yacc.c  */
#line 123 "ICAP_parse_.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE ICAP_parse_lval;


