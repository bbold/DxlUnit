//<Unit Tests for dxl scripts
/** 
Implements a framework for unit testing. A unit test consists of 
an assertion, which checks an expected condition. A method with
return parameter should for example deliver false under defined
circumstances, true in others. The caller can call an assertion 
function together with the expectation. The assertion method 
themself then calls the real method and compares the result with 
the expectation. If we get what we assumed, the framework prints 
an ok, otherwise what the differences are andthe line of the 
Assert method in code.
   
\file DxlUnit.inc
\author Bernhard Bold
\date 05.10.2010
  
\version 2.8
 
\history
 05.10.2010 created <br>
---------------------------------------------------------------
Copyright (c) 2014, Bernhard Bold
All rights reserved.

The software is licensed under the conditions of the BSD Simplified.
For details see the license.txt file.
***************************************************************/

/* globals */
///Constant of successful test
const string g_DxlUnitTestOk = "ok";

///Result of current assertion.
string g_DxlTestResult = "";
///Result of all assertions during the test.
string g_DxlUnitErrors = "";
///Varible to catch run time errors.
string g_DxlRunTimeError = "";
///Other errors logged in the session.
string g_DxlUnitOtherErrors = "";
///Informations logged in the session.
string g_DxlUnitInfos = "";

///Holds the currently tested method.
string g_DxlUnitTestMethod = "";
///Number of assertions, for each method.
int g_DxlUnitNoOfAssertions = 0;
///Number of assertions that succeeded in this method.
int g_DxlUnitNoOfValidAssertions = 0;

///Variable for logging results to standard file.
bool g_DxlLogToFile	= false;

/**************************************************************/
/** Get the line in code where the test failed.

\param stack The call stack of the interpreter (extracted by 
dxlHere function).

\returns The line where the error occured.

\remarks Because the error is tested in the framework, the calling
stack delivered by dxlHere() has too many entries, which must be 
skipped. So we filter out all lines of the includes and give 
back the first occurrance of the main script, which is the call 
to the function Assert.
***************************************************************/
string LineNo(string stack){
	string lineNo = "";
	
	//ignore any includes
	Regexp re = regexp2("<Line:.*>");
	
	if(!null stack && re stack){
		lineNo = stack[match 0];
	}
	delete(re);
	
	return lineNo;
}//LineNo
/*************************************************************/
/** Store a message from the test file, which is considered
as an error and therefore shown in the result view.

\param err The text to log.

\remarks Writes line number where the error was occured in front 
of the error message, if not already in the argument. Ignores empty errors.
***************************************************************/
void WriteError(string err){
    if(!null(err)){
        //check if error contains already a line number
        string lneNo = LineNo(err);
    	//not, so compute the line number
    	if(null(lneNo)){
    	   g_DxlUnitOtherErrors = g_DxlUnitOtherErrors LineNo(dxlHere);
    	}
    	//add message
    	g_DxlUnitOtherErrors = g_DxlUnitOtherErrors  " " err "\n";
    }
}//WriteError

/**************************************************************/
/** Store an error fom the test file with method name and 
the line number where the error occured.

\param method The method, where the error occured.
\param message The error message.

\remarks Ignores empty messages.
***************************************************************/
void WriteError(string method, string message){
    if(!null(message)){
		string lineNo = LineNo(dxlHere);
		//call to standard with extended method name
		WriteError("[" method "] " message);
    }
}//WriteError
/**************************************************************/
/** Store an info, which means it is only placed inside the output.

\param infos The information text to log.

\remarks Ignore empty infos.
***************************************************************/
void WriteInfo(Buffer infos){
    if(length(infos) > 0){
    	string lineNo = LineNo(dxlHere);
    	//take over info
    	g_DxlUnitInfos = g_DxlUnitInfos lineNo tempStringOf(infos) "\n";
    }
}//WriteInfo
/**************************************************************/
/** Store an info, which means it is only placed inside the output.

\param infos The information text to log.

\remarks Ignore empty infos.
***************************************************************/
void WriteInfo(string infos){
    if(!null(infos)){
    	string lineNo = LineNo(dxlHere);
    	//take over info
    	g_DxlUnitInfos = g_DxlUnitInfos lineNo infos "\n";
    }
}//WriteInfo

