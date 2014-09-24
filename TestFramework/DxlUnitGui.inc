//<
/** GUI runner for test files.
\file DxlUnitGui.inc
\author Bernhard Bold
\date 15.03.2010
  
\version 2.8

\copyright (c) Bernhard Bold

\dependencies <br>

\history
 15.03.2010 Created .<br>
---------------------------------------------------------------
Copyright (c) 2014, Bernhard Bold
All rights reserved.

The software is licensed under the conditions of the BSD Simplified.
For details see the license.txt file.
***************************************************************/

/// \cond 
pragma encoding, "UTF-8"
/// \endcond


//test icons
Icon icoValid 	= null;
Icon icoInvalid = null;
Icon icoIgnored = null;
Icon icoNotRun 	= null;

//flag for debug info
bool gDxlDebugInfo = false;

//the dialog and controls
DB 	dlgUnitTests = null;
DBE cnvLogo		 = null;
DBE frmCopyright = null;
DBE txtCopyright = null;
DBE frmTests	 = null;
DBE dbeFile      = null;
DBE btnRefresh	 = null;
DBE chkSort		 = null;
DBE tvTests      = null;
DBE frmResults	 = null;
DBE txtResult    = null;
DBE chkLogResults = null;
DBE btnOk 		 = null;


/**************************************************************/
/** Get the starting paths from the registry, to find the
absolute path of a script.

\returns  A list with the directories in the addin environment.

\remarks Looks only in the addins path.
***************************************************************/
Skip GetStartingPaths(){
	Skip skpPaths = null;
	string envDoorsAddins = "";
	int idx = 0;
	
	Regexp parsePath = regexp "([^;]*)(;[ \t]*)*";
 
	// Set path name separators
	string sep = "\\"		// for WIN32 platforms
 
	// Get the content of the DOORSADDINS environment variable
	// and add a ";" to be on the safe side
	envDoorsAddins = getenv("DOORSADDINS");
	envDoorsAddins = envDoorsAddins ";";
	if (!null(envDoorsAddins)){
		// DOORSADDINS specifies one or more addins locations
		skpPaths = create();
		while(!null(envDoorsAddins) && parsePath envDoorsAddins){
			// Store addins location; add a final backslash, if there is none
			if (envDoorsAddins[match 1] != ""){
				if (envDoorsAddins[end 1] "" == sep){
					put( skpPaths, idx, envDoorsAddins[match 1]);
				}
				else{
					put( skpPaths, idx, envDoorsAddins[match 1] "" sep);
				}
			}
			envDoorsAddins = envDoorsAddins[end 2 + 1:];
			idx++;
		}
	}
	return skpPaths;
}//GetStartingPaths
/**************************************************************/
/** Get the name of the current file dynamicaly.

\param[out] relativePath The relative path fom theaddins path.
\param dxlHereResult Result of the function call dxlHere.

\returns The name of this file.

\remarks 
***************************************************************/
string GetMyName(string dxlHereResult, string &relativePath){
	string myName = "";
	
	relativePath = "";
	
	// Retrieve the location of this DXL library relative to the addins path;
	// Replace Unix style '/' by Windows style '\' 
	Regexp reRelativePath = regexp "^<(.*/)([^:]+):";
	Regexp getMyName = regexp "^<([^:]+):";
	string dxlFile = dxlHereResult;
	// Retrieve the location of this DXL library relative to the addins path;
	///\todo Replace Unix style '/' by Windows style '\' 
	if (reRelativePath dxlFile){
		relativePath = dxlFile[match 1];
		myName = dxlFile[match 2];
	}
	else if (getMyName dxlFile){
		myName = dxlFile[match 1];
	}
		
	return myName;
}//GetMyName

