## -*- mode: make; tab-width: 4 -*-
## $Id: Makefile.am 13842 2007-10-12 14:31:59Z fthomas $

AUTOMAKE_OPTIONS = foreign

## make sure rebuilding uses the right aclocal-switches
ACLOCAL_AMFLAGS = -I m4

API_SUBDIRS = ../boinc/api ../boinc/lib ../boinc/zip

#if ENABLE_SERVER
#   SERVER_SUBDIRS = db test py sched apps tools
#endif
#
#if ENABLE_CLIENT
#   CLIENT_SUBDIRS = client 
#endif
#
#if BUILD_CLIENTGUI
#   CLIENTGUI_SUBDIRS = clientgui sea
#endif

# ORDER MATTERS below.  One must build dependencies FIRST, then things
# that depend on them later
#SUBDIRS = main graphics
SUBDIRS = client/main client/graphics client/qcnlive
if OS_DARWIN
   SUBDIRS += client/qcnusb
else
endif

# Putting a directory name recursively copies the entire contents - the
# dist-hook below gets rid of CVS directories.

#includedir = ${prefix}/include/BOINC
include_HEADERS = \
    config.h \
    version.h

dist-hook:
	rm -rf `find $(distdir) -name .backup -prune -o -name '*~' -o -name CVS`
#	cd $(distdir) && ./_autosetup