/**************************************************************/
/** To have a more detailed result report, the test methods
will be logged from the runner over this method.

\param method The method that will be run.

\remarks 
***************************************************************/
void SetTestMethod(string method){
	//take over method
	g_DxlUnitTestMethod = method;
	//reset assertion counters
	g_DxlUnitNoOfAssertions = 0;
	g_DxlUnitNoOfValidAssertions = 0;
}//SetTestMethod
/**************************************************************/
/** Take over test results.

\param result Ok for no error, an that error occured, or null
if the code failed.
\param summary A user defined description of the test.  

\remarks Introduced in V 2.0. The error message is build in the
testing function, cause there is more info about the called 
function and the test.  
***************************************************************/
void LogResult(string result, string summary){
	
	//take over current result
	g_DxlTestResult = result;
	//check result
	if(null result){
		//framework didn't return a value
	}
	else{
		//increment assertion counter
		g_DxlUnitNoOfAssertions++;
		
		//check if valid
		if(result == g_DxlUnitTestOk){
			//increment positive tests
			g_DxlUnitNoOfValidAssertions++;
		}else{
			//add summary if available
			if(!null(summary)){
				g_DxlUnitErrors = g_DxlUnitErrors "[" summary "]";
			}
			//add message to result variable
			g_DxlUnitErrors = g_DxlUnitErrors result"\n";
		}
	}	

}//LogResult
/**************************************************************/
/** Internal function to give out the results.
Central routine to give out the results to screen or
elsewhere.

\param success Depending on the it's value the output will be
generated.
\param summary Short form of the test (best one word).
\param expected the expected value as a string.
\param actual The actual value as a string.

\remarks The target of the output is held in the global variable
g_DxlUnitTarget. Use one of the DxlUnitTarget constants to set a 
valid output.
***************************************************************/
void WriteResult(bool success, string summary, string expected, string actual){
	string result = "";
	//build result line.. 
	if(!success){
		string lineNo = LineNo(dxlHere);
		
		result = lineNo " expected \"" expected "\" but was: \"" actual "\"";
	}	
	//..and store it
	LogResult(result, summary);
}//WriteResult
/**************************************************************/
/** Get the results of the test as string.

\returns The result of the tests as formated string.

\remarks Internal used only.
***************************************************************/
string GetTestResults(){
	string result = "";
	//no errors
	if(null g_DxlUnitErrors){
		//return ok 
		result = g_DxlUnitTestOk;
	}
	else{
		result = g_DxlUnitErrors;
	}

	//return only test errors and unexpected errors
	return result;
}//GetTestResults
/**************************************************************/
/** Get all logged Information of the current test including
other reported errors and infos.

\returns A formated report with the logged informations.

\remarks Only valid for the last test run.
***************************************************************/
string GetAllLogging(){
	string result = "";
	
	//first we return the tested method and the results
	result = "@Tested Method\n";
	result = result g_DxlUnitTestMethod "\n";
	result = result "@Count Assertions\n";
	result = result g_DxlUnitNoOfAssertions "\n";
	result = result "@Valid Assertions\n";
	result = result g_DxlUnitNoOfValidAssertions "\n";
	result = result "@Test Errors\n";
	//get the test results
	result = result GetTestResults();
	//add other errors
	result = result "\n@Other Errors\n";
	result = result g_DxlUnitOtherErrors;
	//add infos
	result = result "\n@Infos\n";
	result = result g_DxlUnitInfos;

	return result;
}//GetAllLogging

