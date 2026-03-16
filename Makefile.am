ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST=LICENSE README.md autogen.sh rm_generated.sh
SUBDIRS = config src examples contrib misc othersrc QT
SUBDIRS += doc
m4datadir = $(datadir)/aclocal
# m4data_DATA = m4/pstoedit.m4
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

if !DOCS
# do not check in no docs mode since in that case we might leave some files 
distcleancheck:
	@:
endif