/**************************************************************/
/** Get the full path of the current script starting from 
the addins path.

\param dxlHereResult Result of the function call dxlHere.

\returns The absolute path of the entry in the dxlHereResult
parameter.

\remarks This will not work in a main script, because if you
call there dxlHere(), then you only get the line number!
So the paramter for this script isn't necessary, but may be 
used later.
***************************************************************/
string GetMyFullPath(string dxlHereResult){
	string fullPath = "";
	
	Skip skpPaths = GetStartingPaths();
	if(!null(skpPaths)){
		string relativePath = "";
		string myName = GetMyName(dxlHereResult, relativePath);
		string addinsPath;
		for addinsPath in skpPaths do{
			if(canOpenFile(addinsPath relativePath myName, false )){
				// Found myself in: "addinsPath relativePath"
				fullPath = addinsPath relativePath;
				break;
			}
		}
	}
		
	delete(skpPaths);
	
	return fullPath;
}//GetMyFullPath

/**************************************************************/
/** Load icons. This needs the full path to the location.

\remarks The Icons path is located beneath the location of
this script.
***************************************************************/
void InitIcons(){
	string DxlImageFolder = "";

	DxlImageFolder = GetMyFullPath(dxlHere());
	DxlImageFolder = DxlImageFolder "Icons/";
	
	icoValid 	= load(DxlImageFolder "Valid.ico");
	icoInvalid 	= load(DxlImageFolder "Invalid.ico");
	icoIgnored 	= load(DxlImageFolder "Ignored.ico");
	icoNotRun 	= load(DxlImageFolder "NotRun.ico");
	
}//InitIcons
/**************************************************************
Add a method to the treeview underneath 'All Tests' with given icon.

\param tv The treeview.
\param method The method name.
\param ico The icon to set.
***************************************************************/
void AddItem(DBE tv, string method, Icon ico){
    Icon iconOpen, iconClosed;
   	insert(tv, "All Tests" dbSep method, ico, ico);
   	//expand
	set(tv, "All Tests" dbSep method);
   	
}//AddItem
/**************************************************************/
/** Replace an entry in the treeview, which is used for replacing 
the icon if a test is run.

\param tv The treeview.
\param method The method name.
\param ico The icon to set.

\remarks There is no normal way for replacing the icon in the
treeview. So we use this function. If the treeview is not sorted,
the entry will be placed at the end of the treeview.
***************************************************************/
void ReplaceItem(DBE tv, string method, Icon ico){
	//delete old entry
	delete(tv, "All Tests" dbSep method);
	AddItem(tv, method, ico);
}//ReplaceItem

