# Makefile for pstoedit. 

BINDIR=/home/sun11b/local/sun4/bin
MANDIR=/home/sun11b/local/man

AWK=gawk


# uncommment one of the following CCC lines for Un*x like systems
# For users of GNU g++
CCC=g++ -pedantic -Wall  -Wwrite-strings -Wcast-qual -O2
#CCC=g++ -pedantic -Wall  -Wwrite-strings -Wcast-qual -g
# For Sun's SparcCompiler (You may need to change SCC4 to CC)
#CCC=SCC4.1 +w 
# CCC=CC-4.2 -xsb +w -g
# For CenterLine (You may need to change clCC to CC)
#CCC=clCC +w -ptv 

# rest of flags for Un*x systems
LINK=$(CCC)
CCFLAGS=-DDEFAULTGS=gs
LDFLAGS=-lm
INSTALL=install -m 0755
OBJEXT=o
EXEEXT=
OUTPUT=-o pstoedit
SPECIFICDRIVERS= drvtgif.$(OBJEXT)  drvfig.$(OBJEXT)  drvidraw.$(OBJEXT)  drvgnplt.$(OBJEXT) 
MAIN=cmdmain
target=pstoedit
#############################################################

## IRIX 5.3
#CCC=CC -O2
## IRIX 6.2
#CCC=CC -mips3 -n32 -O2 -OPT:fold_arith_limit=2500 -DINTERNALBOOL
#############################################################

## For RiscOS users using g++
#CCC=g++
#CCFLAGS=-Wall -Dunix -Ilibgpp: -Ilibstdc: -Ilibio:
#LDFLAGS=-rescan libgpp:o.libg++ libstdc:o.libstdc++ libio:o.libiostrea gcc:o.libgcc Unix:o.UnixLib
#LINK=link
#############################################################

# For AIX xlC users
#CCC=xlC
#CCFLAGS=-D__unix__ -g -+ -I/usr/lpp/xlC/src/nihC/aixinclude          
#LINK=$(CCC)
#############################################################

## For DEC OSF/1 with its native cxx
#
#> Ferner muss drvbase.cc mit der cxx compiler-option -define_templates
#> compiliert werden; fuer alle anderen cc-Files kann der Compiler im
#> makefile mit (wilms@astro.uni-tuebingen.de)
#>
#CCC=cxx -w
#CCFLAGS=-DDEFAULTGS=gs -define_templates
#LINK=$(CCC)
#############################################################


# uncomment for Win32 / Borland C++
#CCC=bcc32
## add -v to CCFLAGS for debugging
#CCFLAGS= -DINTERNALBOOL -P  -DDEFAULTGS=gswin32 -I.;../cd1.3 
#LDFLAGS= 
#LDLIBS= owlwf.lib bidsf.lib import32.lib cw32.lib
#OBJEXT=obj
#EXEEXT=.exe
#OUTPUT=-epstoedit
#SPECIFICDRIVERS=drvwmf.$(OBJEXT)
#MAIN=cmdmain
#target=pstoedit.exe
#############################################################

# uncomment for DOS / Turbo C++
#CCC=tcc
#CCFLAGS=-P -I. -ml -DDEFAULTGS=gs386
#LDFLAGS=
#OBJEXT=obj
#EXEEXT=.exe
#OUTPUT=-epstoedit
#SPECIFICDRIVERS=
#MAIN=cmdmain
#target=pstoedit.exe
#############################################################

# uncomment for Win32 / Visual C++
#CCC=cl
## /Ze is used in version 4.2 to enable language extensions like bool
## unfortunately this does not work as expected
## so we have to #define bool,false and true
#CCFLAGS= /Zi /MLd /W3 /I. -Dbool=BOOL -Dfalse=FALSE -Dtrue=TRUE -DDEFAULTGS=gswin32 
#FORCECPPFLAGS=/Tp
#LDFLAGS=
#OBJEXT=obj
#EXEEXT=.exe
#OUTPUT=/Fepstoedit
#SPECIFICDRIVERS=
#MAIN=cmdmain
#target=pstoedit.exe
#############################################################

