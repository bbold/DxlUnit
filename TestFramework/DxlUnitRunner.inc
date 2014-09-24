//< 
/** Include for running a test script for DxlUnit.
\file DxlUnitRunner.inc
\author Bernhard Bold
\date 03.02.2011
  
\version 2.8

\copyright (c) Bernhard Bold

\dependencies <br>
DxlUnitBL.inc - the business layer.<br>
DxlUnitGui.inc - the UI to show (eventualy).<br>

\history <br>
 03.02.2011 Created. <br>
---------------------------------------------------------------
Copyright (c) 2014, Bernhard Bold
All rights reserved.

The software is licensed under the conditions of the BSD Simplified.
For details see the license.txt file.
***************************************************************/
/// \cond
pragma encoding, "UTF-8"
/// \endcond

///\todo Ignore tests in console with '@Ignore', if next line is a test method.

//common methods
#include <TestFramework/DxlUnitBL.inc>
//the GUI to show eventualy
#include <TestFramework/DxlUnitGui.inc>

/**************************************************************/
/** Run the tests in the given file.

\param file The file with the tests to run.
\param logFile the file to log the results into.
\param debug If true writes debug informations to the console;
the code to be run.

\remarks Here is the real code for running.
***************************************************************/
void RunConsole(string file, string logFile, bool debug){
	string message = "";
	
	//test summary collector
	Buffer testSummary = create();
	//summary
	Buffer summary = create();

	//The time we started the test run.
	Date startOfTestRun = dateAndTime(today);
	int startTime, stopTime;
	
	//if exists file
	if(fileExists_(file)){
		int countTests = 0;
		int countPassed = 0;
		int countIgnored = 0;
		
		//parse tests
		Buffer testResult = create();
		Buffer code = create();
		DxlObject metaData = null;	
		Skip skpResults = GetTestMethods(file, code);
		Skip tmpSkip = null;
		//extract meta data
		if(find(skpResults, duMetaData, tmpSkip)){
			//get them into the DxlObject
			find(tmpSkip, duMetaData, metaData);
		}
		//extract test methods
		Skip testMethods = null;
		find(skpResults, duMethods, testMethods);
		//first check if code is valid
		message = checkDXL(tempStringOf(code));
		
		//code valid
		if(null(message)){
		    Buffer call = create();
			string methodName, result;
			DxlObject method;
			// variables to split results into test errors, other errors and infos
			string methodResult = "";
			//if we have fixture setup, we run it before all tests
			message = RunFixture(duFixtureSetup, code, metaData);
			
			//if setup fixture succeeded
			if(null(message)){
				//start of test
				startTime = getTickCount_ ();
				//call each test method separately
			    for method in testMethods do{
			    	methodName = (string method->duMethod);
			    	setempty(testResult);
			    	//in the calling we include Setup and Teardown
			    	call = code "\n";
			    	call += EncapsulateMethod(methodName, metaData);
	
			    	//validate code
					message = checkDXL(tempStringOf(code));
					if(null(message)){
				    	//update test counter
				    	countTests++;
						
						//with debug information
						if(debug){
							testResult += "//Testing <" methodName ">\n";
							testResult += duDebugStart;
							testResult += call;
							testResult += duDebugEnd;
						}
						
						//run test and get results from DxlUnit-framework
						result = eval_(tempStringOf(call));
						//get test result
						testResult += GetFormatedTestResult(methodName, result, methodResult);
						//update counter
						if(methodResult == "passed"){
							countPassed++;
						}else if(methodResult == "ignored"){
							countIgnored++;
						}
					}//complete code valid
					else{
						testResult += "* " methodName ":\n Error in code to test: " message "\n";
                        //append code added by framework to see the problems
                        testResult+= "\n" stringOf(call);
					}
					//print result immediate
					print tempStringOf(testResult);
					//remember test result for logging
					if(methodResult == "passed"){
						//positive tests don't return a message; create it here for logging
						testResult += methodName "\n";
						testResult += "Successfull.\n\n";
					}
					testSummary += testResult;
			    }//for method
			    //log finish time
				stopTime = getTickCount_ ();
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
			}//FixtureSetup succeeded
			else{
			    summary += "** Error in FixtureSetup: \n";
				summary += message "\n";
				summary += "No test run performed.";
			}
			delete(call);
		}//code valid
		else{
			summary += message "\n";
		}
		delete(metaData);
		delete(tmpSkip);
		delete(code);
		delete(testMethods);
		delete(testResult);
	}//test file exists
	else{
		summary = "Test file '" file "' not found.\n";
	}

	//log to file
	message = LogTestsToFile(logFile, testSummary, summary);
	if(!null(message)){
		summary += message;
	}

	//show summary in console
	print duSummarySep;
	print tempStringOf(summary);
	
	delete(testSummary);
	delete(summary);
	
}//RunConsole
/**************************************************************/
/** Run the tests in the given file.

\param file The file with the tests to run.
\param debug If true writes debug informations to the console;
the code to be run.

\remarks Here is the real code for running.
***************************************************************/
void RunConsole(string file, bool debug){
	string logFile = null;
	RunConsole(file, logFile, debug);
}//RunConsole
/**************************************************************/
/** Run the tests in the given file.

\param file The file with the tests to run.

\remarks Short form.
***************************************************************/
void RunConsole(string file ){
	RunConsole(file, false);
}//RunConsole
/**************************************************************/
/** Runs the tests and log the results to the default log file.

\param file The file with the tests to run.
\param logFile Full name of the log file. 

\remarks The default log file will be created in the folder
where the test file exists and will be named equal to it with
extension '.log'.
***************************************************************/
void RunConsole(string file, string logFile){
	RunConsole(file, logFile, false);
}//RunConsole
/**************************************************************/
/** Runs the tests in the Gui.

\param  file The file with the tests to run.
\param debug Show additional debug infos.

\remarks This will start the Gui.
***************************************************************/
void RunGui(string file, bool debug){
	ShowGui(file, debug);
}//RunGui
/**************************************************************/
/** Runs the tests in the Gui without debug information.

\param  file The file with the tests to run.

\remarks This will start the Gui.
***************************************************************/
void RunGui(string file){
	//to get the file name with dxlHere doesn' t work starting with DxlEditor
	//since it will create a temp file
//	infoBox(dxlHere());
	RunGui(file, false);
}//RunGui