/**************************************************************/
/** Check code to run.

\param code The code to verify.

\returns True if code valid.

\remarks Prints any error to the result control. 
***************************************************************/
bool ValidCode(Buffer code){
	string err = checkDXL(tempStringOf(code));
	if(null err){
		return true;
	}
	else{
		set(txtResult, "Error in source code: " err "\n" code "\n");
		return false;
	}
}//ValidCode
/**************************************************************/
/** Append information to output result.

\param infos The information to append.
***************************************************************/
void AppendResult(string infos){
	//ignore empty strings
	if(!null(infos)){
		//get current input
		string txt = get(txtResult);
		
		txt = txt infos;
		
		set(txtResult, txt);
	}
}//AppendResult
/**************************************************************/
/** Run selected Test.

\param file The test file to test.
\param logFile the file to log the results into.

\remarks Doesn't run called from callback with a string parameter
including the treeview selection. 
***************************************************************/
void RunTestInGui(string file, string logFile){
	//variables for profiling
	int startTime, stopTime;
	int countTests = 0;
	int countPassed = 0;
	int countIgnored = 0;

	//The time we started the test run
	Date startOfTestRun = dateAndTime(today);
	
	//summary of test
	Buffer summary = create();
	Buffer testSummary = create();
	Buffer code = create();
	//get the code and the methods
	Skip skpResult = GetTestMethods(file, code);
	string message = null;
	
	//first check if code is valid
	message = checkDXL(tempStringOf(code));
	//code valid
	if(null(message)){
		//extreact test methods
		Skip testMethods = null;
		DxlObject method = null;
		find(skpResult, duMethods, testMethods);
		//extract meta data
		DxlObject metaData = null;
		Skip tmpSkip = null;
		if(find(skpResult, duMetaData, tmpSkip)){
			//get them into the DxlObject
			find(tmpSkip, duMetaData, metaData);
		}
		//remember selected item
		string selectedItem = get(tvTests);
		//what is selected in the treeview
		string selection = getRealPath(dbSep get(tvTests));
		
		int offset = 0;
		int len = 0;
		//find leave of tree
		if(findPlainText(selection, "/", offset, len, false, true)){
			selection = selection[offset+1:];
		}
		
		//loop over all tests
		string result;
		//if we have fixture setup, we run it before all tests
		message = RunFixture(duFixtureSetup, code, metaData);
		
		//if setup fixture succeeded
		if(null(message)){
			Buffer testResult = create();
			Buffer call = create();
			bool errorInCode = false;
			string methodName;
			string methodResult = "";
			
			//start of test
			startTime = getTickCount_ ();
			for method in testMethods do{
				methodName = (string method->duMethod);
				if((selection == "All Tests") || (selection == methodName)){
			    	setempty(testResult);
					//in the calling we include Setup and Teardown
					call = code "\n";
					call += EncapsulateMethod(methodName, metaData);
					//validate code
					message = checkDXL(tempStringOf(call));
					if(null(message)){
				    	//update test counter
				    	countTests++;
						//with debug information
						if(gDxlDebugInfo){
							testResult += "//Testing " upper(methodName) "\n";
							testResult += duDebugStart;
							testResult += call;
							testResult += duDebugEnd; 
						}
				    	//run test
				    	result = eval_(tempStringOf(call));
				    	//get test result
						testResult += GetFormatedTestResult(methodName, result, methodResult);
				    	if(methodResult == "passed"){
				    		countPassed++;
				    		ReplaceItem(tvTests, methodName, icoValid);
				    		
				    	}else if(methodResult == "failed"){
				    		ReplaceItem(tvTests, methodName, icoInvalid);
				    	}else if(methodResult == "ignored"){
				    		countIgnored++;
				    		ReplaceItem(tvTests, methodName, icoIgnored);
				    	}
				    	//append current test result into UI
				    	AppendResult(tempStringOf(testResult));
						
						//remember test result for logging
						if(methodResult == "passed"){
							//positive tests don't return a message; create it here for logging
							testResult += "+ " methodName "\n";
							testResult += "Successfull.\n\n";
						}
					}else{
						//error in code, so no more tests applicable
						errorInCode = true;
						message = "Error in test code:\n" message;
						//append code added by framework to see the problems
						message = message "\n" stringOf(call);
					}
					//remember summary for logging
				    testSummary += testResult;
				    if(errorInCode){
				    	break;
				    }
				}else{
					ReplaceItem(tvTests, methodName, icoIgnored);
				}
			}//for  method
			//end of test
			stopTime = getTickCount_ ();
			
			//show result in treeview and output window
			set(tvTests, selectedItem);
			
			if(errorInCode){
				summary = message;
			}else{
				//get summary
			    summary = GetTestSummary(startOfTestRun, startTime, stopTime, countTests, countPassed, countIgnored);
				
				//run fixture teardown
				message = RunFixture(duFixtureTeardown, code, metaData);
			    //append fixture errors
			    if(!null(message)){
			    	summary += "\n** Error in FixtureTeardown: \n";
			    	summary += message "\n";
			    	summary += "Check your fixture teardown configuration, if all is cleaned up.";
			    }
			}
			delete(testResult);
			delete(call);
		}//valid FixtureSetup
		else{
		    summary += "** Error in FixtureSetup: \n";
			summary += message "\n";
			summary += "No test run performed.";
		}
		//clear mem
		if(!null(metaData)){delete(metaData);}
		delete(tmpSkip);
		delete(testMethods);
	}//code valid
	else{
		summary += message "\n";
		set(txtResult, message);
	}

	//log to file
	if(null(message)){
		message = LogTestsToFile(logFile, testSummary, summary);
		if(!null(message)){
			summary += message;
		}
	}	
	//show summary in UI
	AppendResult(duSummarySep tempStringOf(summary));

	//free buffers	
	delete(testSummary);
	delete(summary);

	delete(code);
	
}//RunTestInGui