# uncomment for OS2 using VisualAge C++
#CCC=icc
# # Tdp forces C++
# # Gm+ enables threads
# # Ti activates debug mode
#CCFLAGS= -I. /Tdp /Gm+ -DDEFAULTGS=gsos2
#OBJEXT=obj
#EXEEXT=.exe
#OUTPUT=-Fepstoedit
#LDFLAGS=
#DEFFILE=pstoedit.def
#SPECIFICDRIVERS=drvmet.obj
#MAIN=cmdmain
#target=pstoedit.exe
#############################################################

# uncomment for OS2 using gcc
#CCC=gcc
#CCFLAGS=-Zexe -Zmt -D__OS2__='' -DDEFAULTGS=gsos2
#OBJEXT=o
#EXEEXT=
#OUTPUT=-o pstoedit
#LDFLAGS=-Zexe -Zmt -lstdcpp 
#DEFFILE=pstoedit.def
#SPECIFICDRIVERS=drvmet.o
#MAIN=cmdmain
#target=pstoedit
#############################################################


# uncomment if you want to use the CGM driver and have installed the 
# cd-library in $CDDIR
# See readme for more details on how to get the cd-library
#CDDIR=../cd1.3
#LOCALDEFINES=-I$(CDDIR) -DWITHCGM=1
#LOCALDRIVERS=drvcgm.$(OBJEXT) cdlib.$(OBJEXT)
#############################################################

# For developers only: 
# Uncomment the following lines (LEXSOURCE= and PSHEADER=) if you  have 
# pstoedit.pro and makeedit.l and need to make some changes in these files.
#LEXSOURCE=lexyy.cpp
# This is some sort ot hack. If LEXSOURCE is not defined, the make rule below
# only relates to dummy.cpp, which is not used by the main target.
#PSHEADER=pstoedit.ph
# same comment applies to PSHEADER variable
#############################################################


default : $(target)
# No changes should be necessary below this line

.SUFFIXES: .cpp .obj .o 
.cpp.o:
	$(CCC) -c $(LOCALDEFINES) $(CCFLAGS) $(FORCECPPFLAGS) $<

.cpp.obj:
	$(CCC) /c $(LOCALDEFINES) $(CCFLAGS) $(FORCECPPFLAGS) $<

#	$(CCC) /c $(LOCALDEFINES) $(CCFLAGS) $&.cpp

HEADERS=\
	drvlwo.h \
	drvrib.h \
	drvrpl.h \
	drvtgif.h \
	drvidraw.h \
	drvmif.h \
	drvfig.h \
	drvgnplt.h \
	drvdxf.h \
	drvmet.h \
	drvpdf.h \
	drvcgm.h \
	drvjava.h \
	drvsampl.h \
	drvbase.h drvfuncs.h psfront.h pstoedit.h pstoedit.ph version.h config.h

# Standard Drivers (for all systems)
STANDARDDRIVERS=\
	drvlwo.$(OBJEXT) \
	drvrib.$(OBJEXT) \
	drvrpl.$(OBJEXT) \
	drvmif.$(OBJEXT) \
	drvdxf.$(OBJEXT) \
	drvjava.$(OBJEXT) \
	drvpdf.$(OBJEXT) 

SAMPLEDRIVER=drvsampl.$(OBJEXT) 

DRIVERS=$(STANDARDDRIVERS) $(LOCALDRIVERS) $(SAMPLEDRIVER) $(SPECIFICDRIVERS)
OBJECTS=$(MAIN).$(OBJEXT) pstoedit.$(OBJEXT) lexyy.$(OBJEXT) drvbase.$(OBJEXT) $(DRIVERS)

# this does not work with GNU make
#OBJECTS:sh =ls drv*.c | sed 's/\.c/\.$(OBJEXT)/'
#OBJECTS += lexyy.$(OBJEXT)

