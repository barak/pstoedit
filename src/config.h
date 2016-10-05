#if defined(__OS2__)
// OS/2 system
// check for OS2 here because gcc on OS2 seems to define at least on of the next checked
// defines  (probably EMX)

#define WITHMET 1
#define WITHDXF 1
#define WITHMIF 1
#define WITHPDF 1
#define WITHJAVA 1
#define WITHSAMPLE 1

#elif defined(unix) || defined(__unix__) || defined(__unix) || defined(__AIX__) || defined(__EMX__)
// Un*x like system
// for using the CGM driver, see makefile
// #define WITHCGM
#define WITHDXF 1
#define WITHFIG 1
#define WITHGNUPLOT 1
#define WITHRPL 1
#define WITHRIB 1
#define WITHLWO 1
#define WITHMIF 1
#define WITHPDF 1
#define WITHSAMPLE 1
#define WITHJAVA 1
#define WITHTGIF 1
#define WITHIDRAW 1

#else
// probably DOS/Windows

#if defined (__BCPLUSPLUS__) 
// wmf only supported for Borland C++, because it uses the OWL library
#define WITHWMF 1
#endif
#define WITHDXF 1
#define WITHMIF 1
#define WITHPDF 1
#define WITHJAVA 1
#define WITHSAMPLE 1

#endif

// set all WITHxxx that are  not set so far to 0
// this needs to be done since we use the value in pstoedit.cpp
#ifndef WITHSAMPLE
#define WITHSAMPLE 0
#endif


#ifndef WITHCGM
#define WITHCGM 0
#endif
#ifndef WITHDXF
#define WITHDXF 0
#endif
#ifndef WITHFIG
#define WITHFIG 0
#endif
#ifndef WITHGNUPLOT
#define WITHGNUPLOT 0
#endif
#ifndef WITHRPL
#define WITHRPL 0
#endif
#ifndef WITHRIB
#define WITHRIB 0
#endif
#ifndef WITHLWO
#define WITHLWO 0
#endif
#ifndef WITHMIF
#define WITHMIF 0
#endif
#ifndef WITHPDF
#define WITHPDF 0
#endif
#ifndef WITHJAVA
#define WITHJAVA 0
#endif
#ifndef WITHTGIF
#define WITHTGIF 0
#endif
#ifndef WITHWMF
#define WITHWMF 0
#endif
#ifndef WITHMET 
#define WITHMET 0
#endif
#ifndef WITHIDRAW 
#define WITHIDRAW 0
#endif