/**************************************************************/
/** Add test methods to treeview by location or sorted.

\param testMethods The list of methods.

\returns The sorted list.

\remarks Originally the tests are sorted by the line they are found.
Sorting deletes that line number and replaces it by an index.
***************************************************************/
void AddMethods(Skip testMethods){
	//get sort option
	bool sorted = get(chkSort);
	
	//skip to sort the methods
	Skip skpSorter = createString();
	
	string  methodName = "";
	DxlObject testMethod = null;
	for testMethod in testMethods do{
		methodName = (string testMethod->duMethod);
		if(sorted){
			//add to sorting skip list
			put(skpSorter, methodName, methodName);
		}else{
			//unsorted can be directly inserted
			AddItem(tvTests, methodName, icoNotRun);
		}
	}
	
	//insert methods if sorted checked
	for methodName in skpSorter do{
		AddItem(tvTests, methodName, icoNotRun);
	}
	
	//delete the old list
	delete(skpSorter);
		
}//AddMethods
/**************************************************************/
/** Reload treeview with tests in selected file.
***************************************************************/
void LoadTests(){
	//clear results window
	set(txtResult, "");
	// clear treeview
	empty(tvTests);
	//de-activate test run button
    inactive(btnOk);

	//does file exist and can it be read
	string file = get(dbeFile);
	if(fileExists_(file) && canOpenFile(file, false)){
		Buffer code = create();
		//read all test methods
		Skip skpResult = GetTestMethods(get(dbeFile), code);
		Skip testMethods = null;
		find(skpResult, duMethods, testMethods);
		
		//script must contain a test method
		bool hasTestMethod = false;
		string methodName;
		for methodName in testMethods do{
		    hasTestMethod = true;
		    break;
		}
		if(hasTestMethod){
    		//check code
    		ValidCode(code);
    		
    		//fill treeview
    		insert(tvTests, "All Tests", iconDatabase, iconDatabase);
    	
    		//and the tests
    		AddMethods(testMethods);
    		//activat run test button
            active(btnOk);
		}else{
		    set(txtResult, "The file '" file "' doesn't contain a test method.");
		}
		//free memory
		delete(code);
		if(!null(testMethods)){delete(testMethods);}
		delete(skpResult);
		//select the root in the treeview
		set(tvTests, "All Tests");
		
		
	}
	else{
		set(txtResult, "The file '" file "' doesn't exist or can't be read.");
	}
}//LoadTests
/**************************************************************/
/** Get the log file, if logging should be done.

\returns Path and name of the log file.

\remarks If logging to file is disabled, we return an empty string.
***************************************************************/
string DxlUnitResults(){
	//create standard log file
	string logFile = "";
	//if we have to log, we create standard log file
	if(get(chkLogResults)){
		logFile = get(dbeFile);
		logFile = logFile ".log";
	}

	return logFile;
}//DxlUnitResults
/**************************************************************/
/** Common event handling, if test run shall be performed.
***************************************************************/
void StartTesting(){
	//show activity	
	busy(dlgUnitTests);
	//treeview needs at least one test method
	string testMethod = get(tvTests);
	if(null(testMethod)){
	    set(txtResult, "No test method to run.");
	}else{
    	//remember selected method
    	string selectedItem = get(tvTests);
    	//reset icons
    	LoadTests();
    	//select it after icons are replaced		
    	set(tvTests, selectedItem);
    	//run the selected tests
    	RunTestInGui(get(dbeFile), DxlUnitResults);
	}	
	ready(dlgUnitTests);
}//StartTesting