TESTDRIVER=drvpdf
useaslib : useaslib.$(OBJEXT) drvbase.$(OBJEXT) $(TESTDRIVER).$(OBJEXT)
	$(LINK) -o useaslib useaslib.$(OBJEXT) drvbase.$(OBJEXT) $(TESTDRIVER).$(OBJEXT)

pstoedit : $(OBJECTS) 
	$(LINK) $(OUTPUT)  $(OBJECTS) $(LDFLAGS) $(DEFFILE) 

pstoedit.pure : $(OBJECTS) 
	purify $(LINK) -o pstoedit.pure  $(CCFLAGS) $(OBJECTS) $(LDFLAGS) $(DEFFILE) 

pstoedit.proof : $(OBJECTS) 
	proof $(LINK) -o pstoedit.proof  $(CCFLAGS) $(OBJECTS) $(LDFLAGS) $(DEFFILE) 

pstoedit.exe : $(OBJECTS) 
#	echo pstoedit.$(OBJEXT) lexyy.$(OBJEXT) drvbase.$(OBJEXT) > link.rsp
# distribute the above objects among the following lines, in case
# one of the driver variable is empty
	echo $(MAIN).$(OBJEXT) pstoedit.$(OBJEXT) $(STANDARDDRIVERS)  > link.rsp
	echo lexyy.$(OBJEXT) $(LOCALDRIVERS)  >> link.rsp
	echo $(SAMPLEDRIVER)  $(SPECIFICDRIVERS)  >> link.rsp
	echo drvbase.$(OBJEXT) $(LDLIBS)  >> link.rsp
	$(CCC) $(OUTPUT)  $(LDFLAGS) $(CCFLAGS) @link.rsp
	-del link.rsp

dummy.cpp $(LEXSOURCE) : makeedit.l
	flex  -L -8 makeedit.l 
	-mv lex.yy.c lexyy.cpp
	-mv lexyy.c lexyy.cpp

clean:
	rm -f core pstoedit.pure pstoedit.proof pstoedit$(EXEEXT) *.$(OBJEXT)

test: pstoedit$(EXEEXT)
	cd ../examples;\
	../src/pstoedit -f tgif illusion.ps illusion.ps.obj;\
	tgif illusion.ps.obj;\
	../src/pstoedit -s 2 -f tgif illusion.ps illusion.ps.obj;\
	tgif illusion.ps.obj;\
	../src/pstoedit -dt -f tgif illusion.ps illusion.ps.obj;\
	tgif illusion.ps.obj;\
	../src/pstoedit -f tgif eschertr.ps eschertr.ps.obj;\
	tgif eschertr.ps.obj;\
	../src/pstoedit -f fig illusion.ps illusion.ps.fig;\
	xfig illusion.ps.fig;\
	../src/pstoedit -f fig eschertr.ps eschertr.ps.fig;\
	xfig eschertr.ps.fig;\
	../src/pstoedit -f fig spiral.ps spiral.ps.fig;\
	xfig spiral.ps.fig;\
	echo "now trying ps from groff";\
	groff -man ../src/pstoedit.man > pstoedit.man.ps;\
	../src/pstoedit -f ps pstoedit.man.ps pstoedit.man.ps.ps;\
	gs pstoedit.man.ps.ps;\
	echo "now trying MIF for Frame ";\
	../src/pstoedit -f mif illusion.ps illusion.ps.mif ;\
	../src/pstoedit -f mif eschertr.ps eschertr.ps.mif ;\
	echo "now trying RIB for RenderMan ";\
	../src/pstoedit -dt -f rib illusion.ps illusion.ps.rib ;\
	../src/pstoedit -dt -f rib eschertr.ps eschertr.ps.rib ;\
	echo "now trying LWO for LightWave 3D ";\
	../src/pstoedit -dt -f lwo illusion.ps illusion.ps.lwo ;\
	../src/pstoedit -dt -f lwo eschertr.ps eschertr.ps.lwo ;\
	echo "now trying RPL for Real3D ";\
	../src/pstoedit -dt -f rpl illusion.ps illusion.ps.rpl ;\
	../src/pstoedit -dt -f rpl eschertr.ps eschertr.ps.rpl ;\
	echo "tests done";

