/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
/************ Begin %include sections from the grammar ************************/

/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    ParseTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYNTOKEN           Number of terminal symbols
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
**    YY_MIN_REDUCE      Minimum value for reduce actions
**    YY_MAX_REDUCE      Maximum value for reduce actions
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 84
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE void *
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 1000000
#endif
#define ParseARG_SDECL  struct geoJson_data *p_data ;
#define ParseARG_PDECL , struct geoJson_data *p_data 
#define ParseARG_FETCH  struct geoJson_data *p_data  = yypParser->p_data 
#define ParseARG_STORE yypParser->p_data  = p_data 
#define YYNSTATE             530
#define YYNRULE              159
#define YYNTOKEN             25
#define YY_MAX_SHIFT         529
#define YY_MIN_SHIFTREDUCE   677
#define YY_MAX_SHIFTREDUCE   835
#define YY_ERROR_ACTION      836
#define YY_ACCEPT_ACTION     837
#define YY_NO_ACTION         838
#define YY_MIN_REDUCE        839
#define YY_MAX_REDUCE        997
/************* End control #defines *******************************************/

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = yy_action[ yy_shift_ofst[S] + X ]
**    (B)   N = yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X.
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (774)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   529,  529,  529,  839,  840,  841,  842,  843,  844,  845,
 /*    10 */   846,  847,  848,  849,  850,  851,  852,  526,  102,  186,
 /*    20 */    95,   94,   93,  512,   86,   85,   92,  222,   91,   90,
 /*    30 */   471,  426,  379,  340,  301,  262,  101,  238,  100,   99,
 /*    40 */   219,  216,  255,  213,  837,    1,  205,  300,  707,  295,
 /*    50 */   234,  230,  287,  475,  473,  251,  247,  339,  261,  334,
 /*    60 */   708,  709,  326,  378,  425,  373,  412,    3,  365,  404,
 /*    70 */   470,  525,  459,  520,  198,  451,  509,  992,  512,  528,
 /*    80 */   182,  181,  190,  189,  194,  193,  202,  201,  204,  192,
 /*    90 */   207,  206,  211,  244,  227,  224,  512,  248,  231,  252,
 /*   100 */   260,  259,  240,  235,  264,  263,  268,  272,  271,  257,
 /*   110 */   512,  280,  276,  275,    2,  512,  284,  283,  286,  274,
 /*   120 */   289,  288,  293,  110,  106,  307,  512,  299,  298,  512,
 /*   130 */   325,  313,  303,  302,  311,  310,  319,  315,  314,  484,
 /*   140 */   512,  118,  114,  323,  322,  328,  327,  332,  225,  338,
 /*   150 */   337,  512,  342,  341,  346,  350,  349,   62,  512,  358,
 /*   160 */   354,  353,  226,  512,   73,  362,  361,  364,  352,  367,
 /*   170 */   366,  371,  120,  381,  380,  512,  119,  377,  376,  385,
 /*   180 */   389,  388,  152,  512,  397,  393,  392,  705,  512,  401,
 /*   190 */   400,  403,  391,  406,  405,  231,  410,  421,  424,  423,
 /*   200 */   512,  416,  136,  154,  128,  432,  428,  427,   28,  512,
 /*   210 */   436,  435,  440,  439,  444,  448,  447,   39,  512,  450,
 /*   220 */   438,  453,  452,  457,  467,  469,  468,  512,  463,  486,
 /*   230 */    50,  490,  494,  485,  498,  512,  493,  502,  497,  506,
 /*   240 */    61,  512,  188,  505,  508,  496,  514,  200,  518,  104,
 /*   250 */   513,  524,  512,  227,  273,  523,  270,  191,  285,  312,
 /*   260 */   282,  309,  203,  324,  351,  321,  348,  363,  390,  360,
 /*   270 */   387,  402,  437,  399,  434,  449,  495,  446,  492,  507,
 /*   280 */   112,  504,    4,  141,  235,  985,  244,  242,  984,  983,
 /*   290 */   241,  243,  108,  116,  147,  248,  105,  978,  252,  977,
 /*   300 */   976,  258,  126,  959,  128,  109,  103,  134,  296,  136,
 /*   310 */   113,  107,  956,  297,  941,  463,  117,  335,  111,  467,
 /*   320 */   138,  374,  938,  144,  127,  336,  115,  135,  375,  122,
 /*   330 */   414,    6,  413,  214,  415,  416,  130,  809,  419,  909,
 /*   340 */   121,  420,  418,  421,  803,  184,  185,    7,  906,  417,
 /*   350 */   137,  187,  139,  129,  875,  422,  460,  461,  462,  873,
 /*   360 */   143,  465,  464,  466,  145,  183,  997,  807,  801,    8,
 /*   370 */   510,  808,  511,  521,  180,  522,  166,  802,    9,  195,
 /*   380 */   199,  196,  197,   10,   11,  806,  800,  167,   12,   13,
 /*   390 */   805,  799,  208,  209,  212,   15,   16,  210,   14,  215,
 /*   400 */   819,  217,  218,   17,  220,  223,  221,  156,  228,  818,
 /*   410 */    87,  229,  829,   88,  233,  827,   89,  232,  157,  825,
 /*   420 */   236,  237,  239,  838,   96,   97,  838,  817,  812,  811,
 /*   430 */   828,  826,  245,  249,  246,  250,   98,  254,  824,  158,
 /*   440 */   253,  256,  838,  159,  804,  810,   18,  798,  791,  785,
 /*   450 */    19,  265,  838,  266,  269,  838,  267,  789,   20,  783,
 /*   460 */   168,   21,   23,  790,   22,  278,  784,  281,  277,   24,
 /*   470 */   838,   25,  788,  279,  782,  169,  787,   26,  781,  290,
 /*   480 */   291,  838,  294,  838,  786,  838,  292,   27,  838,  780,
 /*   490 */   795,  792,   29,  160,  306,   31,   30,  838,  773,  767,
 /*   500 */   304,  170,  771,  305,  308,   32,   34,  765,   33,  838,
 /*   510 */   772,  766,  316,  320,  317,  770,   35,  318,  838,  764,
 /*   520 */   171,   36,   37,  769,  763,  838,  476,  329,  330,  333,
 /*   530 */   838,  478,  331,   38,  838,  777,  838,  768,  762,  774,
 /*   540 */    40,  759,  753,   41,  161,  838,  479,  343,  344,  347,
 /*   550 */   172,  838,  345,   42,   43,  838,  757,  751,   44,  758,
 /*   560 */   481,  752,  355,  356,  838,  482,  359,  756,   46,  357,
 /*   570 */    45,  750,   47,  173,  702,   48,  838,  755,  749,  368,
 /*   580 */   369,  838,  372,  696,  370,  838,   49,   51,   75,  754,
 /*   590 */   761,  760,  838,  748,  162,  739,   52,  383,  733,   76,
 /*   600 */   386,  382,   54,  838,  491,  174,  384,   53,  838,  737,
 /*   610 */   731,  738,   55,  395,  732,   79,  398,  394,   57,  838,
 /*   620 */   503,  175,  396,   56,  838,  736,  730,   58,  515,   60,
 /*   630 */   735,   59,  729,  407,  408,  516,  411,  838,  838,  123,
 /*   640 */   124,  745,  409,  125,  697,  838,  741,  131,  132,  133,
 /*   650 */   838,  742,  838,  163,   63,  740,  734,  728,  838,   64,
 /*   660 */   725,  838,  719,  429,  430,  433,  838,  838,  431,   66,
 /*   670 */    65,  723,  717,  838,  724,  176,   67,  442,  718,  838,
 /*   680 */   441,  838,  445,   69,  722,  477,  443,   68,  838,  716,
 /*   690 */   177,   70,  480,  721,   71,  838,  715,  838,  454,  488,
 /*   700 */   455,  458,  838,  456,  838,   72,  838,  152,  140,  838,
 /*   710 */   727,  489,  142,   74,  838,  726,   77,  146,  838,  705,
 /*   720 */   164,  720,  148,  178,  838,  838,  714,  838,  704,  472,
 /*   730 */   703,  474,  701,    5,  483,  700,  838,  487,   78,  838,
 /*   740 */   694,  695,  499,   80,  500,  838,  838,  501,   81,  699,
 /*   750 */   693,  179,  838,   83,  149,  150,  151,  838,  519,  698,
 /*   760 */   692,   82,  838,  832,  527,  153,  517,  706,  691,  838,
 /*   770 */    84,  838,  155,  165,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    28,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*    10 */    38,   39,   40,   41,   42,   43,   44,    5,   75,   46,
 /*    20 */    77,   78,   79,   50,   81,   82,   79,    5,   81,   82,
 /*    30 */    18,   19,   20,   21,   22,   23,   75,    5,   77,   78,
 /*    40 */    18,   19,    5,    9,   26,   27,   12,    7,   15,    9,
 /*    50 */    18,   19,   12,   51,   52,   18,   19,    7,   24,    9,
 /*    60 */    16,   17,   12,    7,    7,    9,    9,    6,   12,   12,
 /*    70 */     7,    7,    9,    9,   46,   12,   12,    0,   50,    2,
 /*    80 */    73,   74,   73,   74,   73,   74,   73,   74,   47,   48,
 /*    90 */    73,   74,   46,   57,   58,    2,   50,   55,   56,   45,
 /*   100 */    73,   74,    2,   49,   69,   70,   46,   69,   70,    2,
 /*   110 */    50,   46,   69,   70,   10,   50,   69,   70,   47,   48,
 /*   120 */    69,   70,   46,   57,   58,   46,   50,   69,   70,   50,
 /*   130 */    47,   48,   65,   66,   65,   66,   46,   65,   66,    2,
 /*   140 */    50,   55,   56,   65,   66,   65,   66,   46,   80,   65,
 /*   150 */    66,   50,   63,   64,   46,   63,   64,   10,   50,   46,
 /*   160 */    63,   64,   80,   50,   10,   63,   64,   47,   48,   63,
 /*   170 */    64,   46,   45,   57,   58,   50,   49,   63,   64,   46,
 /*   180 */    57,   58,    6,   50,   46,   57,   58,   11,   50,   57,
 /*   190 */    58,   47,   48,   57,   58,   56,   46,   45,   57,   58,
 /*   200 */    50,   49,   59,   10,   61,   46,   55,   56,   10,   50,
 /*   210 */    55,   56,   55,   56,   46,   55,   56,   10,   50,   47,
 /*   220 */    48,   55,   56,   46,   45,   55,   56,   50,   49,   45,
 /*   230 */    10,   46,   45,   49,   45,   50,   49,   46,   49,   45,
 /*   240 */    10,   50,    9,   49,   47,   48,   45,    9,   46,   10,
 /*   250 */    49,   45,   50,   58,    7,   49,    9,   24,    7,    7,
 /*   260 */     9,    9,   24,    7,    7,    9,    9,    7,    7,    9,
 /*   270 */     9,    7,    7,    9,    9,    7,    7,    9,    9,    7,
 /*   280 */    10,    9,    6,   10,   49,   80,   57,   76,   80,   80,
 /*   290 */    80,   76,   10,   10,   10,   55,    6,   76,   45,   76,
 /*   300 */    76,   76,   10,   72,   61,    6,   58,   10,   72,   59,
 /*   310 */     6,   57,   71,   71,   68,   49,    6,   68,   56,   45,
 /*   320 */     6,   54,   67,    6,    6,   67,   55,    6,   53,   49,
 /*   330 */    49,    4,   54,    7,   49,   49,   45,    8,   45,   62,
 /*   340 */    61,   45,   53,   45,    8,    6,   11,   10,   60,   62,
 /*   350 */    49,    4,   49,   59,   54,   60,   54,   50,   50,   53,
 /*   360 */    45,   50,   53,   50,   45,   24,   50,    8,    8,    4,
 /*   370 */    50,    8,   50,   50,   50,   50,    6,    8,    4,   24,
 /*   380 */     4,    6,   11,   10,    4,    8,    8,    6,    4,    4,
 /*   390 */     8,    8,   24,    6,    4,    4,    4,   11,   10,    6,
 /*   400 */    11,    7,    6,    4,    7,    3,    6,    4,    7,   11,
 /*   410 */     4,    6,    8,    4,    6,    8,    4,    7,    4,    8,
 /*   420 */     7,    6,    3,   83,    4,    4,   83,   11,   11,   11,
 /*   430 */     8,    8,    7,    7,    6,    6,    4,    6,    8,    4,
 /*   440 */     7,    3,   83,    6,    8,   11,    4,    8,    8,    8,
 /*   450 */     4,    7,   83,    6,    4,   83,   11,    8,   10,    8,
 /*   460 */     6,    4,   10,    8,    4,    6,    8,    4,    7,    4,
 /*   470 */    83,    4,    8,   11,    8,    6,    8,    4,    8,    7,
 /*   480 */     6,   83,    4,   83,    8,   83,   11,   10,   83,    8,
 /*   490 */    11,   11,    4,    6,   11,   10,    4,   83,    8,    8,
 /*   500 */     7,    6,    8,    6,    4,    4,   10,    8,    4,   83,
 /*   510 */     8,    8,    7,    4,    6,    8,    4,   11,   83,    8,
 /*   520 */     6,    4,    4,    8,    8,   83,   13,    7,    6,    4,
 /*   530 */    83,    4,   11,   10,   83,   11,   83,    8,    8,   11,
 /*   540 */     4,    8,    8,    4,    6,   83,   14,    7,    6,    4,
 /*   550 */     6,   83,   11,   10,    4,   83,    8,    8,    4,    8,
 /*   560 */    13,    8,    7,    6,   83,    4,    4,    8,    4,   11,
 /*   570 */    10,    8,    4,    6,    8,    4,   83,    8,    8,    7,
 /*   580 */     6,   83,    4,    8,   11,   83,   10,    4,    4,    8,
 /*   590 */    11,   11,   83,    8,    6,    8,    4,    6,    8,   10,
 /*   600 */     4,    7,    4,   83,    4,    6,   11,   10,   83,    8,
 /*   610 */     8,    8,    4,    6,    8,   10,    4,    7,    4,   83,
 /*   620 */     4,    6,   11,   10,   83,    8,    8,    4,    7,   10,
 /*   630 */     8,    4,    8,    7,    6,    6,    4,   83,   83,    6,
 /*   640 */     6,   11,   11,    6,    8,   83,   11,    6,    6,    6,
 /*   650 */    83,   11,   83,    6,    4,   11,    8,    8,   83,    4,
 /*   660 */     8,   83,    8,    7,    6,    4,   83,   83,   11,    4,
 /*   670 */    10,    8,    8,   83,    8,    6,    4,    6,    8,   83,
 /*   680 */     7,   83,    4,    4,    8,    2,   11,   10,   83,    8,
 /*   690 */     6,    4,    6,    8,    4,   83,    8,   83,    7,    6,
 /*   700 */     6,    4,   83,   11,   83,   10,   83,    6,    6,   83,
 /*   710 */    11,   11,    6,    4,   83,   11,    4,    6,   83,   11,
 /*   720 */     6,    8,    6,    6,   83,   83,    8,   83,    8,    8,
 /*   730 */     8,    8,    8,    4,    3,    8,   83,    7,    4,   83,
 /*   740 */     8,    8,    7,    4,    6,   83,   83,   11,    4,    8,
 /*   750 */     8,    6,   83,   10,    6,    6,    6,   83,    4,    8,
 /*   760 */     8,    4,   83,    1,    3,    6,   11,   11,    8,   83,
 /*   770 */     4,   83,    4,    6,   83,   83,   83,   83,   83,   83,
 /*   780 */    83,   83,   83,   83,   83,   83,   83,   83,   83,   83,
 /*   790 */    83,   83,   83,   83,   83,   83,   83,   83,   83,
};
#define YY_SHIFT_COUNT    (529)
#define YY_SHIFT_MIN      (0)
#define YY_SHIFT_MAX      (768)
static const unsigned short int yy_shift_ofst[] = {
 /*     0 */   774,   77,   93,  100,  107,   44,  104,   33,  104,  104,
 /*    10 */    33,  104,  137,  104,   33,  147,  154,  193,  104,  198,
 /*    20 */    33,  198,  198,   33,  198,  137,  198,   33,  147,  198,
 /*    30 */   207,   33,  207,  207,   33,  207,  137,  207,   33,  154,
 /*    40 */   207,  220,   33,  220,  220,   33,  220,  137,  220,   33,
 /*    50 */   193,  220,  147,   33,  147,  147,   33,  147,  137,  147,
 /*    60 */    33,  193,  230,  147,  154,   33,  154,  154,   33,  154,
 /*    70 */   137,  154,   33,  193,  154,  193,   33,  193,  193,   33,
 /*    80 */   193,  137,  193,   33,  193,   61,   61,  239,  270,  273,
 /*    90 */    61,   61,   61,   61,  276,  276,  282,  283,  284,  276,
 /*   100 */   276,  276,  276,  290,  292,  239,  290,  299,  297,  282,
 /*   110 */   299,  304,  273,  270,  304,  310,  284,  283,  310,  314,
 /*   120 */   317,  318,  314,  273,  273,  273,  273,  292,  318,  321,
 /*   130 */   317,  284,  284,  284,  284,  297,  321,  314,  273,  314,
 /*   140 */    33,   33,  273,  317,  284,  317,   33,   33,  284,   33,
 /*   150 */    33,   33,   33,   33,   33,   12,   22,   32,   37,   34,
 /*   160 */    40,   50,   56,   57,   63,   64,  233,  238,  247,  251,
 /*   170 */   252,  256,  257,  260,  261,  264,  265,  268,  269,  272,
 /*   180 */   176,  329,  336,  327,  341,  339,  335,  337,  347,  359,
 /*   190 */   360,  365,  370,  363,  369,  374,  355,  375,  371,  373,
 /*   200 */   376,  377,  378,  380,  381,  384,  382,  383,  385,  368,
 /*   210 */   387,  386,  388,  390,  391,  326,  393,  392,  394,  396,
 /*   220 */   399,  397,  400,  403,  402,  389,  398,  404,  406,  401,
 /*   230 */   405,  407,  409,  410,  408,  411,  412,  413,  415,  414,
 /*   240 */   419,  416,  417,  418,  422,  420,  425,  428,  423,  421,
 /*   250 */   426,  429,  430,  432,  433,  431,  435,  438,  434,  436,
 /*   260 */   439,  442,  437,  440,  441,  446,  444,  447,  445,  448,
 /*   270 */   450,  449,  451,  457,  454,  455,  458,  460,  461,  459,
 /*   280 */   462,  452,  463,  464,  466,  465,  469,  467,  468,  470,
 /*   290 */   473,  472,  474,  475,  477,  478,  479,  480,  476,  481,
 /*   300 */   488,  487,  490,  491,  492,  493,  497,  483,  485,  500,
 /*   310 */   494,  499,  501,  495,  502,  503,  504,  505,  508,  506,
 /*   320 */   496,  509,  507,  511,  512,  514,  517,  515,  516,  518,
 /*   330 */   520,  522,  521,  523,  525,  524,  528,  529,  530,  536,
 /*   340 */   538,  533,  534,  539,  540,  542,  541,  543,  545,  548,
 /*   350 */   549,  550,  544,  551,  553,  554,  555,  557,  558,  560,
 /*   360 */   562,  559,  563,  564,  567,  568,  569,  570,  571,  572,
 /*   370 */   574,  573,  576,  578,  579,  580,  581,  585,  583,  588,
 /*   380 */   587,  590,  592,  594,  591,  595,  597,  596,  601,  602,
 /*   390 */   598,  599,  603,  606,  608,  610,  607,  611,  613,  612,
 /*   400 */   617,  618,  614,  615,  623,  622,  624,  627,  626,  628,
 /*   410 */   631,  619,  632,  630,  633,  634,  637,  635,  640,  641,
 /*   420 */   642,  643,  644,  648,  649,  650,  647,  652,  654,  655,
 /*   430 */   656,  658,  657,  660,  661,  663,  664,  665,  669,  666,
 /*   440 */   670,  672,  673,  671,  675,  677,  678,  676,  681,  679,
 /*   450 */   684,  687,  685,  688,  690,  691,  694,  692,  695,  697,
 /*   460 */   699,  701,  702,  706,  704,  708,  711,  716,  713,  718,
 /*   470 */   709,  714,  720,  721,  722,  723,  729,  513,  683,  527,
 /*   480 */   532,  686,  547,  561,  731,  566,  575,  584,  730,  693,
 /*   490 */   700,  589,  600,  727,  732,  712,  717,  724,  733,  734,
 /*   500 */   735,  738,  736,  605,  616,  741,  742,  739,  745,  744,
 /*   510 */   748,  749,  750,  751,  752,  757,  621,  629,  755,  743,
 /*   520 */   754,  756,  759,  636,  760,  766,  767,  768,  761,  762,
};
#define YY_REDUCE_COUNT (154)
#define YY_REDUCE_MIN   (-57)
#define YY_REDUCE_MAX   (325)
static const short yy_reduce_ofst[] = {
 /*     0 */    18,  -28,  -57,  -53,  -39,    2,    7,  -27,    9,   11,
 /*    10 */    28,   13,   41,   17,   46,   36,   42,   54,   27,   35,
 /*    20 */    60,   38,   43,   65,   47,   71,   51,   76,   66,   58,
 /*    30 */    67,   79,   69,   72,   90,   78,   83,   80,  101,   86,
 /*    40 */    84,   89,  108,   92,   97,  113,  102,  120,  106,  125,
 /*    50 */   127,  114,  116,  133,  123,  128,  138,  132,  144,  136,
 /*    60 */   150,  152,  143,  141,  151,  159,  155,  157,  168,  160,
 /*    70 */   172,  166,  177,  179,  170,  184,  185,  187,  189,  191,
 /*    80 */   194,  197,  201,  202,  206,   68,   82,  195,  139,  235,
 /*    90 */   205,  208,  209,  210,  211,  215,  229,  240,  253,  221,
 /*   100 */   223,  224,  225,  231,  243,  248,  236,  241,  250,  254,
 /*   110 */   242,  246,  266,  262,  249,  255,  274,  271,  258,  267,
 /*   120 */   275,  277,  278,  280,  281,  285,  286,  279,  287,  288,
 /*   130 */   289,  291,  293,  296,  298,  294,  295,  300,  301,  302,
 /*   140 */   307,  308,  303,  306,  315,  309,  311,  313,  319,  316,
 /*   150 */   320,  322,  323,  324,  325,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   993,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*    10 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*    20 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*    30 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*    40 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*    50 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*    60 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*    70 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*    80 */   836,  836,  836,  836,  836,  982,  982,  836,  836,  836,
 /*    90 */   982,  982,  982,  982,  975,  975,  836,  836,  836,  975,
 /*   100 */   975,  975,  975,  958,  836,  836,  958,  955,  836,  836,
 /*   110 */   955,  940,  836,  836,  940,  937,  836,  836,  937,  874,
 /*   120 */   872,  908,  874,  836,  836,  836,  836,  836,  908,  905,
 /*   130 */   872,  836,  836,  836,  836,  836,  905,  874,  836,  874,
 /*   140 */   836,  836,  836,  872,  836,  872,  836,  836,  836,  836,
 /*   150 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   160 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   170 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   180 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   190 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   200 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   210 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   220 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   230 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   240 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   250 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   260 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   270 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   280 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   290 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   300 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   310 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   320 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   330 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   340 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   350 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   360 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   370 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   380 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   390 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   400 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   410 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   420 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   430 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   440 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   450 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   460 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   470 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   480 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   490 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   500 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   510 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
 /*   520 */   836,  836,  836,  836,  836,  836,  836,  836,  836,  836,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos;          /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
  yyStackEntry yystk0;          /* First stack entry */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
  yyStackEntry *yystackEnd;            /* Last entry in the stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#if defined(YYCOVERAGE) || !defined(NDEBUG)
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  /*    0 */ "$",
  /*    1 */ "GEOJSON_NEWLINE",
  /*    2 */ "GEOJSON_OPEN_BRACE",
  /*    3 */ "GEOJSON_TYPE",
  /*    4 */ "GEOJSON_COLON",
  /*    5 */ "GEOJSON_POINT",
  /*    6 */ "GEOJSON_COMMA",
  /*    7 */ "GEOJSON_COORDS",
  /*    8 */ "GEOJSON_CLOSE_BRACE",
  /*    9 */ "GEOJSON_BBOX",
  /*   10 */ "GEOJSON_OPEN_BRACKET",
  /*   11 */ "GEOJSON_CLOSE_BRACKET",
  /*   12 */ "GEOJSON_CRS",
  /*   13 */ "GEOJSON_NAME",
  /*   14 */ "GEOJSON_PROPS",
  /*   15 */ "GEOJSON_NUM",
  /*   16 */ "GEOJSON_SHORT_SRID",
  /*   17 */ "GEOJSON_LONG_SRID",
  /*   18 */ "GEOJSON_LINESTRING",
  /*   19 */ "GEOJSON_POLYGON",
  /*   20 */ "GEOJSON_MULTIPOINT",
  /*   21 */ "GEOJSON_MULTILINESTRING",
  /*   22 */ "GEOJSON_MULTIPOLYGON",
  /*   23 */ "GEOJSON_GEOMETRYCOLLECTION",
  /*   24 */ "GEOJSON_GEOMS",
  /*   25 */ "error",
  /*   26 */ "main",
  /*   27 */ "in",
  /*   28 */ "state",
  /*   29 */ "program",
  /*   30 */ "geo_text",
  /*   31 */ "point",
  /*   32 */ "pointz",
  /*   33 */ "linestring",
  /*   34 */ "linestringz",
  /*   35 */ "polygon",
  /*   36 */ "polygonz",
  /*   37 */ "multipoint",
  /*   38 */ "multipointz",
  /*   39 */ "multilinestring",
  /*   40 */ "multilinestringz",
  /*   41 */ "multipolygon",
  /*   42 */ "multipolygonz",
  /*   43 */ "geocoll",
  /*   44 */ "geocollz",
  /*   45 */ "point_coordxy",
  /*   46 */ "bbox",
  /*   47 */ "short_crs",
  /*   48 */ "long_crs",
  /*   49 */ "point_coordxyz",
  /*   50 */ "coord",
  /*   51 */ "short_srid",
  /*   52 */ "long_srid",
  /*   53 */ "extra_pointsxy",
  /*   54 */ "extra_pointsxyz",
  /*   55 */ "linestring_text",
  /*   56 */ "linestring_textz",
  /*   57 */ "polygon_text",
  /*   58 */ "polygon_textz",
  /*   59 */ "ring",
  /*   60 */ "extra_rings",
  /*   61 */ "ringz",
  /*   62 */ "extra_ringsz",
  /*   63 */ "multipoint_text",
  /*   64 */ "multipoint_textz",
  /*   65 */ "multilinestring_text",
  /*   66 */ "multilinestring_textz",
  /*   67 */ "multilinestring_text2",
  /*   68 */ "multilinestring_textz2",
  /*   69 */ "multipolygon_text",
  /*   70 */ "multipolygon_textz",
  /*   71 */ "multipolygon_text2",
  /*   72 */ "multipolygon_textz2",
  /*   73 */ "geocoll_text",
  /*   74 */ "geocoll_textz",
  /*   75 */ "coll_point",
  /*   76 */ "geocoll_text2",
  /*   77 */ "coll_linestring",
  /*   78 */ "coll_polygon",
  /*   79 */ "coll_pointz",
  /*   80 */ "geocoll_textz2",
  /*   81 */ "coll_linestringz",
  /*   82 */ "coll_polygonz",
};
#endif /* defined(YYCOVERAGE) || !defined(NDEBUG) */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "geo_text ::= point",
 /*   1 */ "geo_text ::= pointz",
 /*   2 */ "geo_text ::= linestring",
 /*   3 */ "geo_text ::= linestringz",
 /*   4 */ "geo_text ::= polygon",
 /*   5 */ "geo_text ::= polygonz",
 /*   6 */ "geo_text ::= multipoint",
 /*   7 */ "geo_text ::= multipointz",
 /*   8 */ "geo_text ::= multilinestring",
 /*   9 */ "geo_text ::= multilinestringz",
 /*  10 */ "geo_text ::= multipolygon",
 /*  11 */ "geo_text ::= multipolygonz",
 /*  12 */ "geo_text ::= geocoll",
 /*  13 */ "geo_text ::= geocollz",
 /*  14 */ "point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE",
 /*  15 */ "point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE",
 /*  16 */ "point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE",
 /*  17 */ "point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE",
 /*  18 */ "point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE",
 /*  19 */ "point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE",
 /*  20 */ "pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE",
 /*  21 */ "pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE",
 /*  22 */ "pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE",
 /*  23 */ "pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE",
 /*  24 */ "point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE",
 /*  25 */ "point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE",
 /*  26 */ "short_crs ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_NAME GEOJSON_COMMA GEOJSON_PROPS GEOJSON_COLON GEOJSON_OPEN_BRACE GEOJSON_NAME GEOJSON_COLON short_srid GEOJSON_CLOSE_BRACE GEOJSON_CLOSE_BRACE",
 /*  27 */ "long_crs ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_NAME GEOJSON_COMMA GEOJSON_PROPS GEOJSON_COLON GEOJSON_OPEN_BRACE GEOJSON_NAME GEOJSON_COLON long_srid GEOJSON_CLOSE_BRACE GEOJSON_CLOSE_BRACE",
 /*  28 */ "point_coordxy ::= GEOJSON_OPEN_BRACKET coord GEOJSON_COMMA coord GEOJSON_CLOSE_BRACKET",
 /*  29 */ "point_coordxyz ::= GEOJSON_OPEN_BRACKET coord GEOJSON_COMMA coord GEOJSON_COMMA coord GEOJSON_CLOSE_BRACKET",
 /*  30 */ "coord ::= GEOJSON_NUM",
 /*  31 */ "short_srid ::= GEOJSON_SHORT_SRID",
 /*  32 */ "long_srid ::= GEOJSON_LONG_SRID",
 /*  33 */ "extra_pointsxy ::=",
 /*  34 */ "extra_pointsxy ::= GEOJSON_COMMA point_coordxy extra_pointsxy",
 /*  35 */ "extra_pointsxyz ::=",
 /*  36 */ "extra_pointsxyz ::= GEOJSON_COMMA point_coordxyz extra_pointsxyz",
 /*  37 */ "linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE",
 /*  38 */ "linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE",
 /*  39 */ "linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE",
 /*  40 */ "linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE",
 /*  41 */ "linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE",
 /*  42 */ "linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE",
 /*  43 */ "linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE",
 /*  44 */ "linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE",
 /*  45 */ "linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE",
 /*  46 */ "linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE",
 /*  47 */ "linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE",
 /*  48 */ "linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE",
 /*  49 */ "linestring_text ::= GEOJSON_OPEN_BRACKET point_coordxy GEOJSON_COMMA point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET",
 /*  50 */ "linestring_textz ::= GEOJSON_OPEN_BRACKET point_coordxyz GEOJSON_COMMA point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET",
 /*  51 */ "polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE",
 /*  52 */ "polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE",
 /*  53 */ "polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE",
 /*  54 */ "polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE",
 /*  55 */ "polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE",
 /*  56 */ "polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE",
 /*  57 */ "polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE",
 /*  58 */ "polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE",
 /*  59 */ "polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE",
 /*  60 */ "polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE",
 /*  61 */ "polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE",
 /*  62 */ "polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE",
 /*  63 */ "polygon_text ::= GEOJSON_OPEN_BRACKET ring extra_rings GEOJSON_CLOSE_BRACKET",
 /*  64 */ "polygon_textz ::= GEOJSON_OPEN_BRACKET ringz extra_ringsz GEOJSON_CLOSE_BRACKET",
 /*  65 */ "ring ::= GEOJSON_OPEN_BRACKET point_coordxy GEOJSON_COMMA point_coordxy GEOJSON_COMMA point_coordxy GEOJSON_COMMA point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET",
 /*  66 */ "extra_rings ::=",
 /*  67 */ "extra_rings ::= GEOJSON_COMMA ring extra_rings",
 /*  68 */ "ringz ::= GEOJSON_OPEN_BRACKET point_coordxyz GEOJSON_COMMA point_coordxyz GEOJSON_COMMA point_coordxyz GEOJSON_COMMA point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET",
 /*  69 */ "extra_ringsz ::=",
 /*  70 */ "extra_ringsz ::= GEOJSON_COMMA ringz extra_ringsz",
 /*  71 */ "multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE",
 /*  72 */ "multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE",
 /*  73 */ "multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE",
 /*  74 */ "multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE",
 /*  75 */ "multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE",
 /*  76 */ "multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE",
 /*  77 */ "multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE",
 /*  78 */ "multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE",
 /*  79 */ "multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE",
 /*  80 */ "multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE",
 /*  81 */ "multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE",
 /*  82 */ "multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE",
 /*  83 */ "multipoint_text ::= GEOJSON_OPEN_BRACKET point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET",
 /*  84 */ "multipoint_textz ::= GEOJSON_OPEN_BRACKET point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET",
 /*  85 */ "multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE",
 /*  86 */ "multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE",
 /*  87 */ "multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE",
 /*  88 */ "multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE",
 /*  89 */ "multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE",
 /*  90 */ "multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE",
 /*  91 */ "multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE",
 /*  92 */ "multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE",
 /*  93 */ "multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE",
 /*  94 */ "multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE",
 /*  95 */ "multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE",
 /*  96 */ "multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE",
 /*  97 */ "multilinestring_text ::= GEOJSON_OPEN_BRACKET linestring_text multilinestring_text2 GEOJSON_CLOSE_BRACKET",
 /*  98 */ "multilinestring_text2 ::=",
 /*  99 */ "multilinestring_text2 ::= GEOJSON_COMMA linestring_text multilinestring_text2",
 /* 100 */ "multilinestring_textz ::= GEOJSON_OPEN_BRACKET linestring_textz multilinestring_textz2 GEOJSON_CLOSE_BRACKET",
 /* 101 */ "multilinestring_textz2 ::=",
 /* 102 */ "multilinestring_textz2 ::= GEOJSON_COMMA linestring_textz multilinestring_textz2",
 /* 103 */ "multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE",
 /* 104 */ "multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE",
 /* 105 */ "multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE",
 /* 106 */ "multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE",
 /* 107 */ "multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE",
 /* 108 */ "multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE",
 /* 109 */ "multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE",
 /* 110 */ "multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE",
 /* 111 */ "multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE",
 /* 112 */ "multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE",
 /* 113 */ "multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE",
 /* 114 */ "multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE",
 /* 115 */ "multipolygon_text ::= GEOJSON_OPEN_BRACKET polygon_text multipolygon_text2 GEOJSON_CLOSE_BRACKET",
 /* 116 */ "multipolygon_text2 ::=",
 /* 117 */ "multipolygon_text2 ::= GEOJSON_COMMA polygon_text multipolygon_text2",
 /* 118 */ "multipolygon_textz ::= GEOJSON_OPEN_BRACKET polygon_textz multipolygon_textz2 GEOJSON_CLOSE_BRACKET",
 /* 119 */ "multipolygon_textz2 ::=",
 /* 120 */ "multipolygon_textz2 ::= GEOJSON_COMMA polygon_textz multipolygon_textz2",
 /* 121 */ "geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE",
 /* 122 */ "geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE",
 /* 123 */ "geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE",
 /* 124 */ "geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE",
 /* 125 */ "geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE",
 /* 126 */ "geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE",
 /* 127 */ "geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE",
 /* 128 */ "geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE",
 /* 129 */ "geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE",
 /* 130 */ "geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE",
 /* 131 */ "geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE",
 /* 132 */ "geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE",
 /* 133 */ "geocoll_text ::= GEOJSON_OPEN_BRACKET coll_point geocoll_text2 GEOJSON_CLOSE_BRACKET",
 /* 134 */ "geocoll_text ::= GEOJSON_OPEN_BRACKET coll_linestring geocoll_text2 GEOJSON_CLOSE_BRACKET",
 /* 135 */ "geocoll_text ::= GEOJSON_OPEN_BRACKET coll_polygon geocoll_text2 GEOJSON_CLOSE_BRACKET",
 /* 136 */ "geocoll_text2 ::=",
 /* 137 */ "geocoll_text2 ::= GEOJSON_COMMA coll_point geocoll_text2",
 /* 138 */ "geocoll_text2 ::= GEOJSON_COMMA coll_linestring geocoll_text2",
 /* 139 */ "geocoll_text2 ::= GEOJSON_COMMA coll_polygon geocoll_text2",
 /* 140 */ "geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_pointz geocoll_textz2 GEOJSON_CLOSE_BRACKET",
 /* 141 */ "geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_linestringz geocoll_textz2 GEOJSON_CLOSE_BRACKET",
 /* 142 */ "geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_polygonz geocoll_textz2 GEOJSON_CLOSE_BRACKET",
 /* 143 */ "geocoll_textz2 ::=",
 /* 144 */ "geocoll_textz2 ::= GEOJSON_COMMA coll_pointz geocoll_textz2",
 /* 145 */ "geocoll_textz2 ::= GEOJSON_COMMA coll_linestringz geocoll_textz2",
 /* 146 */ "geocoll_textz2 ::= GEOJSON_COMMA coll_polygonz geocoll_textz2",
 /* 147 */ "coll_point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE",
 /* 148 */ "coll_pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE",
 /* 149 */ "coll_linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE",
 /* 150 */ "coll_linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE",
 /* 151 */ "coll_polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE",
 /* 152 */ "coll_polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE",
 /* 153 */ "main ::= in",
 /* 154 */ "in ::=",
 /* 155 */ "in ::= in state GEOJSON_NEWLINE",
 /* 156 */ "state ::= program",
 /* 157 */ "program ::= geo_text",
 /* 158 */ "bbox ::= coord GEOJSON_COMMA coord GEOJSON_COMMA coord GEOJSON_COMMA coord",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p){
  int newSize;
  int idx;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;
  if( p->yystack==&p->yystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->yystk0;
  }else{
    pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              yyTracePrompt, p->yystksz, newSize);
    }
