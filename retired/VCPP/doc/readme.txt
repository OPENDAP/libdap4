
Note: This code was built on WIndows XP using libdap 3.7.10 in Feb 2008 with
only minor modifications to the VCPP/Makefile. 

James Gallagher
22 Feb 2008
--------------------------

These are the instructions that accompany each Windows release of the OPeNDAP
Core Library known as "libdap".

Introduction
------------

This is the 3.7.3 release of libdap for MS Windows XP (or later) on 32-bit
machines.  It was built using Visual C++ 8.x.  Both static and dynamic
versions of the Core libraries are provided.

You must have the prerequisites for this distribution installed before
installing this distribution.  The prerequisites are contained in the
libdap-prerequisites3.7.3.zip file available from http://opendap.org.


Purposes of this Distribution
-----------------------------

This distribution serves three purposes:

1)  The Dynamic-link libraries necessary for running executables based
    upon this version of libdap are provided.

2)  The export libraries and the associated headers that are required
    to OPeNDAP enable 3rd-party applications that are to be based upon this
    specific version of libdap are provided.  Dynamic-link libraries necessary
    to run such applications are also provided.  These are the tools that
    experienced developers require to OPeNDAP-enable their applications.

    The documentation that specifies the interface to the OPeNDAP Core Library
    is not provided with this release.  With this distriubution, an experienced
    developer should be able to link their software with libdap to OPeNDAP-enable
    it.

    See the 'sample' subdirectory for a makefile that details how to compile and
    link.  Sample code is also provided.

3)  Provides instructions for rebuilding this version of libdap from source
    code.

Contents
--------

  bin/       - Contains a basic OPeNDAP client "getdap"
  dll/       - Provides the run-time dynamic link libraries
  etc/       - Contains the 'deflate' program
  include/   - Contains the headers necessary to compile
  lib/       - Contains the libraries needed to link.  Both a static and
               dynamic version of the OPeNDAP Core library (libdapMT.lib
               and libdapMD.lib respectively) are provided.
  sample/    - Contains sample code and a sample makefile
  readme.txt - This readme file



Installation Instruction
------------------------

   1)  Install the prerequisites outlined above

   2)  Drag-n-drop the 'bin', 'dll', 'include', 'lib', 'etc' and 'sample' directories
       found in this distriubution to C:\opendap.  If prompted to over write the
       existing directory structure, choose 'Yes to all'.

   3)  Place C:\opendap\bin and C:\opendap\dll on your execution path.

   Test the Install:

   1)  Open a DOS window

   2)  Issue the following command

       C:\> getdap --help

       If the install is successful, you should see the usage message for the getdap
       utility.



Rebuilding from source code
---------------------------


  To Rebuild libdap 3.7.3 from source code using MS Visual C++ 8.x,
  the subversion command-line utility and the installed prerequisite package
  outlined above:
  
  1)  Add BISON_HAIRY environment var and set to C:\opendap\include\bison.hairy
  2)  Add BISON_SIMPLE and set to C:\opendap\include\bison.simple
  3)  Add INCLUDE environment variable if necessary and add following to it:

         C:\opendap\include
         C:\opendap\include\pthreads
         C:\opendap\include\libxml
         C:\opendap\include\curl

  4)  Add LIB environment variable if necessary and add the following to it:

         C:\opendap\lib

  
  Note: Microsoft provides a means of opening a command prompt where compiler
  command-line tools can be conveniently executed.  Under Visual Studio,
  this is available via the "Visual Studio 2005 Command Prompt" located
  underneath the start->programs menu.  In Visual C++ 8.x an identical
  mechanism is provided.  When we refer to a "command-line" window in the below
  instructions, we are referring to this Microsoft-provided compiler command
  prompt.  We are not referring to a simple DOS Command Prompt.

  Open a command-line window and:

  C:\> cd C:\opendap
  C:\> svn co http://scm.opendap.org:8090/svn/tags/libdap/3.7.3 libdap3.7.3
  C:\> cd libdap3.7.3\VCPP
  C:\> nmake grammarclean
  C:\> nmake all


December 10, 2006
Robert O. Morris



