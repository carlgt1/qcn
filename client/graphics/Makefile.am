include $(top_srcdir)/Makefile.incl

AUTOMAKE_OPTIONS = std-options

client-bin: @CLIENT_GUI_BIN_FILENAME@

BINARY = @CLIENT_GUI_BIN_FILENAME@_$(QCN_VERSION_STRING)_$(build)

if OS_DARWIN
   #  also note need to have the following settings in environment for Leopard and higher OS X (i.e. >=10.5)
   # DEVELOPER_SDK_DIR=/Developer/SDKs
   # LDFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch i386 $LDFLAGS"
   # CPPFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 $CPPFLAGS"
   # CFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 $CFLAGS"
   # CXXFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 $CXXFLAGS"
   # SDKROOT="/Developer/SDKs/MacOSX10.4u.sdk"
   # MACOSX_DEPLOYMENT_TARGET=10.4
   # export DEVELOPER_SDK_DIR LDFLAGS CPPFLAGS CFLAGS CXXFLAGS SDKROOT MACOSX_DEPLOYMENT_TARGET
   LIBS += -framework IOKit -framework Foundation -framework ScreenSaver -framework Cocoa -framework GLUT -framework OpenGL
   STACKSIZE = -Wl,-stack_size -Wl,2000000
else
   # LIBS += -L/usr/local/lib -lGL -lGLU -lglut
   # LIBS += /usr/local/lib/libGL.a /usr/local/lib/libGLU.a /usr/local/lib/libglut.a
   MYLDADD = -lc -lftgl -lfreetype -ljpeg -lGL -lGLU -lglut
   STACKSIZE = --stack=2000000
endif

LIBS += @CLIENTLIBS@ $(BOINC_LIB) $(MYLDADD)

bin_PROGRAMS = qcn_graphics
      
qcn_graphics_SOURCES = \
      $(UTILDIR)/qcn_signal.cpp \
      $(UTILDIR)/cserialize.cpp \
      $(UTILDIR)/gzstream.cpp \
      $(MAINDIR)/qcn_shmem.cpp \
      $(GRAPHICSDIR)/qcn_graphics.cpp \
      $(GRAPHICSDIR)/qcn_2dplot.cpp \
      $(GRAPHICSDIR)/qcn_earth.cpp \
      $(GRAPHICSDIR)/qcn_cube.cpp \
      $(GRAPHICSDIR)/nation_boundary.cpp \
      $(GRAPHICSDIR)/coastline.cpp \
      $(GRAPHICSDIR)/plate_boundary.cpp \
      $(UTILDIR)/texture.cpp \
      $(UTILDIR)/qcn_util.cpp \
      $(UTILDIR)/ttfont.cpp 

qcn_graphics_DEPENDENCIES =
qcn_graphics_CFLAGS = -DQCNGRAPHICS -DGRAPHICS_PROGRAM $(AM_CPPFLAGS) $(PTHREAD_CFLAGS)
qcn_graphics_CPPFLAGS = -DQCNGRAPHICS -DGRAPHICS_PROGRAM $(AM_CPPFLAGS) $(PTHREAD_CFLAGS)
qcn_graphics_LDFLAGS = $(LIBS) $(AM_LDFLAGS) $(STACKSIZE) $(PTHREAD_LIBS)

.PHONY: moveover

qcndir = $(BASEDIR)/bin

all-local: client-bin moveover

moveover:
	mv @CLIENT_GUI_BIN_FILENAME@ $(BINARY)
	$(STRIP) $(BINARY)
	mkdir -p $(top_srcdir)/client/test/projects/qcn.edu_qcn
	mv $(BINARY) $(top_srcdir)/client/bin/
	cp $(top_srcdir)/client/bin/$(BINARY) $(top_srcdir)/client/test/projects/qcn.edu_qcn/

## these source files need to be specified because no rule uses them.

#EXTRA_DIST = *.h \
#    mac \
#    translation \
#    win
#
clean-local:
	rm -f @CLIENT_BIN_FILENAME@ $(BINARY) $(top_srcdir)/client/bin/*qcn_graphics* $(top_srcdir)/client/test/projects/qcn.edu_qcn/*qcn_graphics* 

