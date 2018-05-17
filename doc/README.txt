To build the QCN system you probably just need to run "./build VERSION" i.e. "./build 1.43" will build execs called qcn_1.43_i686-apple-darwin etc

To prepare for BOINC -- the files should be named "graphics_app=qcn_x.xx_osname" and "qcn_x.xx_osname" where osname is i686-apple-darwin, windows_intelx86.exe etc,
and x.xx is the BOINC version number (i.e. the number you used in build)

Also you should run the "./getboinc" script to get the latest svn version of BOINC which will be in the boinc directory (or symlink to your boinc directory).
The ./build x.xx script assumes you have already built boinc!  Typically:  
      cd boinc ; ./_autosetup ; ./configure --disable-client --disable-server

Warning: you may need to upgrade autoconf & automake & perhaps other such GNU utilities!  check ftp://ftp.gnu.org/pub
also libjpeg required for graphics

Note:  if you run build with a second argument it will get & build the boinc subdir for you!  e.g. "./build 1.43 t"
 
Windows note:
   -- unzip winlibs.zip to the root QCN directory (e.g. c:\qcn) -- this will make available
      the zlib, glut, and jpeglib directories required (these should be available or already 
      installed on Mac & Linux)

   -- run getboinc.cmd to make the boinc/ subdirectory

   -- in Visual Studio (C++) -- open up client/win_build/qcn.sln which should import the projects
      for QCN (including the BOINC projects required for building libraries etc)

   -- edit the appropriate version number in "version.h", and ensure that the executable
      name matches (executable will be built in client/test/projects/qcn.edu_qcn/)

   -- you should be able to "Rebuild Solution" for the "Debug" or "Release" version and the appropriate
      exec will be placed in client/test/projects/qcn.edu_qcn/

   -- note you can intereactively debug with the "Debug' build (note execs have "d" at the end before the .exe)

 Windows/HP laptops -- look for sensor code at http://lwn.net/Articles/204532/