/*** Version 2.0 ***/
/* --------- test functions with 2 parameters --------- */
/**************************************************************/
/** Check equality of strings.

\param actual What we have.
\param expected What we expect.

\returns Null or qualified message.
***************************************************************/
string Equals(string actual, string expected){
	string result = "";
	if(expected == actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " Equals expected \"" expected "\" but was \"" actual "\""; 
	}
	return result;
}//Equals
/**************************************************************/
/** Check equality of int.

\param actual What we have.
\param expected What we expect.

\returns Null or qualified message.
***************************************************************/
string Equals(int actual, int expected){
	string result = "";
	if(expected == actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " Equals expected \"" expected "\" but was \"" actual "\""; 
	}
	return result;
}//Equals
/**************************************************************/
/** Check equality of bool.

\param actual What we have.
\param expected What we expect.

\returns Null or qualified message.
***************************************************************/
string Equals(bool actual, bool expected){
	string result = "";
	if(expected == actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " Equals expected \"" expected "\" but was \"" actual "\""; 
	}
	return result;
}//Equals
/**************************************************************/
/** Check if a string begins with defined part of a string. 

\param actual What we have.
\param expected What we expect.

\returns Null or qualified message.
***************************************************************/
string StartsWith(string actual, string expected){
	string result = "";
	string part = actual[0:length(expected) - 1];
	if(expected == part){ 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " StartsWith expected \"" expected "\" but was \"" part "\"";
	}
	return result;
}//StartsWith

/**************************************************************/
/** Check if a string ends with defined part of a string. 

\param actual What we have.
\param expected What we expect.

\returns Null or qualified message.
***************************************************************/
string EndsWith(string actual, string expected){
	string result = "";
	string part = actual[length(actual)-length(expected):];
	if(expected == part){ 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " EndsWith expected \"" expected "\" but was \"" part "\""; 
	}
	return result;
}//EndsWith
/**************************************************************/
/** Check if a string is contained in another case sensitive. 

\param actual What we have.
\param expected What we expect.

\returns Null or qualified message.

***************************************************************/
string Contains(string actual, string expected){
	string result = "";
	int offset = 0;
	int length = 0;
	
	if(findPlainText(actual, expected, offset, length, true)){ 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " Contains expected \"" expected "\" but was not inside \"" actual "\""; 
	}
	return result;

}//Contains
/* --------- test functions with 1 parameters --------- */
/**************************************************************/
/** Test if the given string is null.

\param actual The value to check.

\returns True, if so.
***************************************************************/
string IsNull(string actual){
	string result = "";
	if(null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNull expected \"null\" but was \"" actual "\""; 
	}
	return result;
}//IsNull
/**************************************************************/
/** Test if the given module is null.

\param actual The value to check.

\returns True, if so.
***************************************************************/
string IsNull(Module actual){
	string result = "";
	if(null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNull expected \"null\" but was \"" fullName(actual) "\""; 
	}
	return result;
}//IsNull
/**************************************************************/
/** Test if the given skip list is null.

\param actual The value to check.

\returns True, if so.
***************************************************************/
string IsNull(Skip actual){
	string result = "";
	if(null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNull expected \"null\" but was \"not null\""; 
	}
	return result;
}//IsNull
/**************************************************************/
/** Test if the given stream is null.

\param actual The value to check.

\returns True, if so.
***************************************************************/
string IsNull(Stream actual){
	string result = "";
	if(null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNull expected \"null\" but was \"not null\""; 
	}
	return result;
}//IsNull
/**************************************************************/
/** Test if the given DxlObject is null.

\param actual The value to check.

\returns True, if so.
***************************************************************/
string IsNull(DxlObject actual){
	string result = "";
	if(null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNull expected \"null\" but was \"not null\""; 
	}
	return result;
}//IsNull
/**************************************************************/
/** Test if the given OleAutoObj is null.

\param actual The value to check.

\returns True, if so.
***************************************************************/
string IsNull(OleAutoObj actual){
	string result = "";
	if(null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNull expected \"null\" but was \"not null\""; 
	}
	return result;
}//IsNull
/**************************************************************/
/** Test if the given string is not null.

\param actual The value to check.

\returns Null or qualified message.
***************************************************************/
string IsNotNull(string actual){
	string result = "";
	if(!null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNotNull expected \"not null\" but was \"null\""; 
	}
	return result;
}//IsNotNull
/**************************************************************/
/** Test if the given module is not null.

\param actual The value to check.

\returns Null or qualified message.
***************************************************************/
string IsNotNull(Module actual){
	string result = "";
	if(!null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNotNull expected \"not null\" but was \"null\""; 
	}
	return result;
}//IsNotNull
/**************************************************************/
/** Test if the given skip is not null.

\param actual The value to check.

\returns Null or qualified message.
***************************************************************/
string IsNotNull(Skip actual){
	string result = "";
	if(!null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNotNull expected \"not null\" but was \"null\""; 
	}
	return result;
}//IsNotNull
/**************************************************************/
/** Test if the given stream is not null.

\param actual The value to check.

\returns Null or qualified message.
***************************************************************/
string IsNotNull(Stream actual){
	string result = "";
	if(!null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNotNull expected \"not null\" but was \"null\""; 
	}
	return result;
}//IsNotNull
/**************************************************************/
/** Test if the given DxlObject is not null.

\param actual The value to check.

\returns Null or qualified message.
***************************************************************/
string IsNotNull(DxlObject actual){
	string result = "";
	if(!null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNotNull expected \"not null\" but was \"null\""; 
	}
	return result;
}//IsNotNull
/**************************************************************/
/** Test if the given OleAutoObj is not null.

\param actual The value to check.

\returns Null or qualified message.
***************************************************************/
string IsNotNull(OleAutoObj actual){
	string result = "";
	if(!null actual){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsNotNull expected \"not null\" but was \"null\""; 
	}
	return result;
}//IsNotNull