#endif
    p->yystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
*/
void ParseInit(void *yypParser){
  yyParser *pParser = (yyParser*)yypParser;
#ifdef YYTRACKMAXSTACKDEPTH
  pParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH<=0
  pParser->yytos = NULL;
  pParser->yystack = NULL;
  pParser->yystksz = 0;
  if( yyGrowStack(pParser) ){
    pParser->yystack = &pParser->yystk0;
    pParser->yystksz = 1;
  }
#endif
#ifndef YYNOERRORRECOVERY
  pParser->yyerrcnt = -1;
#endif
  pParser->yytos = pParser->yystack;
  pParser->yystack[0].stateno = 0;
  pParser->yystack[0].major = 0;
#if YYSTACKDEPTH>0
  pParser->yystackEnd = &pParser->yystack[YYSTACKDEPTH-1];
#endif
}

#ifndef Parse_ENGINEALWAYSONSTACK
/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ) ParseInit(pParser);
  return pParser;
}
#endif /* Parse_ENGINEALWAYSONSTACK */


/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yytos!=0 );
  assert( pParser->yytos > pParser->yystack );
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/*
** Clear all secondary memory allocations from the parser
*/
void ParseFinalize(void *p){
  yyParser *pParser = (yyParser*)p;
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
}

#ifndef Parse_ENGINEALWAYSONSTACK
/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
#ifndef YYPARSEFREENEVERNULL
  if( p==0 ) return;
