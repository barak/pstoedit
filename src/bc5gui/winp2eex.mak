#
# Borland C++ IDE generated makefile
# Generated 4/14/97 at 6:44:35 PM 
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
TLINK32 = TLink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LinkFLAGS32 =  -LD:\WINPRG\BC5\LIB
IDE_ResFLAGS32 = 
LinkerLocalOptsAtW32_winp2eexdexe =  -LF:\BC5\LIB -Tpe -aa -V4.0 -c
ResLocalOptsAtW32_winp2eexdexe = 
BLocalOptsAtW32_winp2eexdexe = 
CompInheritOptsAt_winp2eexdexe = -IF:\BC5\INCLUDE;../;. -DSTRICT;_OWLPCH;_RTLDLL;_BIDSDLL;_OWLDLL;
LinkerInheritOptsAt_winp2eexdexe = -x
LinkerOptsAt_winp2eexdexe = $(LinkerLocalOptsAtW32_winp2eexdexe)
ResOptsAt_winp2eexdexe = $(ResLocalOptsAtW32_winp2eexdexe)
BOptsAt_winp2eexdexe = $(BLocalOptsAtW32_winp2eexdexe)

#
# Dependency List
#
Dep_winp2eex = \
   winp2eex.exe

winp2eex : BccW32.cfg $(Dep_winp2eex)
  echo MakeNode

Dep_winp2eexdexe = \
   paradlg.obj\
   winp2eap.obj\
   winp2edc.obj\
   winp2ead.obj\
   winp2eap.res\
   winp2eap.def

winp2eex.exe : $(Dep_winp2eexdexe)
  $(TLINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_winp2eexdexe) $(LinkerInheritOptsAt_winp2eexdexe) +
F:\BC5\LIB\c0w32.obj+
paradlg.obj+
winp2eap.obj+
winp2edc.obj+
winp2ead.obj
$<,$*
F:\BC5\LIB\owlwf.lib+
F:\BC5\LIB\bidsf.lib+
F:\BC5\LIB\import32.lib+
F:\BC5\LIB\cw32.lib
winp2eap.def
|
   $(BRC32) winp2eap.res $<

paradlg.obj :  paradlg.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eexdexe) $(CompInheritOptsAt_winp2eexdexe) -o$@ paradlg.cpp
|

Dep_winp2eapdobj = \
   winp2eap.h\
   winp2eap.cpp

winp2eap.obj : $(Dep_winp2eapdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eexdexe) $(CompInheritOptsAt_winp2eexdexe) -o$@ winp2eap.cpp
|

Dep_winp2edcdobj = \
   winp2edc.h\
   winp2edc.cpp

winp2edc.obj : $(Dep_winp2edcdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eexdexe) $(CompInheritOptsAt_winp2eexdexe) -o$@ winp2edc.cpp
|

Dep_winp2eaddobj = \
   winp2ead.h\
   winp2ead.cpp

winp2ead.obj : $(Dep_winp2eaddobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eexdexe) $(CompInheritOptsAt_winp2eexdexe) -o$@ winp2ead.cpp
|

Dep_winp2eapdres = \
   winp2eap.rh\
   winp2eap.rc

winp2eap.res : $(Dep_winp2eapdres)
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_winp2eexdexe) $(CompInheritOptsAt_winp2eexdexe)  -FO$@ winp2eap.rc
|
# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-w
-R
-v
-vi
-H
-H=Winp2e.csm
-y-
-N-
-H-
-Od
-Og
-O-S
-v-
-R-
-a-
-3
-ff
-VF-
-vi
-k-
-W
-H"owl\pch.h"
| $@


