This is the git repository for the Quake-Catcher Network project, Home Page:

http://quakecatcher.net

All files are (c) 2007-2018 Carl Christensen

Project formerly developed at Stanford University (2007-2014), the hosted at CalTech, now at USC via 
the Southern California Earthquake Center with support by IRIS.

Primary author of all files unless otherwise noted is Carl Christensen, carlgt1@hotmail.com  (github: carlgt1)

This repository is released under the Lesser GNU Public License (LGPL), with the hopes that it will
be useful for academic use and experimentation in seismology.

-------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the Lesser GNU General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program (e.g. COPYING.LESSER).  If not, see http://www.gnu.org/licenses/lgpl.html

---------------

You will need various dependencies, some of which are included e.g. libcurl etc, but mainly you will need to get
the BOINC libraries cloned parallel to the qcn directory:
cd ..
git clone git://boinc.berkeley.edu/boinc.git

basically the QCN client code is in the subdir "client" and the server code in "server" (duh)

In client there is a subdirectory "qcnlive" for the Qt-based (you'll need to install Qt) QCNLive program - and makefiles
for Linux and visual editor files for XCode (Mac) and Visual Studio (Windows)

precompiled libraries and the visual studio/xcode files are also in win_build & mac_build respectively