#endif
  ParseFinalize(p);
  (*freeProc)(p);
}
#endif /* Parse_ENGINEALWAYSONSTACK */

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyhwm;
}
#endif

/* This array of booleans keeps track of the parser statement
** coverage.  The element yycoverage[X][Y] is set when the parser
** is in state X and has a lookahead token Y.  In a well-tested
** systems, every element of this matrix should end up being set.
*/
#if defined(YYCOVERAGE)
static unsigned char yycoverage[YYNSTATE][YYNTOKEN];
#endif

/*
** Write into out a description of every state/lookahead combination that
**
**   (1)  has not been used by the parser, and
**   (2)  is not a syntax error.
**
** Return the number of missed state/lookahead combinations.
*/
#if defined(YYCOVERAGE)
int ParseCoverage(FILE *out){
  int stateno, iLookAhead, i;
  int nMissed = 0;
  for(stateno=0; stateno<YYNSTATE; stateno++){
    i = yy_shift_ofst[stateno];
    for(iLookAhead=0; iLookAhead<YYNTOKEN; iLookAhead++){
      if( yy_lookahead[i+iLookAhead]!=iLookAhead ) continue;
      if( yycoverage[stateno][iLookAhead]==0 ) nMissed++;
      if( out ){
        fprintf(out,"State %d lookahead %s %s\n", stateno,
                yyTokenName[iLookAhead],
                yycoverage[stateno][iLookAhead] ? "ok" : "missed");
      }
    }
  }
  return nMissed;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yytos->stateno;
 
  if( stateno>YY_MAX_SHIFT ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
#if defined(YYCOVERAGE)
  yycoverage[stateno][iLookAhead] = 1;
#endif
  do{
    i = yy_shift_ofst[stateno];
    assert( i>=0 );
    assert( i+YYNTOKEN<=(int)sizeof(yy_lookahead)/sizeof(yy_lookahead[0]) );
    assert( iLookAhead!=YYNOCODE );
    assert( iLookAhead < YYNTOKEN );
    i += iLookAhead;
    if( yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD && iLookAhead>0
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead],
               yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
      return yy_default[stateno];
    }else{
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   ParseARG_FETCH;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/

     spatialite_e( "Giving up.  Parser stack overflow\n");
/******** End %stack_overflow code ********************************************/
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState, const char *zTag){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%s%s '%s', go to state %d\n",
         yyTracePrompt, zTag, yyTokenName[yypParser->yytos->major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%s%s '%s', pending reduce %d\n",
         yyTracePrompt, zTag, yyTokenName[yypParser->yytos->major],
         yyNewState - YY_MIN_REDUCE);
    }
  }
}
#else
# define yyTraceShift(X,Y,Z)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  ParseTOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yytos>yypParser->yystackEnd ){
    yypParser->yytos--;
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yypParser->yytos--;
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor.yy0 = yyMinor;
  yyTraceShift(yypParser, yyNewState, "Shift");
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;       /* Symbol on the left-hand side of the rule */
  signed char nrhs;     /* Negative of the number of RHS symbols in the rule */
} yyRuleInfo[] = {
  {   30,   -1 }, /* (0) geo_text ::= point */
  {   30,   -1 }, /* (1) geo_text ::= pointz */
  {   30,   -1 }, /* (2) geo_text ::= linestring */
  {   30,   -1 }, /* (3) geo_text ::= linestringz */
  {   30,   -1 }, /* (4) geo_text ::= polygon */
  {   30,   -1 }, /* (5) geo_text ::= polygonz */
  {   30,   -1 }, /* (6) geo_text ::= multipoint */
  {   30,   -1 }, /* (7) geo_text ::= multipointz */
  {   30,   -1 }, /* (8) geo_text ::= multilinestring */
  {   30,   -1 }, /* (9) geo_text ::= multilinestringz */
  {   30,   -1 }, /* (10) geo_text ::= multipolygon */
  {   30,   -1 }, /* (11) geo_text ::= multipolygonz */
  {   30,   -1 }, /* (12) geo_text ::= geocoll */
  {   30,   -1 }, /* (13) geo_text ::= geocollz */
  {   31,   -9 }, /* (14) point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
  {   31,  -15 }, /* (15) point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
  {   31,  -13 }, /* (16) point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
  {   31,  -13 }, /* (17) point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
  {   31,  -19 }, /* (18) point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
  {   31,  -19 }, /* (19) point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
  {   32,   -9 }, /* (20) pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */
  {   32,  -15 }, /* (21) pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */
  {   32,  -13 }, /* (22) pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */
  {   32,  -13 }, /* (23) pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */
  {   31,  -19 }, /* (24) point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */
  {   31,  -19 }, /* (25) point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */
  {   47,  -13 }, /* (26) short_crs ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_NAME GEOJSON_COMMA GEOJSON_PROPS GEOJSON_COLON GEOJSON_OPEN_BRACE GEOJSON_NAME GEOJSON_COLON short_srid GEOJSON_CLOSE_BRACE GEOJSON_CLOSE_BRACE */
  {   48,  -13 }, /* (27) long_crs ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_NAME GEOJSON_COMMA GEOJSON_PROPS GEOJSON_COLON GEOJSON_OPEN_BRACE GEOJSON_NAME GEOJSON_COLON long_srid GEOJSON_CLOSE_BRACE GEOJSON_CLOSE_BRACE */
  {   45,   -5 }, /* (28) point_coordxy ::= GEOJSON_OPEN_BRACKET coord GEOJSON_COMMA coord GEOJSON_CLOSE_BRACKET */
  {   49,   -7 }, /* (29) point_coordxyz ::= GEOJSON_OPEN_BRACKET coord GEOJSON_COMMA coord GEOJSON_COMMA coord GEOJSON_CLOSE_BRACKET */
  {   50,   -1 }, /* (30) coord ::= GEOJSON_NUM */
  {   51,   -1 }, /* (31) short_srid ::= GEOJSON_SHORT_SRID */
  {   52,   -1 }, /* (32) long_srid ::= GEOJSON_LONG_SRID */
  {   53,    0 }, /* (33) extra_pointsxy ::= */
  {   53,   -3 }, /* (34) extra_pointsxy ::= GEOJSON_COMMA point_coordxy extra_pointsxy */
  {   54,    0 }, /* (35) extra_pointsxyz ::= */
  {   54,   -3 }, /* (36) extra_pointsxyz ::= GEOJSON_COMMA point_coordxyz extra_pointsxyz */
  {   33,   -9 }, /* (37) linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
  {   33,  -15 }, /* (38) linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
  {   33,  -13 }, /* (39) linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
  {   33,  -13 }, /* (40) linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
  {   33,  -19 }, /* (41) linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
  {   33,  -19 }, /* (42) linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
  {   34,   -9 }, /* (43) linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */
  {   34,  -15 }, /* (44) linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */
  {   34,  -13 }, /* (45) linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */
  {   34,  -13 }, /* (46) linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */
  {   34,  -19 }, /* (47) linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */
  {   34,  -19 }, /* (48) linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */
  {   55,   -6 }, /* (49) linestring_text ::= GEOJSON_OPEN_BRACKET point_coordxy GEOJSON_COMMA point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET */
  {   56,   -6 }, /* (50) linestring_textz ::= GEOJSON_OPEN_BRACKET point_coordxyz GEOJSON_COMMA point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET */
  {   35,   -9 }, /* (51) polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
  {   35,  -15 }, /* (52) polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
  {   35,  -13 }, /* (53) polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
  {   35,  -13 }, /* (54) polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
  {   35,  -19 }, /* (55) polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
  {   35,  -19 }, /* (56) polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
  {   36,   -9 }, /* (57) polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */
  {   36,  -15 }, /* (58) polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */
  {   36,  -13 }, /* (59) polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */
  {   36,  -13 }, /* (60) polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */
  {   36,  -19 }, /* (61) polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */
  {   36,  -19 }, /* (62) polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */
  {   57,   -4 }, /* (63) polygon_text ::= GEOJSON_OPEN_BRACKET ring extra_rings GEOJSON_CLOSE_BRACKET */
  {   58,   -4 }, /* (64) polygon_textz ::= GEOJSON_OPEN_BRACKET ringz extra_ringsz GEOJSON_CLOSE_BRACKET */
  {   59,  -10 }, /* (65) ring ::= GEOJSON_OPEN_BRACKET point_coordxy GEOJSON_COMMA point_coordxy GEOJSON_COMMA point_coordxy GEOJSON_COMMA point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET */
  {   60,    0 }, /* (66) extra_rings ::= */
  {   60,   -3 }, /* (67) extra_rings ::= GEOJSON_COMMA ring extra_rings */
  {   61,  -10 }, /* (68) ringz ::= GEOJSON_OPEN_BRACKET point_coordxyz GEOJSON_COMMA point_coordxyz GEOJSON_COMMA point_coordxyz GEOJSON_COMMA point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET */
  {   62,    0 }, /* (69) extra_ringsz ::= */
  {   62,   -3 }, /* (70) extra_ringsz ::= GEOJSON_COMMA ringz extra_ringsz */
  {   37,   -9 }, /* (71) multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
  {   37,  -15 }, /* (72) multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
  {   37,  -13 }, /* (73) multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
  {   37,  -13 }, /* (74) multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
  {   37,  -19 }, /* (75) multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
  {   37,  -19 }, /* (76) multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
  {   38,   -9 }, /* (77) multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */
  {   38,  -15 }, /* (78) multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */
  {   38,  -13 }, /* (79) multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */
  {   38,  -13 }, /* (80) multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */
  {   38,  -19 }, /* (81) multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */
  {   38,  -19 }, /* (82) multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */
  {   63,   -4 }, /* (83) multipoint_text ::= GEOJSON_OPEN_BRACKET point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET */
  {   64,   -4 }, /* (84) multipoint_textz ::= GEOJSON_OPEN_BRACKET point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET */
  {   39,   -9 }, /* (85) multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */
  {   39,  -15 }, /* (86) multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */
  {   39,  -13 }, /* (87) multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */
  {   39,  -13 }, /* (88) multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */
  {   39,  -19 }, /* (89) multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */
  {   39,  -19 }, /* (90) multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */
  {   40,   -9 }, /* (91) multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */
  {   40,  -15 }, /* (92) multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */
  {   40,  -13 }, /* (93) multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */
  {   40,  -13 }, /* (94) multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */
  {   40,  -19 }, /* (95) multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */
  {   40,  -19 }, /* (96) multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */
  {   65,   -4 }, /* (97) multilinestring_text ::= GEOJSON_OPEN_BRACKET linestring_text multilinestring_text2 GEOJSON_CLOSE_BRACKET */
  {   67,    0 }, /* (98) multilinestring_text2 ::= */
  {   67,   -3 }, /* (99) multilinestring_text2 ::= GEOJSON_COMMA linestring_text multilinestring_text2 */
  {   66,   -4 }, /* (100) multilinestring_textz ::= GEOJSON_OPEN_BRACKET linestring_textz multilinestring_textz2 GEOJSON_CLOSE_BRACKET */
  {   68,    0 }, /* (101) multilinestring_textz2 ::= */
  {   68,   -3 }, /* (102) multilinestring_textz2 ::= GEOJSON_COMMA linestring_textz multilinestring_textz2 */
  {   41,   -9 }, /* (103) multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */
  {   41,  -15 }, /* (104) multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */
  {   41,  -13 }, /* (105) multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */
  {   41,  -13 }, /* (106) multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */
  {   41,  -19 }, /* (107) multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */
  {   41,  -19 }, /* (108) multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */
  {   42,   -9 }, /* (109) multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */
  {   42,  -15 }, /* (110) multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */
  {   42,  -13 }, /* (111) multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */
  {   42,  -13 }, /* (112) multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */
  {   42,  -19 }, /* (113) multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */
  {   42,  -19 }, /* (114) multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */
  {   69,   -4 }, /* (115) multipolygon_text ::= GEOJSON_OPEN_BRACKET polygon_text multipolygon_text2 GEOJSON_CLOSE_BRACKET */
  {   71,    0 }, /* (116) multipolygon_text2 ::= */
  {   71,   -3 }, /* (117) multipolygon_text2 ::= GEOJSON_COMMA polygon_text multipolygon_text2 */
  {   70,   -4 }, /* (118) multipolygon_textz ::= GEOJSON_OPEN_BRACKET polygon_textz multipolygon_textz2 GEOJSON_CLOSE_BRACKET */
  {   72,    0 }, /* (119) multipolygon_textz2 ::= */
  {   72,   -3 }, /* (120) multipolygon_textz2 ::= GEOJSON_COMMA polygon_textz multipolygon_textz2 */
  {   43,   -9 }, /* (121) geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */
  {   43,  -15 }, /* (122) geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */
  {   43,  -13 }, /* (123) geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */
  {   43,  -13 }, /* (124) geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */
  {   43,  -19 }, /* (125) geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */
  {   43,  -19 }, /* (126) geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */
  {   43,   -9 }, /* (127) geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */
  {   44,  -15 }, /* (128) geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */
  {   44,  -13 }, /* (129) geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */
  {   44,  -13 }, /* (130) geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */
  {   44,  -19 }, /* (131) geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */
  {   44,  -19 }, /* (132) geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */
  {   73,   -4 }, /* (133) geocoll_text ::= GEOJSON_OPEN_BRACKET coll_point geocoll_text2 GEOJSON_CLOSE_BRACKET */
  {   73,   -4 }, /* (134) geocoll_text ::= GEOJSON_OPEN_BRACKET coll_linestring geocoll_text2 GEOJSON_CLOSE_BRACKET */
  {   73,   -4 }, /* (135) geocoll_text ::= GEOJSON_OPEN_BRACKET coll_polygon geocoll_text2 GEOJSON_CLOSE_BRACKET */
  {   76,    0 }, /* (136) geocoll_text2 ::= */
  {   76,   -3 }, /* (137) geocoll_text2 ::= GEOJSON_COMMA coll_point geocoll_text2 */
  {   76,   -3 }, /* (138) geocoll_text2 ::= GEOJSON_COMMA coll_linestring geocoll_text2 */
  {   76,   -3 }, /* (139) geocoll_text2 ::= GEOJSON_COMMA coll_polygon geocoll_text2 */
  {   74,   -4 }, /* (140) geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_pointz geocoll_textz2 GEOJSON_CLOSE_BRACKET */
  {   74,   -4 }, /* (141) geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_linestringz geocoll_textz2 GEOJSON_CLOSE_BRACKET */
  {   74,   -4 }, /* (142) geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_polygonz geocoll_textz2 GEOJSON_CLOSE_BRACKET */
  {   80,    0 }, /* (143) geocoll_textz2 ::= */
  {   80,   -3 }, /* (144) geocoll_textz2 ::= GEOJSON_COMMA coll_pointz geocoll_textz2 */
  {   80,   -3 }, /* (145) geocoll_textz2 ::= GEOJSON_COMMA coll_linestringz geocoll_textz2 */
  {   80,   -3 }, /* (146) geocoll_textz2 ::= GEOJSON_COMMA coll_polygonz geocoll_textz2 */
  {   75,   -9 }, /* (147) coll_point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
  {   79,   -9 }, /* (148) coll_pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */
  {   77,   -9 }, /* (149) coll_linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
  {   81,   -9 }, /* (150) coll_linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */
  {   78,   -9 }, /* (151) coll_polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
  {   82,   -9 }, /* (152) coll_polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */
  {   26,   -1 }, /* (153) main ::= in */
  {   27,    0 }, /* (154) in ::= */
  {   27,   -3 }, /* (155) in ::= in state GEOJSON_NEWLINE */
  {   28,   -1 }, /* (156) state ::= program */
  {   29,   -1 }, /* (157) program ::= geo_text */
  {   46,   -7 }, /* (158) bbox ::= coord GEOJSON_COMMA coord GEOJSON_COMMA coord GEOJSON_COMMA coord */
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
**
** The yyLookahead and yyLookaheadToken parameters provide reduce actions
** access to the lookahead token (if any).  The yyLookahead will be YYNOCODE
** if the lookahead token has already been consumed.  As this procedure is
** only called from one place, optimizing compilers will in-line it, which
** means that the extra parameters have no performance impact.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno,       /* Number of the rule by which to reduce */
  int yyLookahead,             /* Lookahead token, or YYNOCODE if none */
  ParseTOKENTYPE yyLookaheadToken  /* Value of the lookahead token */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  (void)yyLookahead;
  (void)yyLookaheadToken;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfo[yyruleno].nrhs;
    if( yysize ){
      fprintf(yyTraceFILE, "%sReduce %d [%s], go to state %d.\n",
        yyTracePrompt,
        yyruleno, yyRuleName[yyruleno], yymsp[yysize].stateno);
    }else{
      fprintf(yyTraceFILE, "%sReduce %d [%s].\n",
        yyTracePrompt, yyruleno, yyRuleName[yyruleno]);
    }
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfo[yyruleno].nrhs==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
      yypParser->yyhwm++;
      assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH>0 
    if( yypParser->yytos>=yypParser->yystackEnd ){
      yyStackOverflow(yypParser);
      return;
    }
#else
    if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz-1] ){
      if( yyGrowStack(yypParser) ){
        yyStackOverflow(yypParser);
        return;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        YYMINORTYPE yylhsminor;
      case 0: /* geo_text ::= point */
      case 1: /* geo_text ::= pointz */ yytestcase(yyruleno==1);
      case 2: /* geo_text ::= linestring */ yytestcase(yyruleno==2);
      case 3: /* geo_text ::= linestringz */ yytestcase(yyruleno==3);
      case 4: /* geo_text ::= polygon */ yytestcase(yyruleno==4);
      case 5: /* geo_text ::= polygonz */ yytestcase(yyruleno==5);
      case 6: /* geo_text ::= multipoint */ yytestcase(yyruleno==6);
      case 7: /* geo_text ::= multipointz */ yytestcase(yyruleno==7);
      case 8: /* geo_text ::= multilinestring */ yytestcase(yyruleno==8);
      case 9: /* geo_text ::= multilinestringz */ yytestcase(yyruleno==9);
      case 10: /* geo_text ::= multipolygon */ yytestcase(yyruleno==10);
      case 11: /* geo_text ::= multipolygonz */ yytestcase(yyruleno==11);
      case 12: /* geo_text ::= geocoll */ yytestcase(yyruleno==12);
      case 13: /* geo_text ::= geocollz */ yytestcase(yyruleno==13);
{ p_data->result = yymsp[0].minor.yy0; }
        break;
      case 14: /* point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
      case 20: /* pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==20);
{ yymsp[-8].minor.yy0 = geoJSON_buildGeomFromPoint( p_data, (gaiaPointPtr)yymsp[-1].minor.yy0); }
        break;
      case 15: /* point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
      case 21: /* pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==21);
{ yymsp[-14].minor.yy0 = geoJSON_buildGeomFromPoint( p_data, (gaiaPointPtr)yymsp[-1].minor.yy0); }
        break;
      case 16: /* point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
      case 17: /* point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==17);
      case 22: /* pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==22);
      case 23: /* pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==23);
{ yymsp[-12].minor.yy0 = geoJSON_buildGeomFromPointSrid( p_data, (gaiaPointPtr)yymsp[-1].minor.yy0, (int *)yymsp[-5].minor.yy0); }
        break;
      case 18: /* point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
      case 19: /* point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==19);
      case 24: /* point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==24);
      case 25: /* point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==25);
{ yymsp[-18].minor.yy0 = geoJSON_buildGeomFromPointSrid( p_data, (gaiaPointPtr)yymsp[-1].minor.yy0, (int *)yymsp[-11].minor.yy0); }
        break;
      case 26: /* short_crs ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_NAME GEOJSON_COMMA GEOJSON_PROPS GEOJSON_COLON GEOJSON_OPEN_BRACE GEOJSON_NAME GEOJSON_COLON short_srid GEOJSON_CLOSE_BRACE GEOJSON_CLOSE_BRACE */
      case 27: /* long_crs ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_NAME GEOJSON_COMMA GEOJSON_PROPS GEOJSON_COLON GEOJSON_OPEN_BRACE GEOJSON_NAME GEOJSON_COLON long_srid GEOJSON_CLOSE_BRACE GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==27);
{ yymsp[-12].minor.yy0 = yymsp[-2].minor.yy0; }
        break;
      case 28: /* point_coordxy ::= GEOJSON_OPEN_BRACKET coord GEOJSON_COMMA coord GEOJSON_CLOSE_BRACKET */
{ yymsp[-4].minor.yy0 = (void *) geoJSON_point_xy( p_data, (double *)yymsp[-3].minor.yy0, (double *)yymsp[-1].minor.yy0); }
        break;
      case 29: /* point_coordxyz ::= GEOJSON_OPEN_BRACKET coord GEOJSON_COMMA coord GEOJSON_COMMA coord GEOJSON_CLOSE_BRACKET */
{ yymsp[-6].minor.yy0 = (void *) geoJSON_point_xyz( p_data, (double *)yymsp[-5].minor.yy0, (double *)yymsp[-3].minor.yy0, (double *)yymsp[-1].minor.yy0); }
        break;
      case 30: /* coord ::= GEOJSON_NUM */
      case 31: /* short_srid ::= GEOJSON_SHORT_SRID */ yytestcase(yyruleno==31);
      case 32: /* long_srid ::= GEOJSON_LONG_SRID */ yytestcase(yyruleno==32);
{ yylhsminor.yy0 = yymsp[0].minor.yy0; }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 33: /* extra_pointsxy ::= */
      case 35: /* extra_pointsxyz ::= */ yytestcase(yyruleno==35);
      case 66: /* extra_rings ::= */ yytestcase(yyruleno==66);
      case 69: /* extra_ringsz ::= */ yytestcase(yyruleno==69);
      case 98: /* multilinestring_text2 ::= */ yytestcase(yyruleno==98);
      case 101: /* multilinestring_textz2 ::= */ yytestcase(yyruleno==101);
      case 116: /* multipolygon_text2 ::= */ yytestcase(yyruleno==116);
      case 119: /* multipolygon_textz2 ::= */ yytestcase(yyruleno==119);
      case 136: /* geocoll_text2 ::= */ yytestcase(yyruleno==136);
      case 143: /* geocoll_textz2 ::= */ yytestcase(yyruleno==143);
{ yymsp[1].minor.yy0 = NULL; }
        break;
      case 34: /* extra_pointsxy ::= GEOJSON_COMMA point_coordxy extra_pointsxy */
      case 36: /* extra_pointsxyz ::= GEOJSON_COMMA point_coordxyz extra_pointsxyz */ yytestcase(yyruleno==36);
{ ((gaiaPointPtr)yymsp[-1].minor.yy0)->Next = (gaiaPointPtr)yymsp[0].minor.yy0;  yymsp[-2].minor.yy0 = yymsp[-1].minor.yy0; }
        break;
      case 37: /* linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
      case 43: /* linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==43);
{ yymsp[-8].minor.yy0 = geoJSON_buildGeomFromLinestring( p_data, (gaiaLinestringPtr)yymsp[-1].minor.yy0); }
        break;
      case 38: /* linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
      case 44: /* linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==44);
{ yymsp[-14].minor.yy0 = geoJSON_buildGeomFromLinestring( p_data, (gaiaLinestringPtr)yymsp[-1].minor.yy0); }
        break;
      case 39: /* linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
      case 40: /* linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==40);
      case 45: /* linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==45);
      case 46: /* linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==46);
{ yymsp[-12].minor.yy0 = geoJSON_buildGeomFromLinestringSrid( p_data, (gaiaLinestringPtr)yymsp[-1].minor.yy0, (int *)yymsp[-5].minor.yy0); }
        break;
      case 41: /* linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
      case 42: /* linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==42);
      case 47: /* linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==47);
      case 48: /* linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==48);
{ yymsp[-18].minor.yy0 = geoJSON_buildGeomFromLinestringSrid( p_data, (gaiaLinestringPtr)yymsp[-1].minor.yy0, (int *)yymsp[-11].minor.yy0); }
        break;
      case 49: /* linestring_text ::= GEOJSON_OPEN_BRACKET point_coordxy GEOJSON_COMMA point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET */
{ 
	   ((gaiaPointPtr)yymsp[-2].minor.yy0)->Next = (gaiaPointPtr)yymsp[-1].minor.yy0; 
	   ((gaiaPointPtr)yymsp[-4].minor.yy0)->Next = (gaiaPointPtr)yymsp[-2].minor.yy0;
	   yymsp[-5].minor.yy0 = (void *) geoJSON_linestring_xy( p_data, (gaiaPointPtr)yymsp[-4].minor.yy0);
	}
        break;
      case 50: /* linestring_textz ::= GEOJSON_OPEN_BRACKET point_coordxyz GEOJSON_COMMA point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET */
{ 
	   ((gaiaPointPtr)yymsp[-2].minor.yy0)->Next = (gaiaPointPtr)yymsp[-1].minor.yy0; 
	   ((gaiaPointPtr)yymsp[-4].minor.yy0)->Next = (gaiaPointPtr)yymsp[-2].minor.yy0;
	   yymsp[-5].minor.yy0 = (void *) geoJSON_linestring_xyz( p_data, (gaiaPointPtr)yymsp[-4].minor.yy0);
	}
        break;
      case 51: /* polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
      case 57: /* polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==57);
{ yymsp[-8].minor.yy0 = geoJSON_buildGeomFromPolygon( p_data, (gaiaPolygonPtr)yymsp[-1].minor.yy0); }
        break;
      case 52: /* polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
      case 58: /* polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==58);
{ yymsp[-14].minor.yy0 = geoJSON_buildGeomFromPolygon( p_data, (gaiaPolygonPtr)yymsp[-1].minor.yy0); }
        break;
      case 53: /* polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
      case 54: /* polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==54);
      case 59: /* polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==59);
      case 60: /* polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==60);
{ yymsp[-12].minor.yy0 = geoJSON_buildGeomFromPolygonSrid( p_data, (gaiaPolygonPtr)yymsp[-1].minor.yy0, (int *)yymsp[-5].minor.yy0); }
        break;
      case 55: /* polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
      case 56: /* polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==56);
      case 61: /* polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==61);
      case 62: /* polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==62);
{ yymsp[-18].minor.yy0 = geoJSON_buildGeomFromPolygonSrid( p_data, (gaiaPolygonPtr)yymsp[-1].minor.yy0, (int *)yymsp[-11].minor.yy0); }
        break;
      case 63: /* polygon_text ::= GEOJSON_OPEN_BRACKET ring extra_rings GEOJSON_CLOSE_BRACKET */
{ 
		((gaiaRingPtr)yymsp[-2].minor.yy0)->Next = (gaiaRingPtr)yymsp[-1].minor.yy0;
		yymsp[-3].minor.yy0 = (void *) geoJSON_polygon_xy(p_data, (gaiaRingPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 64: /* polygon_textz ::= GEOJSON_OPEN_BRACKET ringz extra_ringsz GEOJSON_CLOSE_BRACKET */
{  
		((gaiaRingPtr)yymsp[-2].minor.yy0)->Next = (gaiaRingPtr)yymsp[-1].minor.yy0;
		yymsp[-3].minor.yy0 = (void *) geoJSON_polygon_xyz(p_data, (gaiaRingPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 65: /* ring ::= GEOJSON_OPEN_BRACKET point_coordxy GEOJSON_COMMA point_coordxy GEOJSON_COMMA point_coordxy GEOJSON_COMMA point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET */
{
		((gaiaPointPtr)yymsp[-8].minor.yy0)->Next = (gaiaPointPtr)yymsp[-6].minor.yy0; 
		((gaiaPointPtr)yymsp[-6].minor.yy0)->Next = (gaiaPointPtr)yymsp[-4].minor.yy0;
		((gaiaPointPtr)yymsp[-4].minor.yy0)->Next = (gaiaPointPtr)yymsp[-2].minor.yy0; 
		((gaiaPointPtr)yymsp[-2].minor.yy0)->Next = (gaiaPointPtr)yymsp[-1].minor.yy0;
		yymsp[-9].minor.yy0 = (void *) geoJSON_ring_xy(p_data, (gaiaPointPtr)yymsp[-8].minor.yy0);
	}
        break;
      case 67: /* extra_rings ::= GEOJSON_COMMA ring extra_rings */
      case 70: /* extra_ringsz ::= GEOJSON_COMMA ringz extra_ringsz */ yytestcase(yyruleno==70);
{
		((gaiaRingPtr)yymsp[-1].minor.yy0)->Next = (gaiaRingPtr)yymsp[0].minor.yy0;
		yymsp[-2].minor.yy0 = yymsp[-1].minor.yy0;
	}
        break;
      case 68: /* ringz ::= GEOJSON_OPEN_BRACKET point_coordxyz GEOJSON_COMMA point_coordxyz GEOJSON_COMMA point_coordxyz GEOJSON_COMMA point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET */
{
		((gaiaPointPtr)yymsp[-8].minor.yy0)->Next = (gaiaPointPtr)yymsp[-6].minor.yy0; 
		((gaiaPointPtr)yymsp[-6].minor.yy0)->Next = (gaiaPointPtr)yymsp[-4].minor.yy0;
		((gaiaPointPtr)yymsp[-4].minor.yy0)->Next = (gaiaPointPtr)yymsp[-2].minor.yy0; 
		((gaiaPointPtr)yymsp[-2].minor.yy0)->Next = (gaiaPointPtr)yymsp[-1].minor.yy0;
		yymsp[-9].minor.yy0 = (void *) geoJSON_ring_xyz(p_data, (gaiaPointPtr)yymsp[-8].minor.yy0);
	}
        break;
      case 71: /* multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
      case 77: /* multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==77);
      case 85: /* multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==85);
      case 91: /* multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==91);
      case 103: /* multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==103);
      case 109: /* multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==109);
      case 121: /* geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==121);
      case 127: /* geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==127);
{ yymsp[-8].minor.yy0 = yymsp[-1].minor.yy0; }
        break;
      case 72: /* multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
      case 78: /* multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==78);
      case 86: /* multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==86);
      case 92: /* multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==92);
      case 104: /* multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==104);
      case 110: /* multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==110);
      case 122: /* geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==122);
      case 128: /* geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==128);
{ yymsp[-14].minor.yy0 = yymsp[-1].minor.yy0; }
        break;
      case 73: /* multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
      case 74: /* multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==74);
      case 79: /* multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==79);
      case 80: /* multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==80);
      case 87: /* multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==87);
      case 88: /* multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==88);
      case 93: /* multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==93);
      case 94: /* multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==94);
      case 105: /* multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==105);
      case 106: /* multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==106);
      case 111: /* multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==111);
      case 112: /* multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==112);
      case 123: /* geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==123);
      case 124: /* geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==124);
      case 129: /* geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==129);
      case 130: /* geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==130);
{ yymsp[-12].minor.yy0 = (void *) geoJSON_setSrid((gaiaGeomCollPtr)yymsp[-1].minor.yy0, (int *)yymsp[-5].minor.yy0); }
        break;
      case 75: /* multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */
      case 76: /* multipoint ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==76);
      case 81: /* multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==81);
      case 82: /* multipointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOINT GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipoint_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==82);
      case 89: /* multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==89);
      case 90: /* multilinestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==90);
      case 95: /* multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==95);
      case 96: /* multilinestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTILINESTRING GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multilinestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==96);
      case 107: /* multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==107);
      case 108: /* multipolygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==108);
      case 113: /* multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==113);
      case 114: /* multipolygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_MULTIPOLYGON GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON multipolygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==114);
      case 125: /* geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==125);
      case 126: /* geocoll ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_text GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==126);
      case 131: /* geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON short_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==131);
      case 132: /* geocollz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_GEOMETRYCOLLECTION GEOJSON_COMMA GEOJSON_CRS GEOJSON_COLON long_crs GEOJSON_COMMA GEOJSON_BBOX GEOJSON_COLON GEOJSON_OPEN_BRACKET bbox GEOJSON_CLOSE_BRACKET GEOJSON_COMMA GEOJSON_GEOMS GEOJSON_COLON geocoll_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==132);
{ yymsp[-18].minor.yy0 = (void *) geoJSON_setSrid((gaiaGeomCollPtr)yymsp[-1].minor.yy0, (int *)yymsp[-11].minor.yy0); }
        break;
      case 83: /* multipoint_text ::= GEOJSON_OPEN_BRACKET point_coordxy extra_pointsxy GEOJSON_CLOSE_BRACKET */
{ 
	   ((gaiaPointPtr)yymsp[-2].minor.yy0)->Next = (gaiaPointPtr)yymsp[-1].minor.yy0; 
	   yymsp[-3].minor.yy0 = (void *) geoJSON_multipoint_xy(p_data, (gaiaPointPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 84: /* multipoint_textz ::= GEOJSON_OPEN_BRACKET point_coordxyz extra_pointsxyz GEOJSON_CLOSE_BRACKET */
{ 
	   ((gaiaPointPtr)yymsp[-2].minor.yy0)->Next = (gaiaPointPtr)yymsp[-1].minor.yy0; 
	   yymsp[-3].minor.yy0 = (void *) geoJSON_multipoint_xyz(p_data, (gaiaPointPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 97: /* multilinestring_text ::= GEOJSON_OPEN_BRACKET linestring_text multilinestring_text2 GEOJSON_CLOSE_BRACKET */
{ 
	   ((gaiaLinestringPtr)yymsp[-2].minor.yy0)->Next = (gaiaLinestringPtr)yymsp[-1].minor.yy0; 
	   yymsp[-3].minor.yy0 = (void *) geoJSON_multilinestring_xy( p_data, (gaiaLinestringPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 99: /* multilinestring_text2 ::= GEOJSON_COMMA linestring_text multilinestring_text2 */
      case 102: /* multilinestring_textz2 ::= GEOJSON_COMMA linestring_textz multilinestring_textz2 */ yytestcase(yyruleno==102);
{ ((gaiaLinestringPtr)yymsp[-1].minor.yy0)->Next = (gaiaLinestringPtr)yymsp[0].minor.yy0;  yymsp[-2].minor.yy0 = yymsp[-1].minor.yy0; }
        break;
      case 100: /* multilinestring_textz ::= GEOJSON_OPEN_BRACKET linestring_textz multilinestring_textz2 GEOJSON_CLOSE_BRACKET */
{ 
	   ((gaiaLinestringPtr)yymsp[-2].minor.yy0)->Next = (gaiaLinestringPtr)yymsp[-1].minor.yy0; 
	   yymsp[-3].minor.yy0 = (void *) geoJSON_multilinestring_xyz(p_data, (gaiaLinestringPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 115: /* multipolygon_text ::= GEOJSON_OPEN_BRACKET polygon_text multipolygon_text2 GEOJSON_CLOSE_BRACKET */
{ 
	   ((gaiaPolygonPtr)yymsp[-2].minor.yy0)->Next = (gaiaPolygonPtr)yymsp[-1].minor.yy0; 
	   yymsp[-3].minor.yy0 = (void *) geoJSON_multipolygon_xy(p_data, (gaiaPolygonPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 117: /* multipolygon_text2 ::= GEOJSON_COMMA polygon_text multipolygon_text2 */
      case 120: /* multipolygon_textz2 ::= GEOJSON_COMMA polygon_textz multipolygon_textz2 */ yytestcase(yyruleno==120);
{ ((gaiaPolygonPtr)yymsp[-1].minor.yy0)->Next = (gaiaPolygonPtr)yymsp[0].minor.yy0;  yymsp[-2].minor.yy0 = yymsp[-1].minor.yy0; }
        break;
      case 118: /* multipolygon_textz ::= GEOJSON_OPEN_BRACKET polygon_textz multipolygon_textz2 GEOJSON_CLOSE_BRACKET */
{ 
	   ((gaiaPolygonPtr)yymsp[-2].minor.yy0)->Next = (gaiaPolygonPtr)yymsp[-1].minor.yy0; 
	   yymsp[-3].minor.yy0 = (void *) geoJSON_multipolygon_xyz(p_data, (gaiaPolygonPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 133: /* geocoll_text ::= GEOJSON_OPEN_BRACKET coll_point geocoll_text2 GEOJSON_CLOSE_BRACKET */
      case 134: /* geocoll_text ::= GEOJSON_OPEN_BRACKET coll_linestring geocoll_text2 GEOJSON_CLOSE_BRACKET */ yytestcase(yyruleno==134);
      case 135: /* geocoll_text ::= GEOJSON_OPEN_BRACKET coll_polygon geocoll_text2 GEOJSON_CLOSE_BRACKET */ yytestcase(yyruleno==135);
{ 
		((gaiaGeomCollPtr)yymsp[-2].minor.yy0)->Next = (gaiaGeomCollPtr)yymsp[-1].minor.yy0;
		yymsp[-3].minor.yy0 = (void *) geoJSON_geomColl_xy(p_data, (gaiaGeomCollPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 137: /* geocoll_text2 ::= GEOJSON_COMMA coll_point geocoll_text2 */
      case 138: /* geocoll_text2 ::= GEOJSON_COMMA coll_linestring geocoll_text2 */ yytestcase(yyruleno==138);
      case 139: /* geocoll_text2 ::= GEOJSON_COMMA coll_polygon geocoll_text2 */ yytestcase(yyruleno==139);
      case 144: /* geocoll_textz2 ::= GEOJSON_COMMA coll_pointz geocoll_textz2 */ yytestcase(yyruleno==144);
      case 145: /* geocoll_textz2 ::= GEOJSON_COMMA coll_linestringz geocoll_textz2 */ yytestcase(yyruleno==145);
      case 146: /* geocoll_textz2 ::= GEOJSON_COMMA coll_polygonz geocoll_textz2 */ yytestcase(yyruleno==146);
{
		((gaiaGeomCollPtr)yymsp[-1].minor.yy0)->Next = (gaiaGeomCollPtr)yymsp[0].minor.yy0;
		yymsp[-2].minor.yy0 = yymsp[-1].minor.yy0;
	}
        break;
      case 140: /* geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_pointz geocoll_textz2 GEOJSON_CLOSE_BRACKET */
      case 141: /* geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_linestringz geocoll_textz2 GEOJSON_CLOSE_BRACKET */ yytestcase(yyruleno==141);
      case 142: /* geocoll_textz ::= GEOJSON_OPEN_BRACKET coll_polygonz geocoll_textz2 GEOJSON_CLOSE_BRACKET */ yytestcase(yyruleno==142);
{ 
		((gaiaGeomCollPtr)yymsp[-2].minor.yy0)->Next = (gaiaGeomCollPtr)yymsp[-1].minor.yy0;
		yymsp[-3].minor.yy0 = (void *) geoJSON_geomColl_xyz(p_data, (gaiaGeomCollPtr)yymsp[-2].minor.yy0);
	}
        break;
      case 147: /* coll_point ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxy GEOJSON_CLOSE_BRACE */
      case 148: /* coll_pointz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POINT GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON point_coordxyz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==148);
{ yymsp[-8].minor.yy0 = geoJSON_buildGeomFromPoint(p_data, (gaiaPointPtr)yymsp[-1].minor.yy0); }
        break;
      case 149: /* coll_linestring ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_text GEOJSON_CLOSE_BRACE */
      case 150: /* coll_linestringz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_LINESTRING GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON linestring_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==150);
{ yymsp[-8].minor.yy0 = geoJSON_buildGeomFromLinestring(p_data, (gaiaLinestringPtr)yymsp[-1].minor.yy0); }
        break;
      case 151: /* coll_polygon ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_text GEOJSON_CLOSE_BRACE */
      case 152: /* coll_polygonz ::= GEOJSON_OPEN_BRACE GEOJSON_TYPE GEOJSON_COLON GEOJSON_POLYGON GEOJSON_COMMA GEOJSON_COORDS GEOJSON_COLON polygon_textz GEOJSON_CLOSE_BRACE */ yytestcase(yyruleno==152);
{ yymsp[-8].minor.yy0 = geoJSON_buildGeomFromPolygon(p_data, (gaiaPolygonPtr)yymsp[-1].minor.yy0); }
        break;
      default:
      /* (153) main ::= in */ yytestcase(yyruleno==153);
      /* (154) in ::= */ yytestcase(yyruleno==154);
      /* (155) in ::= in state GEOJSON_NEWLINE */ yytestcase(yyruleno==155);
      /* (156) state ::= program (OPTIMIZED OUT) */ assert(yyruleno!=156);
      /* (157) program ::= geo_text (OPTIMIZED OUT) */ assert(yyruleno!=157);
      /* (158) bbox ::= coord GEOJSON_COMMA coord GEOJSON_COMMA coord GEOJSON_COMMA coord */ yytestcase(yyruleno==158);
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfo)/sizeof(yyRuleInfo[0]) );
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yyact = yy_find_reduce_action(yymsp[yysize].stateno,(YYCODETYPE)yygoto);

  /* There are no SHIFTREDUCE actions on nonterminals because the table
  ** generator has simplified them to pure REDUCE actions. */
  assert( !(yyact>YY_MAX_SHIFT && yyact<=YY_MAX_SHIFTREDUCE) );

  /* It is not possible for a REDUCE to be followed by an error */
  assert( yyact!=YY_ERROR_ACTION );

  yymsp += yysize+1;
  yypParser->yytos = yymsp;
  yymsp->stateno = (YYACTIONTYPE)yyact;
  yymsp->major = (YYCODETYPE)yygoto;
  yyTraceShift(yypParser, yyact, "... then shift");
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
/************ End %parse_failure code *****************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  ParseTOKENTYPE yyminor         /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/

/* 
** when the LEMON parser encounters an error
** then this global variable is set 
*/
	p_data->geoJson_parse_error = 1;
	p_data->result = NULL;
/************ End %syntax_error code ******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert( yypParser->yytos==yypParser->yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  unsigned int yyact;   /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  yypParser = (yyParser*)yyp;
  assert( yypParser->yytos!=0 );
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    int stateno = yypParser->yytos->stateno;
    if( stateno < YY_MIN_REDUCE ){
      fprintf(yyTraceFILE,"%sInput '%s' in state %d\n",
              yyTracePrompt,yyTokenName[yymajor],stateno);
    }else{
      fprintf(yyTraceFILE,"%sInput '%s' with pending reduce %d\n",
              yyTracePrompt,yyTokenName[yymajor],stateno-YY_MIN_REDUCE);
    }
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact >= YY_MIN_REDUCE ){
      yy_reduce(yypParser,yyact-YY_MIN_REDUCE,yymajor,yyminor);
    }else if( yyact <= YY_MAX_SHIFTREDUCE ){
      yy_shift(yypParser,yyact,yymajor,yyminor);
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      yymajor = YYNOCODE;
    }else if( yyact==YY_ACCEPT_ACTION ){
      yypParser->yytos--;
      yy_accept(yypParser);
      return;
    }else{
      assert( yyact == YY_ERROR_ACTION );
      yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yytos->major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while( yypParser->yytos >= yypParser->yystack
            && yymx != YYERRORSYMBOL
            && (yyact = yy_find_reduce_action(
                        yypParser->yytos->stateno,
                        YYERRORSYMBOL)) >= YY_MIN_REDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yytos < yypParser->yystack || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift(yypParser,yyact,YYERRORSYMBOL,yyminor);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yytos>yypParser->yystack );
#ifndef NDEBUG
  if( yyTraceFILE ){
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=&yypParser->yystack[1]; i<=yypParser->yytos; i++){
      fprintf(yyTraceFILE,"%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}