# if you don't have groff, try the following instead
#	troff -man -Tpsc pstoedit.man | psdit > pstoedit.man.ps

dummy.ph $(PSHEADER) : pstoedit.pro
	$(AWK) 'BEGIN { \
		print "// DO NOT CHANGE THIS FILE. THIS FILE IS GENERATED FROM pstoedit.pro "; \
		print "// You can get pstoedit.pro from the author of pstoedit"; \
		print "// pstoedit.pro contains a lot more comments and explanations than pstoedit.ph"; \
		print "const char * PS_prologue[] = "; print "{"; } \
	      /^%\/\//      { gsub(/%/,""); print $$0; next;}	\
	      /^[ \t]*%/      { next; }	\
	      /^[ \t]*$$/      { next; }	\
	      { gsub(/^[ \t]*/,""); gsub(/\\/,"\\\\"); gsub(/"/,"\\\""); print " \" " $$0 " \","; } \
	      END   { print "0"; print "};" } ' < pstoedit.pro > pstoedit.ph

install: pstoedit pstoedit.man
	$(INSTALL) pstoedit $(BINDIR)
	$(INSTALL) pstoedit.man $(MANDIR)/man1/pstoedit.1

mtest:	
	echo $(OBJECTS)
#depend: 
#	makedepend *.cpp
# DO NOT DELETE


lexyy.$(OBJEXT): lexyy.cpp drvbase.h drvfuncs.h
drvbase.$(OBJEXT): drvbase.cpp drvbase.h drvfuncs.h
pstoedit.$(OBJEXT): pstoedit.cpp $(HEADERS)
$(MAIN).$(OBJEXT): $(MAIN).cpp pstoedit.h

drvtgif.$(OBJEXT): drvtgif.cpp drvtgif.h drvbase.h drvfuncs.h
drvlwo.$(OBJEXT): drvlwo.cpp drvlwo.h drvbase.h drvfuncs.h
drvrib.$(OBJEXT): drvrib.cpp drvrib.h drvbase.h drvfuncs.h
drvrpl.$(OBJEXT): drvrpl.cpp drvrpl.h drvbase.h drvfuncs.h
drvmif.$(OBJEXT): drvmif.cpp drvmif.h drvbase.h drvfuncs.h
drvidraw.$(OBJEXT): drvidraw.cpp drvidraw.h drvbase.h drvfuncs.h
drvfig.$(OBJEXT): drvfig.cpp drvfig.h drvbase.h drvfuncs.h
drvgnplt.$(OBJEXT): drvgnplt.cpp drvgnplt.h drvbase.h drvfuncs.h
drvdxf.$(OBJEXT): drvdxf.cpp drvdxf.h drvbase.h drvfuncs.h
drvmet.$(OBJEXT): drvmet.cpp drvmet.h drvbase.h drvfuncs.h
drvpdf.$(OBJEXT): drvpdf.cpp drvpdf.h drvbase.h drvfuncs.h
drvcgm.$(OBJEXT): drvcgm.cpp drvcgm.h drvbase.h drvfuncs.h
cdlib.$(OBJEXT): cdlib.cpp 
drvsampl.$(OBJEXT): drvsampl.cpp drvbase.h drvsampl.h drvfuncs.h
drvjava.$(OBJEXT): drvjava.cpp drvbase.h drvjava.h drvfuncs.h
drvwmf.$(OBJEXT): drvwmf.cpp drvbase.h drvwmf.h drvfuncs.h
pstoedit.ph: # dummy for RiscOS GNU make's screwed dependencies
