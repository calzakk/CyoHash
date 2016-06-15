# CyoHash


## Building

CyoHash in built using Microsoft Visual Studio 2015 (but will probably also build with an earlier version). The installer is built using NSIS (http://nsis.sourceforge.net/Download).

1. Build the solution:

Load the source\CyoHash.sln solution file into Visual Studio and perform a release build of both the Win32 and x64 configurations.  This will build the main application, the shell extension, and the installer plugin.

2. Build the installer:

After building the solution, simply right-click the installer file (ie. install\CyoHash.nsi) and select 'Compile NSIS Script' to build the installer executable. Alternatively, drag the .nsi file into the NSIS Compiler (MakeNSISW).

## Usage

After installing, simply access CyoHash by right-clicking a file and selecting a hash function from the CyoHash context menu.

## License

### Simplified BSD License

All the files in this library are covered under the terms of the Berkeley Software Distribution (BSD) License:

Copyright (c) 2009-2016, Graham Bull.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

