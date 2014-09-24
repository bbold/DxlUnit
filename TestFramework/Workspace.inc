//Include for global settings in the workspace, such as base path.
/** Include for global settings in the workspace, such as base path.
\file Workspace.inc

\author Bernhard Bold
\date 23.04.2012
  
\version 1.0


\dependencies <br>

\remarks To change the absolute path faster, that is needed in 
the test files, we define it here globally. May also be used
for other globals, if necessary.

\history <br>
 23.04.2012 Created. <br>
---------------------------------------------------------------
Copyright (c) 2014, Bernhard Bold
All rights reserved.

The software is licensed under the conditions of the BSD Simplified.
For details see the license.txt file.
***************************************************************/
/// \cond 
pragma  runLim, 0;  		// no time limit
pragma encoding, "UTF-8";	//use UTF-8 encoding
/// \endcond

//don't forget the slash at the end
const string BASE_PATH = "D:/BeBold/"; 