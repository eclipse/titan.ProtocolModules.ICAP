/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         ICAP_parse_parse
#define yylex           ICAP_parse_lex
#define yyerror         ICAP_parse_error
#define yylval          ICAP_parse_lval
#define yychar          ICAP_parse_char
#define yydebug         ICAP_parse_debug
#define yynerrs         ICAP_parse_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "ICAP.y"

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



/* Line 268 of yacc.c  */
#line 201 "ICAP_parse_.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 293 of yacc.c  */
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



/* Line 293 of yacc.c  */
#line 310 "ICAP_parse_.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 322 "ICAP_parse_.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   227

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  71
/* YYNRULES -- Number of states.  */
#define YYNSTATES  190

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    12,    17,    23,    30,    31,
      34,    35,    37,    39,    45,    46,    49,    52,    54,    57,
      60,    62,    65,    70,    75,    80,    85,    90,    95,   100,
     105,   110,   115,   120,   125,   130,   135,   140,   145,   150,
     155,   160,   165,   170,   175,   180,   185,   190,   195,   200,
     202,   204,   207,   210,   214,   217,   221,   224,   228,   231,
     235,   238,   242,   245,   249,   251,   255,   257,   261,   263,
     267,   269
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      54,     0,    -1,    55,    -1,    56,    -1,    57,    64,    59,
      60,    -1,    58,    66,    59,    60,    -1,    61,    62,    14,
       7,    17,    -1,     7,    14,    13,    14,     8,    17,    -1,
      -1,    59,     9,    -1,    -1,    12,    -1,     3,    -1,    14,
       4,    15,     5,    63,    -1,    -1,    16,     6,    -1,    65,
      17,    -1,    68,    -1,    65,    68,    -1,    67,    17,    -1,
      68,    -1,    67,    68,    -1,    25,    14,     9,    17,    -1,
      26,    14,    13,    17,    -1,    27,    14,     9,    17,    -1,
      28,    14,     9,    17,    -1,    29,    14,     9,    17,    -1,
      30,    14,    69,    17,    -1,    31,    14,     9,    17,    -1,
      32,    14,     9,    17,    -1,    33,    14,     9,    17,    -1,
      34,    14,     9,    17,    -1,    35,    14,    13,    17,    -1,
      36,    14,     9,    17,    -1,    37,    14,     9,    17,    -1,
      38,    14,    13,    17,    -1,    39,    14,     9,    17,    -1,
      40,    14,    13,    17,    -1,    41,    14,     9,    17,    -1,
      45,    14,    72,    17,    -1,    46,    14,     9,    17,    -1,
      47,    14,     9,    17,    -1,    42,    14,     9,    17,    -1,
      48,    14,     9,    17,    -1,    49,    14,     9,    17,    -1,
      50,    14,    73,    17,    -1,    43,    14,    74,    17,    -1,
      44,    14,    75,    17,    -1,    10,    14,    11,    17,    -1,
      70,    -1,    71,    -1,    70,    71,    -1,    19,    13,    -1,
      18,    19,    13,    -1,    20,    13,    -1,    18,    20,    13,
      -1,    21,    13,    -1,    18,    21,    13,    -1,    22,    13,
      -1,    18,    22,    13,    -1,    23,    13,    -1,    18,    23,
      13,    -1,    24,    13,    -1,    18,    24,    13,    -1,     9,
      -1,    72,    14,     9,    -1,     9,    -1,    73,    18,     9,
      -1,     9,    -1,    74,    18,     9,    -1,     9,    -1,    75,
      14,     9,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   168,   168,   169,   171,   184,   197,   204,   213,   217,
     233,   234,   241,   246,   259,   260,   265,   269,   270,   274,
     278,   279,   283,   287,   292,   297,   302,   307,   312,   317,
     321,   325,   329,   333,   337,   341,   345,   350,   355,   359,
     363,   367,   371,   376,   380,   384,   388,   393,   397,   417,
     421,   422,   425,   429,   433,   437,   441,   445,   449,   453,
     457,   461,   465,   469,   475,   480,   488,   493,   501,   506,
     514,   519
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "METHOD", "SCHEME", "NET_PATH",
  "QUERY_TOKEN", "VERSION_TOKEN", "REASON", "CHR_TOKEN", "H_OTHER",
  "V_OTHER", "HTTP_BODY_TOKEN", "INT_TOKEN", "SP", "DSLASH", "QM", "CRLF",
  "COMMA_SP", "REQ_HDR", "RES_HDR", "RES_BODY", "REQ_BODY", "NULL_BODY",
  "OPT_BODY", "AUTHORIZATION", "ALLOW", "C_CONTROL", "CONNECTION",
  "DATE_TOKEN", "ENCAPSULATED", "EXPIRES", "FROM", "HOST", "ISTAG_TOKEN",
  "MAX_CONN", "METHODS", "OPT_BTYPE", "TTL", "PRAGMA", "PREVIEW",
  "REFERER", "TRAILER", "UPGRADE", "USER_A", "SERVER", "SERVICE",
  "SERVICE_ID", "TR_PRE", "TR_IG", "TR_COMP", "HEX_LEN", "EOF_BODY",
  "$accept", "icap_message", "request", "response", "RequestLine",
  "StatusLine", "HTTPMessage_List", "HTTP_Body", "Method", "Uri", "Query",
  "RequestHeaders", "RequestHeader_list", "ResponseHeaders",
  "ResponseHeader_list", "Header_list", "Encapsulated",
  "Encapsulated_list", "Params", "Server", "Transfer_Complete", "Upgrade",
  "User_Agent", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    53,    54,    54,    55,    56,    57,    58,    59,    59,
      60,    60,    61,    62,    63,    63,    64,    65,    65,    66,
      67,    67,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    69,
      70,    70,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    71,    71,    71,    72,    72,    73,    73,    74,    74,
      75,    75
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     4,     4,     5,     6,     0,     2,
       0,     1,     1,     5,     0,     2,     2,     1,     2,     2,
       1,     2,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     1,
       1,     2,     2,     3,     2,     3,     2,     3,     2,     3,
       2,     3,     2,     3,     1,     3,     1,     3,     1,     3,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    12,     0,     0,     2,     3,     0,     0,     0,     0,
       1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     8,     0,
      17,     8,     0,    20,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    10,    16,    18,    10,    19,    21,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    68,     0,    70,     0,    64,     0,     0,     0,     0,
       0,    66,     0,     9,    11,     4,     5,     0,     0,     0,
      48,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,    52,    54,    56,    58,    60,    62,    27,    51,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    42,    46,     0,     0,    47,     0,    39,    40,    41,
      43,    44,    45,     0,    14,     6,     7,    53,    55,    57,
      59,    61,    63,    69,    71,    65,    67,     0,    13,    15
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,     5,     6,     7,    74,   125,     8,    45,
     188,    38,    39,    41,    42,    40,    96,    97,    98,   116,
     122,   112,   114
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -19
static const yytype_int16 yypact[] =
{
       5,   -19,    -5,    13,   -19,   -19,    72,    72,    39,    41,
     -19,    76,    77,    78,    79,    80,    81,    82,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   -19,   -10,
     -19,   -19,    31,   -19,    51,   129,   130,   134,   137,   135,
     138,   140,   141,    65,   142,   143,   144,   145,   146,   147,
     148,   149,   151,   150,   152,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     2,   -19,   -19,     2,   -19,   -19,
     164,   166,   167,   165,   168,   169,   170,   171,   172,   -18,
     177,   178,   179,   180,   181,   182,   183,    65,   -19,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   -19,    32,   -19,    29,   -19,    30,   196,   197,   198,
     199,   -19,    34,   -19,   -19,   -19,   -19,   153,   200,   201,
     -19,   -19,   -19,   -19,   -19,   -19,   206,   207,   208,   209,
     210,   211,   -19,   -19,   -19,   -19,   -19,   -19,   -19,   -19,
     -19,   -19,   -19,   -19,   -19,   -19,   -19,   -19,   -19,   -19,
     -19,   -19,   -19,   174,   175,   -19,   216,   -19,   -19,   -19,
     -19,   -19,   -19,   217,   139,   -19,   -19,   -19,   -19,   -19,
     -19,   -19,   -19,   -19,   -19,   -19,   -19,   221,   -19,   -19
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -19,   -19,   -19,   -19,   -19,   -19,   133,    99,   -19,   -19,
     -19,   -19,   -19,   -19,   -19,     3,   -19,   -19,    83,   -19,
     -19,   -19,   -19
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      11,   136,   137,   138,   139,   140,   141,    75,     1,     9,
      43,   123,     2,    10,   124,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    11,    76,   164,   166,    79,   165,   167,    78,   162,
     163,   172,   173,    44,    46,    80,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    11,    89,    90,    91,    92,    93,    94,    95,
      47,    48,    49,    50,    51,    52,    53,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    81,    82,    83,    84,    86,    85,    87,
      88,    99,   100,   101,   102,   187,   104,   105,   174,   103,
     107,   109,   106,   108,   110,   111,   113,   115,   117,   118,
     119,   120,   121,   128,    77,   129,   126,     0,     0,   127,
     149,     0,   130,   183,   184,   131,   132,   133,   134,   135,
     142,   143,   144,   145,   146,   147,     0,     0,     0,     0,
     148,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   168,   169,   170,   171,   175,   176,   177,
     178,   179,   180,   181,   182,   185,   186,   189
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-19))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
      10,    19,    20,    21,    22,    23,    24,    17,     3,    14,
       7,     9,     7,     0,    12,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    10,    39,    14,    14,    42,    17,    17,    17,    17,
      18,    17,    18,    14,    13,     4,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    10,    18,    19,    20,    21,    22,    23,    24,
      14,    14,    14,    14,    14,    14,    14,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    11,     9,     9,    13,     9,
       9,     9,     9,     9,     9,    16,     9,     9,     5,    13,
       9,     9,    13,    13,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     7,    41,     8,    77,    -1,    -1,    15,
      97,    -1,    17,     9,     9,    17,    17,    17,    17,    17,
      13,    13,    13,    13,    13,    13,    -1,    -1,    -1,    -1,
      17,    17,    17,    17,    17,    17,    17,    17,    17,    17,
      17,    17,    17,    17,    17,    17,    17,    17,    17,    13,
      13,    13,    13,    13,    13,     9,     9,     6
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     7,    54,    55,    56,    57,    58,    61,    14,
       0,    10,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    64,    65,
      68,    66,    67,    68,    14,    62,    13,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    59,    17,    68,    59,    17,    68,
       4,    14,    14,    11,     9,    13,     9,     9,     9,    18,
      19,    20,    21,    22,    23,    24,    69,    70,    71,     9,
       9,     9,     9,    13,     9,     9,    13,     9,    13,     9,
       9,     9,    74,     9,    75,     9,    72,     9,     9,     9,
       9,     9,    73,     9,    12,    60,    60,    15,     7,     8,
      17,    17,    17,    17,    17,    17,    19,    20,    21,    22,
      23,    24,    13,    13,    13,    13,    13,    13,    17,    71,
      17,    17,    17,    17,    17,    17,    17,    17,    17,    17,
      17,    17,    17,    18,    14,    17,    14,    17,    17,    17,
      17,    17,    17,    18,     5,    17,    17,    13,    13,    13,
      13,    13,    13,     9,     9,     9,     9,    16,    63,     6
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 3: /* "METHOD" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1417 "ICAP_parse_.tab.c"
	break;
      case 4: /* "SCHEME" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1426 "ICAP_parse_.tab.c"
	break;
      case 5: /* "NET_PATH" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1435 "ICAP_parse_.tab.c"
	break;
      case 6: /* "QUERY_TOKEN" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1444 "ICAP_parse_.tab.c"
	break;
      case 7: /* "VERSION_TOKEN" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1453 "ICAP_parse_.tab.c"
	break;
      case 8: /* "REASON" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1462 "ICAP_parse_.tab.c"
	break;
      case 9: /* "CHR_TOKEN" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1471 "ICAP_parse_.tab.c"
	break;
      case 10: /* "H_OTHER" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1480 "ICAP_parse_.tab.c"
	break;
      case 11: /* "V_OTHER" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1489 "ICAP_parse_.tab.c"
	break;
      case 12: /* "HTTP_BODY_TOKEN" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->charstring);};

/* Line 1391 of yacc.c  */
#line 1498 "ICAP_parse_.tab.c"
	break;
      case 13: /* "INT_TOKEN" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->integer);};

/* Line 1391 of yacc.c  */
#line 1507 "ICAP_parse_.tab.c"
	break;
      case 57: /* "RequestLine" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->requestline);};

/* Line 1391 of yacc.c  */
#line 1516 "ICAP_parse_.tab.c"
	break;
      case 58: /* "StatusLine" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->statusline);};

/* Line 1391 of yacc.c  */
#line 1525 "ICAP_parse_.tab.c"
	break;
      case 59: /* "HTTPMessage_List" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->httpmsg_list);};

/* Line 1391 of yacc.c  */
#line 1534 "ICAP_parse_.tab.c"
	break;
      case 60: /* "HTTP_Body" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->httpbody);};

/* Line 1391 of yacc.c  */
#line 1543 "ICAP_parse_.tab.c"
	break;
      case 61: /* "Method" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->method);};

/* Line 1391 of yacc.c  */
#line 1552 "ICAP_parse_.tab.c"
	break;
      case 62: /* "Uri" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->uri);};

/* Line 1391 of yacc.c  */
#line 1561 "ICAP_parse_.tab.c"
	break;
      case 63: /* "Query" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->query);};

/* Line 1391 of yacc.c  */
#line 1570 "ICAP_parse_.tab.c"
	break;
      case 64: /* "RequestHeaders" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->requestheaders);};

/* Line 1391 of yacc.c  */
#line 1579 "ICAP_parse_.tab.c"
	break;
      case 66: /* "ResponseHeaders" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->responseheaders);};

/* Line 1391 of yacc.c  */
#line 1588 "ICAP_parse_.tab.c"
	break;
      case 69: /* "Encapsulated" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->encapsulated);};

/* Line 1391 of yacc.c  */
#line 1597 "ICAP_parse_.tab.c"
	break;
      case 72: /* "Server" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->server);};

/* Line 1391 of yacc.c  */
#line 1606 "ICAP_parse_.tab.c"
	break;
      case 73: /* "Transfer_Complete" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->transfer_complete);};

/* Line 1391 of yacc.c  */
#line 1615 "ICAP_parse_.tab.c"
	break;
      case 74: /* "Upgrade" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->upgrade);};

/* Line 1391 of yacc.c  */
#line 1624 "ICAP_parse_.tab.c"
	break;
      case 75: /* "User_Agent" */