/**************************************************************/
/** Checks if actual value is true.

\param actual The value to check.

\returns Null or qualified message.
***************************************************************/
string IsTrue(bool actual){
	string result = "";
	if(actual == true){// 
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsTrue expected \"true\" but was \"false\""; 
	}
	return result;
}//IsTrue
/**************************************************************/
/** Checks if actual value is false.

\param actual The value to check.

\returns Null or qualified message.
***************************************************************/
string IsFalse(bool actual){
	string result = "";
	if(actual == false){//
		result = g_DxlUnitTestOk; 
	}
	else{
		string lineNo = LineNo(dxlHere);
		result = lineNo " IsFalse expected \"false\" but was \"true\""; 
	}
	return result;
}//IsFalse
/* --------- functions to call --------- */

/**************************************************************/
/** Main routine to test assertions with 2 parameters of type string.

\param actual The actual value.
\param that The test function to use. This one needs 2 parameters.
\param expected What we expect as result.
\param summary Short description of the test.
***************************************************************/
void Assert(string actual, string that(string, string), string expected, string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual, expected);
	LogResult(success, summary);
}//Assert

/**************************************************************/
/** Short form to test assertions of type string with 2 parameters.

\param actual The actual value.
\param that The test function to use. This one needs 2 parameters.
\param expected The value expected.

\remarks Tests of strings like IsNull are not supported! To test
this use an empty string. 
***************************************************************/
void Assert(string actual, string that(string, string), string expected){
	Assert(actual, that, expected, "");
}//Assert

/**************************************************************/
/** Test string assertions with 1 parameter.

\param actual The actual value.
\param that The test function to use. This one needs 1 parameters.
\param summary A short description of the test.

\remarks Tests string assertion with one parameter like IsNull etc. 
***************************************************************/
void Assert(string actual, string that(string), string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual);
	LogResult(success, summary);
}//Assert