/*** Events (Callbacks)***/
/******************************************************************************
	doOk
******************************************************************************/
void doOk(DB db){
    release(dlgUnitTests)
    hide(dlgUnitTests)
}//doOk
/******************************************************************************
Dummy callback.
******************************************************************************/
void doNothing(DB db){
	return;
}//doNothing
/******************************************************************************
	doCancel
******************************************************************************/
void doCancel(DB db){
    if(!null(icoValid)){destroy(icoValid);}
    if(!null(icoInvalid)){destroy(icoInvalid);}
    if(!null(icoIgnored)){destroy(icoIgnored);}
    if(!null(icoNotRun)){destroy(icoNotRun);}
    release(dlgUnitTests);
    hide(dlgUnitTests);
}//doCancel
/******************************************************************************
Event for single click on tree item.
******************************************************************************/
void doSelect(DBE dbe){
	return;
}//doSelect
/******************************************************************************
Event for double click on tree item runs the selected test.
******************************************************************************/
void doActivate(DBE dbe){
	StartTesting();
}//doActivate

/******************************************************************************
Event for click on apply button to run the selected test.
******************************************************************************/
void doRunTest(DB dbe){
	StartTesting();
}//doRunTest
/**************************************************************/
/** Event for file browser.
***************************************************************/
void dbeFile_Changed(DBE dbe){
	//read data and refresh screen
	LoadTests();
}//dbeFile_Changed