/* Line 1391 of yacc.c  */
#line 140 "ICAP.y"
	{ delete (yyvaluep->user_agent);};

/* Line 1391 of yacc.c  */
#line 1633 "ICAP_parse_.tab.c"
	break;

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 168 "ICAP.y"
    {YYACCEPT;}
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 169 "ICAP.y"
    {YYACCEPT;}
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 171 "ICAP.y"
    {
        ICAP_parse_parsed_message->request().requestline()= *(yyvsp[(1) - (4)].requestline);
        ICAP_parse_parsed_message->request().headers()= *(yyvsp[(2) - (4)].requestheaders);
        if(isUnbound)ICAP_parse_parsed_message->request().http__headers()=OMIT_VALUE;
        else ICAP_parse_parsed_message->request().http__headers()()=*(yyvsp[(3) - (4)].httpmsg_list);
        if((yyvsp[(4) - (4)].httpbody))ICAP_parse_parsed_message->request().http__body()()=*(yyvsp[(4) - (4)].httpbody);
        else ICAP_parse_parsed_message->request().http__body()=OMIT_VALUE;
        delete (yyvsp[(1) - (4)].requestline);
        delete (yyvsp[(2) - (4)].requestheaders);
        delete (yyvsp[(3) - (4)].httpmsg_list);
        delete (yyvsp[(4) - (4)].httpbody);
}
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 184 "ICAP.y"
    {
        ICAP_parse_parsed_message->response().statusline()= *(yyvsp[(1) - (4)].statusline);
        ICAP_parse_parsed_message->response().headers()= *(yyvsp[(2) - (4)].responseheaders);
        if(isUnbound)ICAP_parse_parsed_message->response().http__headers()=OMIT_VALUE;
        else ICAP_parse_parsed_message->response().http__headers()()=*(yyvsp[(3) - (4)].httpmsg_list);
        if((yyvsp[(4) - (4)].httpbody))ICAP_parse_parsed_message->response().http__body()()=*(yyvsp[(4) - (4)].httpbody);
        else ICAP_parse_parsed_message->response().http__body()=OMIT_VALUE;
        delete (yyvsp[(1) - (4)].statusline);
        delete (yyvsp[(2) - (4)].responseheaders);
        delete (yyvsp[(3) - (4)].httpmsg_list);
        delete (yyvsp[(4) - (4)].httpbody);
}
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 197 "ICAP.y"
    {
        (yyval.requestline)=new RequestLine(*(yyvsp[(1) - (5)].method), *(yyvsp[(2) - (5)].uri), *(yyvsp[(4) - (5)].charstring));
        delete (yyvsp[(1) - (5)].method);
        delete (yyvsp[(2) - (5)].uri);
        delete (yyvsp[(4) - (5)].charstring);
}
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 204 "ICAP.y"
    {
        (yyval.statusline)=new StatusLine(*(yyvsp[(1) - (6)].charstring), *(yyvsp[(3) - (6)].integer), *(yyvsp[(5) - (6)].charstring));
        delete (yyvsp[(1) - (6)].charstring);
        delete (yyvsp[(3) - (6)].integer);
        delete (yyvsp[(5) - (6)].charstring);
}
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 213 "ICAP.y"
    {
        (yyval.httpmsg_list) = new HTTPMessage__List;
        isUnbound = true;
      }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 217 "ICAP.y"
    {
        int a=0;
        CHARSTRING value;
        if(isUnbound){
          (*(yyval.httpmsg_list))[0] = f_decode_HTTPHeaders(*(yyvsp[(2) - (2)].charstring));
          delete (yyvsp[(2) - (2)].charstring);
          isUnbound = false;
        }
        else{
          a=(yyvsp[(1) - (2)].httpmsg_list)->size_of();
          (yyval.httpmsg_list)=(yyvsp[(1) - (2)].httpmsg_list);
          (*(yyval.httpmsg_list))[a] = f_decode_HTTPHeaders(*(yyvsp[(2) - (2)].charstring));
          delete (yyvsp[(2) - (2)].charstring);
        }; 
      }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 233 "ICAP.y"
    {(yyval.httpbody) = NULL;}
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 234 "ICAP.y"
    { 
          (yyval.httpbody) = new HTTP__Body(f_decode_HTTPBody(*(yyvsp[(1) - (1)].charstring))); 
          delete (yyvsp[(1) - (1)].charstring);
        }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 241 "ICAP.y"
    {
          (yyval.method)=new Method(Method::str_to_enum((const char*)(*(yyvsp[(1) - (1)].charstring))));
          delete (yyvsp[(1) - (1)].charstring);
}
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 246 "ICAP.y"
    {
        if((yyvsp[(5) - (5)].query)){
          (yyval.uri)=new Uri(*(yyvsp[(2) - (5)].charstring), *(yyvsp[(4) - (5)].charstring), *(yyvsp[(5) - (5)].query));
          delete (yyvsp[(2) - (5)].charstring);
          delete (yyvsp[(4) - (5)].charstring);
          delete (yyvsp[(5) - (5)].query);
        } else {
          (yyval.uri)=new Uri(*(yyvsp[(2) - (5)].charstring), *(yyvsp[(4) - (5)].charstring), OMIT_VALUE);
          delete (yyvsp[(2) - (5)].charstring);
          delete (yyvsp[(4) - (5)].charstring);
        }
}
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 259 "ICAP.y"
    {(yyval.query)=NULL;}
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 260 "ICAP.y"
    {
        (yyval.query)=new Query(*(yyvsp[(2) - (2)].charstring));
        delete (yyvsp[(2) - (2)].charstring);
      }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 265 "ICAP.y"
    {
        (yyval.requestheaders) = new RequestHeaders(*RequestHeaders_parsed);
}
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 269 "ICAP.y"
    { /* empty */ }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 270 "ICAP.y"
    { /* empty */ }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 274 "ICAP.y"
    {
        (yyval.responseheaders) = new ResponseHeaders(*ResponseHeaders_parsed);
}
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 278 "ICAP.y"
    { /* empty */ }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 279 "ICAP.y"
    { /* empty */ }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 283 "ICAP.y"
    {
        RequestHeaders_parsed->authorization()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 287 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->allow()()=*(yyvsp[(3) - (4)].integer);
        else ResponseHeaders_parsed->allow()()=*(yyvsp[(3) - (4)].integer);
        delete (yyvsp[(3) - (4)].integer);
      }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 292 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->cache__control()()=*(yyvsp[(3) - (4)].charstring);
        else ResponseHeaders_parsed->cache__control()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 297 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->connection()()=*(yyvsp[(3) - (4)].charstring);
        else ResponseHeaders_parsed->connection()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 302 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->date()()=*(yyvsp[(3) - (4)].charstring);
        else ResponseHeaders_parsed->date()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 307 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->encapsulated()=*(yyvsp[(3) - (4)].encapsulated);
        else ResponseHeaders_parsed->encapsulated()=*(yyvsp[(3) - (4)].encapsulated);
        delete (yyvsp[(3) - (4)].encapsulated);
      }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 312 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->expires()()=*(yyvsp[(3) - (4)].charstring);
        else ResponseHeaders_parsed->expires()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 317 "ICAP.y"
    {
        RequestHeaders_parsed->fRom()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 321 "ICAP.y"
    {
        RequestHeaders_parsed->host()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 325 "ICAP.y"
    {
        ResponseHeaders_parsed->istag()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 329 "ICAP.y"
    {
        ResponseHeaders_parsed->max__connections()()=*(yyvsp[(3) - (4)].integer);
        delete (yyvsp[(3) - (4)].integer);
      }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 333 "ICAP.y"
    {
        ResponseHeaders_parsed->methods()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 337 "ICAP.y"
    {
        ResponseHeaders_parsed->opt__body__type()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 341 "ICAP.y"
    {
        ResponseHeaders_parsed->options__ttl()()=*(yyvsp[(3) - (4)].integer);
        delete (yyvsp[(3) - (4)].integer);
      }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 345 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->pragma()()=*(yyvsp[(3) - (4)].charstring);
        else ResponseHeaders_parsed->pragma()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 350 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->preview()()=*(yyvsp[(3) - (4)].integer);
        else ResponseHeaders_parsed->preview()()=*(yyvsp[(3) - (4)].integer);
        delete (yyvsp[(3) - (4)].integer);
      }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 355 "ICAP.y"
    {
        RequestHeaders_parsed->referer()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 359 "ICAP.y"
    {
        ResponseHeaders_parsed->server()()=*(yyvsp[(3) - (4)].server);
        delete (yyvsp[(3) - (4)].server);
      }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 363 "ICAP.y"
    {
        ResponseHeaders_parsed->service()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 367 "ICAP.y"
    {
        ResponseHeaders_parsed->service__id()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 371 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->trailer()()=*(yyvsp[(3) - (4)].charstring);
        else ResponseHeaders_parsed->trailer()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 376 "ICAP.y"
    {
        ResponseHeaders_parsed->transfer__preview()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 380 "ICAP.y"
    {
        ResponseHeaders_parsed->transfer__ignore()()=*(yyvsp[(3) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 384 "ICAP.y"
    {
        ResponseHeaders_parsed->transfer__complete()()=*(yyvsp[(3) - (4)].transfer_complete);
        delete (yyvsp[(3) - (4)].transfer_complete);
      }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 388 "ICAP.y"
    {
        if(isRequest)RequestHeaders_parsed->upgrade()()=*(yyvsp[(3) - (4)].upgrade);
        else ResponseHeaders_parsed->upgrade()()=*(yyvsp[(3) - (4)].upgrade);
        delete (yyvsp[(3) - (4)].upgrade);
      }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 393 "ICAP.y"
    {
        RequestHeaders_parsed->user__agent()()=*(yyvsp[(3) - (4)].user_agent);
        delete (yyvsp[(3) - (4)].user_agent);
      }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 397 "ICAP.y"
    {
        int a=0;
        if(isRequest){
          if(RequestHeaders_parsed->undefinedHeader__List().ispresent()){
            a=RequestHeaders_parsed->undefinedHeader__List()().size_of();
          }                         
          RequestHeaders_parsed->undefinedHeader__List()()[a].headerName()=*(yyvsp[(1) - (4)].charstring);
          RequestHeaders_parsed->undefinedHeader__List()()[a].headerValue()=*(yyvsp[(3) - (4)].charstring);
        } else {
          if(ResponseHeaders_parsed->undefinedHeader__List().ispresent()){
            a=ResponseHeaders_parsed->undefinedHeader__List()().size_of();
          }                         
          ResponseHeaders_parsed->undefinedHeader__List()()[a].headerName()=*(yyvsp[(1) - (4)].charstring);
          ResponseHeaders_parsed->undefinedHeader__List()()[a].headerValue()=*(yyvsp[(3) - (4)].charstring);
        }
        delete (yyvsp[(1) - (4)].charstring);
        delete (yyvsp[(3) - (4)].charstring);
      }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 417 "ICAP.y"
    {
        (yyval.encapsulated) = new Encapsulated(*encapsulated_parsed);       
}
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 421 "ICAP.y"
    { /* empty */ }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 422 "ICAP.y"
    { /* empty */ }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 425 "ICAP.y"
    {
        encapsulated_parsed->req__hdr()()=*(yyvsp[(2) - (2)].integer);
        delete (yyvsp[(2) - (2)].integer);
      }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 429 "ICAP.y"
    {
        encapsulated_parsed->req__hdr()()=*(yyvsp[(3) - (3)].integer);
        delete (yyvsp[(3) - (3)].integer);
      }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 433 "ICAP.y"
    {
        encapsulated_parsed->res__hdr()()=*(yyvsp[(2) - (2)].integer);
        delete (yyvsp[(2) - (2)].integer);
      }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 437 "ICAP.y"
    {
        encapsulated_parsed->res__hdr()()=*(yyvsp[(3) - (3)].integer);
        delete (yyvsp[(3) - (3)].integer);
      }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 441 "ICAP.y"
    {
        encapsulated_parsed->res__body()()=*(yyvsp[(2) - (2)].integer);
        delete (yyvsp[(2) - (2)].integer);
      }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 445 "ICAP.y"
    {
        encapsulated_parsed->res__body()()=*(yyvsp[(3) - (3)].integer);
        delete (yyvsp[(3) - (3)].integer);
      }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 449 "ICAP.y"
    {
        encapsulated_parsed->req__body()()=*(yyvsp[(2) - (2)].integer);
        delete (yyvsp[(2) - (2)].integer);
      }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 453 "ICAP.y"
    {
        encapsulated_parsed->req__body()()=*(yyvsp[(3) - (3)].integer);
        delete (yyvsp[(3) - (3)].integer);
      }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 457 "ICAP.y"
    {
        encapsulated_parsed->null__body()()=*(yyvsp[(2) - (2)].integer);
        delete (yyvsp[(2) - (2)].integer);
      }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 461 "ICAP.y"
    {
        encapsulated_parsed->null__body()()=*(yyvsp[(3) - (3)].integer);
        delete (yyvsp[(3) - (3)].integer);
      }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 465 "ICAP.y"
    {
        encapsulated_parsed->opt__body()()=*(yyvsp[(2) - (2)].integer);
        delete (yyvsp[(2) - (2)].integer);
      }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 469 "ICAP.y"
    {
        encapsulated_parsed->opt__body()()=*(yyvsp[(3) - (3)].integer);
        delete (yyvsp[(3) - (3)].integer);
      }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 475 "ICAP.y"
    {
        (yyval.server) = new Server;
        (*(yyval.server))[0] = *(yyvsp[(1) - (1)].charstring);
        delete (yyvsp[(1) - (1)].charstring);
      }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 480 "ICAP.y"
    {
        int a=(yyvsp[(1) - (3)].server)->size_of();
        (yyval.server)=(yyvsp[(1) - (3)].server);
        (*(yyval.server))[a] = *(yyvsp[(3) - (3)].charstring);
        delete (yyvsp[(3) - (3)].charstring);
      }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 488 "ICAP.y"
    {
        (yyval.transfer_complete) = new Transfer__Complete;
        (*(yyval.transfer_complete))[0] = *(yyvsp[(1) - (1)].charstring);
        delete (yyvsp[(1) - (1)].charstring);
      }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 493 "ICAP.y"
    {
        int a=(yyvsp[(1) - (3)].transfer_complete)->size_of();
        (yyval.transfer_complete)=(yyvsp[(1) - (3)].transfer_complete);
        (*(yyval.transfer_complete))[a] = *(yyvsp[(3) - (3)].charstring);
        delete (yyvsp[(3) - (3)].charstring);
      }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 501 "ICAP.y"
    {
        (yyval.upgrade) = new Upgrade;
        (*(yyval.upgrade))[0] = *(yyvsp[(1) - (1)].charstring);
        delete (yyvsp[(1) - (1)].charstring);
      }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 506 "ICAP.y"
    {
        int a=(yyvsp[(1) - (3)].upgrade)->size_of();
        (yyval.upgrade)=(yyvsp[(1) - (3)].upgrade);
        (*(yyval.upgrade))[a] = *(yyvsp[(3) - (3)].charstring);
        delete (yyvsp[(3) - (3)].charstring);
      }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 514 "ICAP.y"
    {
        (yyval.user_agent) = new User__Agent;
        (*(yyval.user_agent))[0] = *(yyvsp[(1) - (1)].charstring);
        delete (yyvsp[(1) - (1)].charstring);
      }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 519 "ICAP.y"
    {
        int a=(yyvsp[(1) - (3)].user_agent)->size_of();
        (yyval.user_agent)=(yyvsp[(1) - (3)].user_agent);
        (*(yyval.user_agent))[a] = *(yyvsp[(3) - (3)].charstring);
        delete (yyvsp[(3) - (3)].charstring);
      }
    break;



/* Line 1806 of yacc.c  */
#line 2678 "ICAP_parse_.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 526 "ICAP.y"


/*Additional C code*/

