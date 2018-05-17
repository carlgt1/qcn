-------------------------------------------------------------------------------

                           MotionNode Accel API

-------------------------------------------------------------------------------
                     Version 1.2, October 15, 2008
-------------------------------------------------------------------------------

1. Introduction

The MotionNode Accel API is a standalone dynamic library that provides direct
access to the MotionNode Accel sensor. The API provides the raw and
calibrated outputs of the triaxial accelerometer.

The dynamic library binaries are redistributable with your application. The
header file, import library, and source example files may not be distributed
in any form.


2. File List

MotionNodeAccelAPI.h

  Shared header file for all platforms. Includes method level documentation
  parseable by Doxygen or Javadoc.

example.cpp

  Source file with a full example of API usage.

MotionNodeAccelAPI.lib

  Import library for Visual Studio compiler.

MotionNodeAccelAPI.dll

  Dynamic library for Windows. Redistributable.

libMotionNodeAccelAPI.so

  Dynamic library for Linux (x86). Redistributable.

libMotionNodeAccelAPI.dylib

  Dynamic library for Mac OS X (Universal). Redistributable.


3. Platforms

3.1 Windows

The API is compiled and tested with Microsoft Visual Studio 2005 (MSVC8). Use
the supplied import library to link to the dynamic library.

3.2 Linux

The API is compiled and tested with GCC 4.1.3 and the APBuild package to
maximize binary compatibility with Linux variants.

3.3 Mac OS X

The API is compied and tested with Apple GCC 4.0.1. The dynamic library is
compiled as a Universal binary to maximize compatibility with Mac OS X
platforms.


4. License

(C) Copyright GLI Interactive LLC 2008. All rights reserved.

UNPUBLISHED -- Rights reserved under the copyright laws of the United States.
Use of a copyright notice is precautionary only and does not imply
publication or disclosure.

THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY INFORMATION OF
GLI INTERACTIVE LLC. ANY DUPLICATION, MODIFICATION, DISTRIBUTION, OR
DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY PROHIBITED WITHOUT
THE PRIOR EXPRESS WRITTEN PERMISSION OF GLI INTERACTIVE LLC.