/**************************************************************/
/** Reload file and refresh treeview event for button.

\param btn The button clicked.
***************************************************************/
void doRefresh(DBE btn){
	LoadTests();
}//doRefresh
/**************************************************************/
/** Repaint event for canvas.

\param cnv The canvas to repaint.
***************************************************************/
void doRepaint(DBE cnv){
	string DxlImageFolder = "";
	
	DxlImageFolder = GetMyFullPath(dxlHere());
	DxlImageFolder = DxlImageFolder "Icons/";
	
	if(canOpenFile(DxlImageFolder "BeBold.bmp", false)){
		bitmap(cnv, DxlImageFolder "BeBold.bmp", 0, 0);
	}
}//doRepaint
/**************************************************************/
/** Provides the user interface for the tests to be run.

\param dbParent	Parent dialog box. Returns focus to this dialog
on closing.
\param file The test file to load on startup.

\remarks When run from explorer and the file given is empty, the
browse control initializes with the current directory. So file
willbe set to a blank if it is null.
***************************************************************/
void LoadGui(DB dbParent, string file){
    bool hasFile = false;
	string infoField = "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033{\\fonttbl{\\f0\\fswiss\\fprq2\\fcharset0 Verdana;}{\\f1\\fswiss\\fcharset0 Arial;}}" //-
					   "{\\*\\generator Msftedit 5.41.15.1515;}\\viewkind4\\uc1\\pard\\qc\\lang1031\\b\\f0\\fs20 Dxl Unit Framework " //-
					   duFrameworkVersion " {\\'A9} \\i Bernhard Bold\\lang1033\\b0\\i0\\f1\\par}";
    //file may not be null on startup
    if(null(file)){
        file = " ";
    }else{
        hasFile= true;
    }
	
	//placement of dialog with parent
    if (null dbParent){
	    dlgUnitTests = create("DxlUnit Testframework");
    }
    else{
    	dlgUnitTests = create(dbParent, "Unit Tests");
	}
	/*--- create controls ----*/
	//show infoField
	frmCopyright = frame(dlgUnitTests, "", 250, 3);
	frmCopyright->"top"->"form";
	frmCopyright->"left"->"form";
	frmCopyright->"right"->"form";
	frmCopyright->"bottom"->"unattached";
	cnvLogo = canvas(dlgUnitTests, 40, 40, doRepaint);
	cnvLogo->"top"->"inside"->frmCopyright;
	cnvLogo->"left"->"inside"->frmCopyright;
	cnvLogo->"right"->"unattached";
	cnvLogo->"bottom"->"inside"->frmCopyright;
	txtCopyright = richText(dlgUnitTests, "", infoField, 40, true);
	txtCopyright->"top"->"inside"->frmCopyright;
	txtCopyright->"left"->"spaced"->cnvLogo;
	txtCopyright->"right"->"inside"->frmCopyright;
	txtCopyright->"bottom"->"inside"->frmCopyright;
	
	//
	frmTests = frame(dlgUnitTests, "Tests", 250, 20);
	frmTests->"top"->"spaced"->frmCopyright;
	frmTests->"left"->"form";
	frmTests->"right"->"unattached";
	frmTests->"bottom"->"form";
	//file selection
	dbeFile = fileName(dlgUnitTests, "File:", file, "*.dxl; *.inc", "DxlUnit Files", false);
	set(dbeFile, dbeFile_Changed);
	dbeFile->"top"->"inside"->frmTests;
	dbeFile->"left"->"inside"->frmTests;
	dbeFile->"right"->"inside"->frmTests;
	dbeFile->"bottom"->"unattached";
	
	//create sorting checkbox
	chkSort = toggle(dlgUnitTests, "Sort tests", true);
	chkSort->"top"->"spaced"->dbeFile;
	chkSort->"left"->"inside"->frmTests;
	chkSort->"right"->"unattached";
	chkSort->"bottom"->"unattached";
	
	//create refresh button
	btnRefresh = button(dlgUnitTests, "Refresh UI", doRefresh);
	btnRefresh->"top"->"aligned"->chkSort;
	btnRefresh->"left"->"unattached";
	btnRefresh->"right"->"inside"->frmTests;
	btnRefresh->"bottom"->"unattached";

	//info for using
    DBE lblInfo = label(dlgUnitTests, "Please select a test to run...");
	lblInfo->"top"->"spaced"->chkSort;
	lblInfo->"left"->"inside"->frmTests;
	lblInfo->"right"->"inside"->frmTests;
	lblInfo->"bottom"->"inside"->frmTests;
	
	//create the treeview
    tvTests = treeView(dlgUnitTests, 0, 100, 15);
	tvTests->"top"->"spaced"->lblInfo;
	tvTests->"left"->"inside"->frmTests;
	tvTests->"right"->"inside"->frmTests;
	tvTests->"bottom"->"inside"->frmTests;
    
	frmResults = frame(dlgUnitTests, "Results", 250, 20);
	frmResults->"top"->"spaced"->frmCopyright;
	frmResults->"left"->"unattached";
	frmResults->"right"->"form";
	frmResults->"bottom"->"form";

	chkLogResults = toggle(dlgUnitTests, "Log to file", false); 
	chkLogResults->"top"->"inside"->frmResults;
	chkLogResults->"left"->"inside"->frmResults;
	chkLogResults->"right"->"inside"->frmResults;
	chkLogResults->"bottom"->"unattached";

    txtResult = text(dlgUnitTests, "", "",350, 280, true);
	txtResult->"top"->"spaced"->chkLogResults;
	txtResult->"left"->"inside"->frmResults;
	txtResult->"right"->"inside"->frmResults;
	txtResult->"bottom"->"inside"->frmResults;
	
	// splitter 
	DBE dbeSplitter = splitter(dlgUnitTests, frmTests, frmResults, 4);
	dbeSplitter->"top"->"spaced"->frmCopyright;
	dbeSplitter->"left"->"unattached"; 
	dbeSplitter->"bottom"->"form";
	dbeSplitter->"right"->"unattached"; 
    
    //run the tests
    btnOk   = apply(dlgUnitTests, "Run test", doRunTest);
    inactive(btnOk);
    
	//create close button and redirect standard close action
	close(dlgUnitTests, false, doCancel);
	ok(dlgUnitTests, "Close", doCancel);
	//finsh dialog
    realize dlgUnitTests;
	
	//callback for select(single click) and activate(double click)
    set(tvTests, doSelect, doActivate);
	
    //fill treeview
    if(hasFile){LoadTests();}
    //bring to front
    raise(dlgUnitTests);
    //cursor to normal
    ready(dlgUnitTests);

}//LoadGui
/**************************************************************/
/** Main function to initialize the GUI.

\param file The test file.
\param debug Show additional debug info.
***************************************************************/
void ShowGui(string file, bool debug){
	//take over debug info
	gDxlDebugInfo = debug;
	
	//load icons
	InitIcons();
	
	//show the tree
	LoadGui(null, file);
    
	//display dialog modal
	show(dlgUnitTests);
	
}//ShowGui
