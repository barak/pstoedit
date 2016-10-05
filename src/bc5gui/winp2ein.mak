#
# Borland C++ IDE generated makefile
# Generated 4/14/97 at 8:23:21 PM 
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
LinkerLocalOptsAtW32_winp2eindexe =  -LC:\BC45\LIB;F:\BC5\LIB -Tpe -aa -V4.0 -c
ResLocalOptsAtW32_winp2eindexe = 
BLocalOptsAtW32_winp2eindexe = 
CompInheritOptsAt_winp2eindexe = -IC:\BC45\INCLUDE;F:\BC5\INCLUDE;../;. -DSTRICT;_OWLPCH;_RTLDLL;_BIDSDLL;_OWLDLL;PSTOEDIT_INPROC;
LinkerInheritOptsAt_winp2eindexe = -x
LinkerOptsAt_winp2eindexe = $(LinkerLocalOptsAtW32_winp2eindexe)
ResOptsAt_winp2eindexe = $(ResLocalOptsAtW32_winp2eindexe)
BOptsAt_winp2eindexe = $(BLocalOptsAtW32_winp2eindexe)

#
# Dependency List
#
Dep_winp2ein = \
   winp2ein.exe

winp2ein : BccW32.cfg $(Dep_winp2ein)
  echo MakeNode

Dep_winp2eindexe = \
   drvwmf.obj\
   pstoedit.obj\
   paradlg.obj\
   drvpdf.obj\
   drvdxf.obj\
   drvjava.obj\
   drvmif.obj\
   lexyy.obj\
   drvbase.obj\
   winp2eap.obj\
   winp2edc.obj\
   winp2ead.obj\
   winp2eap.res\
   winp2eap.def

winp2ein.exe : $(Dep_winp2eindexe)
  $(TLINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_winp2eindexe) $(LinkerInheritOptsAt_winp2eindexe) +
F:\BC5\LIB\c0w32.obj+
drvwmf.obj+
pstoedit.obj+
paradlg.obj+
drvpdf.obj+
drvdxf.obj+
drvjava.obj+
drvmif.obj+
lexyy.obj+
drvbase.obj+
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

drvwmf.obj :  ..\drvwmf.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ ..\drvwmf.cpp
|

pstoedit.obj :  ..\pstoedit.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ ..\pstoedit.cpp
|

paradlg.obj :  paradlg.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ paradlg.cpp
|

drvpdf.obj :  ..\drvpdf.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ ..\drvpdf.cpp
|

drvdxf.obj :  ..\drvdxf.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ ..\drvdxf.cpp
|

drvjava.obj :  ..\drvjava.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ ..\drvjava.cpp
|

drvmif.obj :  ..\drvmif.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ ..\drvmif.cpp
|

lexyy.obj :  ..\lexyy.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ ..\lexyy.cpp
|

drvbase.obj :  ..\drvbase.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ ..\drvbase.cpp
|

Dep_winp2eapdobj = \
   winp2eap.h\
   winp2eap.cpp

winp2eap.obj : $(Dep_winp2eapdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ winp2eap.cpp
|

Dep_winp2edcdobj = \
   winp2edc.h\
   winp2edc.cpp

winp2edc.obj : $(Dep_winp2edcdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ winp2edc.cpp
|

Dep_winp2eaddobj = \
   winp2ead.h\
   winp2ead.cpp

winp2ead.obj : $(Dep_winp2eaddobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe) -o$@ winp2ead.cpp
|

Dep_winp2eapdres = \
   winp2eap.rh\
   winp2eap.rc

winp2eap.res : $(Dep_winp2eapdres)
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_winp2eindexe) $(CompInheritOptsAt_winp2eindexe)  -FO$@ winp2eap.rc
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
-k
-W
-H"owl\pch.h"
| $@


