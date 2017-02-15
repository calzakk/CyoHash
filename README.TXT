===============================================================================
CyoHash
https://github.com/calzakk/CyoHash

Copyright (c) 2009-2017, Graham Bull. All rights reserved.
===============================================================================

Version 2.3.0.0
Release Date 15th February 2017

-------------------------------------------------------------------------------
1. License
-------------------------------------------------------------------------------

CyoHash is made available under the terms of the Berkeley Software
Distribution (BSD) licence, as detailed in LICENSE.TXT. This allows you
complete freedom to use and distribute the code in source and/or binary form,
as long as you respect the original copyright.

-------------------------------------------------------------------------------
2. Building
-------------------------------------------------------------------------------

CyoHash in built using Microsoft Visual Studio 2015 (but will probably also
build with an earlier version).
The installer is built using NSIS (http://nsis.sourceforge.net/Download).

1. Build the solution:

Load the source\CyoHash.sln solution file into Visual Studio and perform a
release build of both the Win32 and x64 configurations.  This will build the
main application, the shell extension, and the installer plugin.

2. Build the installer:

After building the solution, simply right-click the installer file
(ie. install\CyoHash.nsi) and select 'Compile NSIS Script' to build the
installer executable. Alternatively, drag the .nsi file into the NSIS Compiler
(MakeNSISW).

-------------------------------------------------------------------------------
3. Usage
-------------------------------------------------------------------------------

After installing, simply access CyoHash by right-clicking a file and selecting
a hash function from the CyoHash context menu.

-------------------------------------------------------------------------------
4. Release Notes
-------------------------------------------------------------------------------

2.3.0 - 15th February 2017
- Ability to hash all files in a folder.

2.2.0 - 13th September 2015
- Ability to sort items by pathname, algorithm, or hash value.
- Ability to recalculate a file's hash; for example, if the file has changed.
- Additional Explorer option that lists the last hashing algorithm used.

2.1.0 - 19th October 2013
- Ability to highlight multiple files; context menu operates on all selected files.
- Ability to select multiple files in the browse dialog.
- Added Ctrl+A keyboard shortcut to select all files in the list.
- Added an About box.

2.0.1 - 27th April 2013
- Attempt to improve stability when hashing multiple files simultaneously.
- Added ability to drag files into the dialog.
- The dialog now handles the Delete and Menu keys.
- Hash File and Clear All now always available regardless of what's right-clicked.

2.0.0 - 13th April 2013
- New dialog for showing multiple hashes simultaneously.

1.0.11 - 15th March 2012
- Added ability to minimize the dialog.

1.0.10 - 2nd March 2012
- Added ability to pause hashing.

1.0.9 - 23rd December 2011
- Multiple files can now be selected and hashed simultaneously.
- Pasted hashes are now trimmed before they're compared to the calculated hash.
- Added VS2010 solution and projects.

1.0.8 - 30th May 2011
- Added ability to hash using SHA256, SHA384, and SHA512 algorithms
  (requires Windows XP SP3 or newer).

1.0.7 - 4th August 2010
- Added ability to paste a hash into a textbox for easy validation.
- An icon is now displayed in the Windows Explorer context menu (Vista+).

1.0.6 - 29th May 2010
- Added ability to display SHA1 hashes with a base32 encoding.
- Fixed issue in which the window isn't always topmost.

1.0.5 - 2nd March 2010
- Consolidated the separate x86 and x64 installers into one installer.

1.0.4 - 3rd February 2010
- New standalone executable that implements the windowing functionality
  instead of it all being done within the shell extension dll.

1.0.3 - 1st September 2009
- First release on sourceforge.net.