/**************************************************************/
/** Test string assertions with 1 parameter.

\param actual The actual value.
\param that The test function to use. This one needs 1 parameters.

\remarks Tests string assertion with one parameter like IsNull etc. 
***************************************************************/
void Assert(string actual, string that(string)){
	Assert(actual, that, "");
}//Assert
/**************************************************************/
/** Main routine to test assertions of type bool with 2 parameters.

\param actual The actual value.
\param that The test function to use. 
\param expected What we expect as result.
\param summary Short description of the test.
***************************************************************/
void Assert(bool actual, string that(bool, bool), bool expected, string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual, expected);
	LogResult(success, summary);
}//Assert
/**************************************************************/
/** Main routine to test assertions of type bool with 2 parameters.

\param actual The actual value.
\param that The test function to use.
\param expected What we expect as result.
***************************************************************/
void Assert(bool actual, string that(bool, bool), bool expected){
	Assert(actual, that, expected, "");
}//Assert
/**************************************************************/
/** Main routine to test assertions of type bool with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull.
\param summary Short description of the test.
***************************************************************/
void Assert(bool actual, string that(bool), string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual);
	
	LogResult(success, summary);
}//Assert
/**************************************************************/
/** Main routine to test assertions of type bool with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. IsTrue,
            IsFalse, IsNull, IsNotNull.
***************************************************************/
void Assert(bool actual, string that(bool)){
	Assert(actual, that, "");
}//Assert

/**************************************************************/
/** Main routine to test assertions of type int with 2 parameters.

\param actual The actual value.
\param that The test function to use. 
\param expected What we expect as result.
\param summary Short description of the test.
***************************************************************/
void Assert(int actual, string that(int, int), int expected, string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual, expected);
	LogResult(success, summary);
}//Assert
/**************************************************************/
/** Main routine to test assertions of type int with 2 parameters.

\param actual The actual value.
\param that The test function to use. 
\param expected What we expect as result.
***************************************************************/
void Assert(int actual, string that(int, int), int expected){
	Assert(actual, that, expected, "");
}//Assert
/**************************************************************/
/** Main routine to test assertions of type Module with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
\param summary Short description of the test.
***************************************************************/
void Assert(Module actual, string that(Module), string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual);
	
	LogResult(success, summary);
}//Assert
/**************************************************************/
/** Main routine to test assertions of type Module with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
***************************************************************/
void Assert(Module actual, string that(Module)){
	Assert(actual, that, "");
}//Assert
/**************************************************************/
/** Main routine to test assertions of type Skip with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
\param summary Short description of the test.
***************************************************************/
void Assert(Skip actual, string that(Skip), string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual);
	
	LogResult(success, summary);
}//Assert
/**************************************************************/
/** Main routine to test assertions of type Skip with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
***************************************************************/
void Assert(Skip actual, string that(Skip)){
	Assert(actual, that, "");
}//Assert
/**************************************************************/
/** Main routine to test assertions of type Stream with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
\param summary Short description of the test.
***************************************************************/
void Assert(Stream actual, string that(Stream), string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual);
	
	LogResult(success, summary);
}//Assert
/**************************************************************/
/** Main routine to test assertions of type Stream with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
***************************************************************/
void Assert(Stream actual, string that(Stream)){
	Assert(actual, that, "");
}//Assert
/**************************************************************/
/** Main routine to test assertions of type DxlObject with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
\param summary Short description of the test.
***************************************************************/
void Assert(DxlObject actual, string that(DxlObject), string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual);
	
	LogResult(success, summary);
}//Assert
/**************************************************************/
/** Main routine to test assertions of type DxlObject with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
***************************************************************/
void Assert(DxlObject actual, string that(DxlObject)){
	Assert(actual, that, "");
}//Assert
/**************************************************************/
/** Main routine to test assertions of type OleAutoObj with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
\param summary Short description of the test.
***************************************************************/
void Assert(OleAutoObj actual, string that(OleAutoObj), string summary){
	string success = "";
	
	//call the test function und log results
	success = that(actual);
	
	LogResult(success, summary);
}//Assert
/**************************************************************/
/** Main routine to test assertions of type OleAutoObj with 1 parameter.

\param actual The actual value.
\param that The test function to use. For ex. Equals, IsTrue,
            IsFalse, IsNull, IsNotNull, Greater, Less, StartsWith,
            EndsWith, IsMatch.
***************************************************************/
void Assert(OleAutoObj actual, string that(OleAutoObj)){
	Assert(actual, that, "");
}//Assert
