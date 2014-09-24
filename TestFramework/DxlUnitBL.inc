//< DxlUnit business layer.
/** Business layer for DxlUnit, which holds common functions.
\file DxlUnitBL.inc
\author Bernhard Bold
\date 10.03.2011
  
\version 2.8

\dependencies <br>

\history <br>
 10.03.2011 Created. <br>
---------------------------------------------------------------
Copyright (c) 2014, Bernhard Bold
All rights reserved.

The software is licensed under the conditions of the BSD Simplified.
For details see the license.txt file.
***************************************************************/
/// \cond
pragma encoding, "UTF-8"
/// \endcond

/*----- constants --------*/
///Tool version.
const string duFrameworkVersion = "[V2.8]";
///Name of setup method in the unit framework.
const string duFixtureSetup		= "FixtureSetup";
///Name of teardown method in the unit framework.
const string duFixtureTeardown		= "FixtureTeardown";
///Name of setup method in the unit framework.
const string duMethodSetup		= "Setup";
///Name of teardown method in the unit framework.
const string duMethodTeardown	= "Teardown";
///Key for the line of code which starts the test run to either Gui or console.
const string duCallToThis	= "CallToThis";
///Separator for test result and summary.	
const string duSummarySep = "\n- Summary --------------------------\n";	
///Start separator for debug information.	
const string duDebugStart = "----- DEBUG ----->>\n";
///End separator for debug information	
const string duDebugEnd = "<<----- DEBUG -----\n\n";
///Key for test methods.
const string duMethods	= "TestMethods";
///Key for single test method.
const string duMethod	= "TestMethod";
///Key for line of test method.
const string duLine	= "LineNo";
///Key for meta data.
const string duMetaData	= "MetaData";
///Key for number of assertions.
const string duCountAssertions = "CountAssertions";
///Text for failed test. 
const string duResultFailed = "failed";
///Text for passed test. 
const string duResultPassed = "passed";
///Text for fignored test. 
const string duResultIgnored = "ignored";

/**************************************************************/
/** Ignore errors of a test method call by encapsualting it between
noError and lastError functions. 

\param method The call to a method. 

\returns The encapsulated call.

\remarks Errors occured will be appended to global variable
g_DxlRunTimeError in the DxlUnit framework..
***************************************************************/
Buffer HandleErrors(string method){
	Buffer bufCode =create();
	
	//on previous errors the function will not be called
	bufCode += "//catch run time errors in method '" method "'\n";
	bufCode += "if(null(g_DxlUnitOtherErrors)){\n";
	bufCode += "    " "noError();\n";
	bufCode += "    " method "();\n";
    bufCode += "    " "//log errors\n";
	bufCode += "    " "g_DxlRunTimeError = lastError();\n";
	bufCode += "    " "WriteError(\"" method "\", g_DxlRunTimeError);\n";
	bufCode += "};\n";
	
	return bufCode;
}//HandleErrors
/**************************************************************/
/** Create the call to the test method and return it.

\param method The method to call.
\param parameters Internal parameters, here to look, if a Setup()
and Teardown() function is available.

\returns The code to call the test method.

\remarks The test method is wrapped in the Setup() and Teardown()
functions.
***************************************************************/
Buffer EncapsulateMethod(string method, DxlObject parameters){
	Buffer call = create();
	bool existMethod = false;
	
	//take over method to test
	call += "SetTestMethod(\"" method "\");\n";
	//insert call to setup if that method is in the test script defined
	existMethod = (bool parameters->duMethodSetup);
	if(existMethod){
		//insert call to setup
		call += HandleErrors(duMethodSetup);
	}
	
	//insert call to the test method
	call += HandleErrors(method);
	
	//insert call to teardown if that method is in the test script defined
	existMethod = (bool parameters->duMethodTeardown);
	if(existMethod){
		call += HandleErrors(duMethodTeardown);
	}
	
	//append method to return results
   	call += "return_ GetAllLogging();\n";

	return call;
}//EncapsulateMethod
/**************************************************************/
/** Checks if buffer is comment line.

\param lne The buffer with no new line (\n) to check.

\returns True if so.

\remarks The buffer may not have a new line.
***************************************************************/
bool IsCommentLine(Buffer lne){
	bool isComment = false;
	string commentLine = "^( *//|\\t//)";
	Regexp re = regexp2(commentLine);
	if(re lne){
		isComment = true;
	}
	delete(re); 
	return isComment;
}//IsCommentLine
/**************************************************************/
/** Check if we are in a comment block.

\param lne Current line of code
\param[out] isBlock Remembers if we are in a comment block. Changes 
to false, if code block ends with current line.

\returns True if still so.

\remarks Comment blocks have to start and end in a separate line
to be identified correctly.
***************************************************************/
bool InBlockComment(Buffer lne, bool &isBlock){
	bool inBlock = isBlock;
	int startPosition = 0;
	
	//are we not in a comment block
	if(!isBlock){
		startPosition = contains(lne, "/*", startPosition);
		//look for start of block
		if(startPosition > -1){
			isBlock = true;
			//block start
			inBlock = isBlock;
			//set new start position to look for block end in this line
			startPosition++;
		}
	}
	//if we are in block (could be in same line)
	if(inBlock){
		//look for end of block
		startPosition = contains(lne, "*/", startPosition);
		if(startPosition > -1){
			isBlock = false;
		}
	}
	return inBlock;
}//InBlockComment
/**************************************************************/
/** Checks if buffer is the begin of a method.

\param lne The buffer to test.

\returns True, if so.

\remarks A method signature consists of a return type followed
by the method name and then the parameter list evetually then
comes an opening curly brace.
Used to test a line of code, if it's the start of a method. 
Doesn't cover incorrect syntax.
***************************************************************/
bool IsMethodStart(Buffer lne){
	//we look only for the most common return types
    string returnType = "^[ \t]*[a-zA-Z0-9]+[^ \t]+";
    string methodName = "[a-zA-Z0-9]+[ ]*[\\(]";
    string paramList = "[\\(]+(.*)\\)"; //[ \n\\{]";

	string methodSignature = returnType methodName paramList;

	bool isMethod = false;
	
	//with equal sign(= ) it's no method
	if(contains(lne, '=') > -1){
		isMethod = false;
	}
	else{
		Regexp signature = null;
		int iStart = 0;
		//new
		signature = regexp2(returnType);
		//find return type
		if(search(signature, lne, iStart)){
			iStart = end(0); 
			//find method name
			signature = regexp(methodName);
			if(search(signature, lne, iStart)){
				iStart = iStart + end(0) - 1;	//opening brace
				//find parameter list
				signature = regexp(paramList);
				if(search(signature, lne, iStart)){
					isMethod = true;
				}
			}
		}
		delete(signature);
	}
	
	return isMethod;
}//IsMethodStart
/**************************************************************/
/** Look for special methods like Setup or Teardown.

\param lne The current line.
\param method The method to look for. 

\returns True if found.

\remarks The methods don't have any parameter and a return type of
void.
***************************************************************/
bool HasSpecialMethod(Buffer lne, string method){
	bool found = false;
	
	//the signature 
	string signature = "( *void)( *" method ") *\\(( *)\\)";
	Regexp re = regexp2(signature);
	
	//check if is searched signature
	if(re lne){
		found = true;
	}
	
	delete(re);
	return found; 
}//HasSpecialMethod
/**************************************************************/
/** Check if line is an Assertion.

\param lne The current line.

\returns True if so.

\remarks An assertion will be found if
- the line starts with an Assert
- followed by an opening bracket '('
***************************************************************/
bool IsAssertion(Buffer lne){
	//what we want to find an Assert with only whitspaces at the beginning
	string assertionSignature = "^[ \t]*Assert *\\(.+";
	Regexp re = regexp2(assertionSignature);
	bool found = false;
	
	//check if is searched signature
	if(re lne){
		found = true;
	}
	
	delete(re);
	return found; 
}//IsAssertion
/**************************************************************/
/** Analyze a source file, if it has test methods.

\param file The file to analyze.
\param [out] code The code read in.

\returns A list with the test methods and meta data.

\remarks Used to test a line of code, if it's the start of a
test method. This is found by 3 factors:
 - return type is void
 - method name ends with 'Test'
 - method has no parameters 
***************************************************************/
Skip GetTestMethods(string file, Buffer &code){
	//clear buffer
	code = create();
	
	int line = 0;			//line counter
	int codeLine = 0;		//temporary used to identify the test code lines (without comment!)
	bool isBlock = false;	//remember if we are in a block
	Stream input;	
	Buffer currentLine = create();
	string methodName;
	bool hasSetup = false;
	bool hasTeardown = false;
	
	Skip skpResult = create();
	//holds methods; key is the line in the sourcefile
	Skip skpMethods = create();
	//a single methode
	DxlObject method = null;
	//structure for constraints of the test script
	Skip skpMetaData = create();
	//only one meta data (necessary to store different data types) 
	DxlObject metaData = new();
	//the method signature has a return value of void, then any spaces
	//followed by the name of the method ending with Test; then 
	//follows the parameter list in round brackets optional separated 
	//by spaces 
	string testMethodSignature = "(void *)(.+Test) *\\(( *)\\)";
	Regexp testSignature = regexp2(testMethodSignature);
	//output signatures for starting in console or UI
	string runCallSignature = "(Run(Console|Gui)) *\\((.+)\\)";
	Regexp runSignature = regexp2(runCallSignature);
	bool isCode = false;
	
	//does file exist?
	if(fileExists_(file)){
	
		input = read(file);
		if(!null input){
			int offset = 0;
			int len =0;
			int countAssert = 0;
			while(true){
				//read formatted line; otherwise you could use '->'
				input >= currentLine;
				line++;
				//remove prefix of UTF-8 formated files
				if(findPlainText(tempStringOf(currentLine), "ï»¿", offset, len, false)){
					currentLine = currentLine[length("ï»¿"):length(currentLine)];
				}
				//take over source code, if not a call to testrunner
				if(!(runSignature currentLine)){
					code +=  currentLine "\n";
				}else{
					//extract call to the runner
					metaData->duCallToThis = line;
				}					
				///\todo Block Comment doesn't work in test scripts
				//ignore comment block
				if(!InBlockComment(currentLine, isBlock)){
					//ignore line comments
					if(!IsCommentLine(currentLine)){
						//take over source code, if not a call to testrunner
						if(runSignature currentLine){
//never reached?
							//extract call to the runner
//							metaData->duCallToThis = stringOf(currentLine);
						}else{ 
							//is method start?
							if(IsMethodStart(currentLine)){
								//are we just collecting code?
								//is it a test method
								if(testSignature currentLine){
									//take over count for last method
									if(!null(method)){
										method->duCountAssertions = countAssert;
									}
									//reset assignment counter
									countAssert = 0;
									//returnType = currentLine[match 1];	//first group 'void'
									methodName = tempStringOf(currentLine[match 2]);	
									//take over method and absolute line in code
									method = new();
									method->duLine = line;
									method->duMethod = methodName;
									//take over 
									put(skpMethods, line, method);
									 
								}
								//check for FixtureSetup and FixtureTeardown
								else if(HasSpecialMethod(currentLine, duFixtureSetup)){
									metaData->duFixtureSetup = true;
									
								}
								else if(HasSpecialMethod(currentLine, duFixtureTeardown)){
									metaData->duFixtureTeardown = true;
								}
								//check for fixturesetup and teardown
								else if(HasSpecialMethod(currentLine, duMethodSetup)){
									metaData->duMethodSetup = true;
								}
								else if(HasSpecialMethod(currentLine, duMethodTeardown)){
									metaData->duMethodTeardown = true;
								}
							}else{
								//count assertions
								if(IsAssertion(currentLine)){countAssert++;}
							}
						}
					}
				}
				//reached the end?
				if(end of input) {break;}		
			}
		}
		close(input);
	}
	//take over results
	put(skpResult, duMethods, skpMethods);
	//first we need the meta data in a skip to take it over
	put(skpMetaData, duMetaData, metaData);
	put(skpResult, duMetaData, skpMetaData);
	
	delete(currentLine);
	delete(testSignature);
	delete(runSignature);
	return skpResult;
}//GetTestMethods

/**************************************************************/
/** Remove crlf to be able to use regular expressions for parsing
over more than one line.

\param buf The buffer with the text to scan.

\returns The 'cleaned' buffer.

\remarks Seems regular expressions is only able to parse line-wise.
***************************************************************/
Buffer RemoveCrLf(Buffer buf){
    string crlf = "\n";
    int i;
    Buffer buf1 = create();
    for (i=0; i<length(buf); i++) {
        if(buf[i:i] == crlf){
        }else{
            buf1 += buf[i];
        }
    }
    return buf1;
}//RemoveCrLf
/**************************************************************/
/** Check all sections available in results.

\param content Text to search in.
\param sectionStart Text for start of the section.
\param sectionEnd Text for end of section.
\param[out] err Message of missing section deleimiters.

\remarks Appends message to the err buffer given.
***************************************************************/
void CheckSection(string content, string sectionStart, string sectionEnd, Buffer &err){
    Buffer buf = create();
    //remove cr lf to be ableto parse whole content
    buf = content;
    buf = RemoveCrLf(buf);
    
    //we check only for the delimiters
    Regexp section = regexp2("(" sectionStart ").*(" sectionEnd ")");
    if(section buf){
        //
    }else{
        err += "One of the section tags is missing: " sectionStart ", " sectionEnd ".\n";
    }
    
    delete(section);
    delete(buf);
    
}//CheckSection
/**************************************************************/
/** Get content of a result section.

\param content Text to search in.
\param sectionStart Starting tag.
\param sectionEnd End tag.

\returns Text between the two tags.

\remarks 
***************************************************************/
string GetSectionContent(string content, string sectionStart, string sectionEnd){
	string selection = "";
	int len = 0;
	int offset = 0;
	int iStart = 0;
	
	//find start
	if(findPlainText(content, sectionStart, offset, len, true)){
		iStart = offset + len + 1;  //add cr
		//last end is new start
		offset = iStart;
		
		//find end (if given)
		if(null(sectionEnd)){
			//rest of content
			selection = content[iStart:];
		}
		else if(findPlainText(content, sectionEnd, offset, len, true)){
			selection = content[iStart:offset-2]; //without cr?	
		}
	}
	
	return selection;
}//GetSectionContent
/**************************************************************/
/** Split results into test errors, other errors and logged 
information.

\param result The result returned by the framework.
\param[out] testedMethod The method tested.
\param[out] noAssertions Number of assertions during the test.
\param[out] noValidAssertions Number of valid assertions during the test.
\param[out] testResults The test result.
\param[out] otherErrors Any other error logged.
\param[out] infos Any info logged.

\remarks Only valid if the test result delivers all info from
the frameworks GetAllLogging() function. The string returned delivers
always three constant parts in and between the information.
***************************************************************/
void SplitResults(string result, string &testedMethod, int &noAssertions, int &noValidAssertions, string &testResults, string &otherErrors, string &infos){
	Buffer err = create();
	//that's what we are looking for
	const string csTestMethod 	    = "@Tested Method";
	const string csAssertions	    = "@Count Assertions";
	const string csValidAssertions	= "@Valid Assertions";
	const string csResult 		    = "@Test Errors";
	const string csOtherErrors 	    = "@Other Errors";
	const string csInfos 		    = "@Infos";	

    //check if result contains all needed sections
    CheckSection(result, csTestMethod, csAssertions, err);
    CheckSection(result, csAssertions, csValidAssertions, err);
    CheckSection(result, csValidAssertions, csResult, err);
    CheckSection(result, csResult, csOtherErrors, err);
    CheckSection(result, csOtherErrors, csInfos, err);
    
    //all sections available
    if(length(err) == 0){
    	//direct extraction
    	testedMethod 			= GetSectionContent(result, csTestMethod, csAssertions);
    	string countAssertions 	= GetSectionContent(result, csAssertions, csValidAssertions);
    	//counted values have to be converted
    	if(null(countAssertions) || !isValidInt(countAssertions)){
    		noAssertions = 0;
    	}else{
    		noAssertions = intOf(countAssertions);
    	}
    	string countValidAssertions = GetSectionContent(result, csValidAssertions, csResult);
    	if(null(countValidAssertions) || !isValidInt(countValidAssertions)){
    		noValidAssertions = 0;
    	}else{
    		noValidAssertions = intOf(countValidAssertions);
    	}
    	//take the rest directly
    	testResults 	= GetSectionContent(result, csResult, csOtherErrors);
    	otherErrors 	= GetSectionContent(result, csOtherErrors, csInfos);
    	infos 			= GetSectionContent(result, csInfos, null);
    }else{
        otherErrors = stringOf(err);
    }
	delete(err);
	
}//SplitResults
/**************************************************************/
/** Extract results from single test in common format.

\param method The method tested.
\param summary The test summary to format.
\param[out] result Returns 'passed' on success, 'failed' if not,
or 'ignored' if test has no assertions.

returns Results for the test method; is null if passed.
***************************************************************/
Buffer GetFormatedTestResult(string method, string summary, string &result){
	//expect a failing test
	Buffer buf = create();
	
	// split results to test errors, other errors and infos
	string testMethod = "";
	int noAssertions = 0;
	int noValidAssertions = 0;
	string testResults = "";
	string otherErrors = "";
	string infos = "";

	//parse resulting string
	SplitResults(summary, testMethod, noAssertions, noValidAssertions, testResults, otherErrors, infos);
	//unknown error
	if(null(testResults)){
		result = duResultFailed;
		//method only, if we have errors
		buf += "* " method "\n";
		buf += "? UNKNOWN ERROR! The Unit Framework didn't return a value.\n";
		buf += summary;
	}
	//or other errors
	else if(testResults != "ok"){
		result = duResultFailed;
		//method only, if we have errors
		buf += "* " method "[" noAssertions "/" noValidAssertions "]" "\n";
		buf += testResults "\n";
	}
	else{
		//show if no assertions are in the tested method
		if(noAssertions == 0){
			result = duResultIgnored;
			buf += "~ " method "\n";
			buf += "Todo: add assertions.\n";
		}else{
			result = duResultPassed;
		}
	}

	//append other information
	if(!null(otherErrors)){
		buf += " -other Errors-\n" otherErrors "\n";
		result = duResultFailed;
	}
	if(!null(infos)){
		buf += " -infos-\n" infos "\n";
	}

	return buf;
}//GetFormatedTestResult
/**************************************************************/
/** Create test summary to show.

\param startOfTestRun Date and time of test start.
\param startTime Time the test has started.
\param stopTime Time the tests have ended.
\param countTests Number of tests.
\param countPassed Number of succeeded tests.
\param countIgnored Number of ignored tests.

\returns The formated summary.

\remarks 
***************************************************************/
Buffer GetTestSummary(Date startOfTestRun, int startTime, int stopTime, int countTests, int countPassed, int countIgnored){
	Buffer summary = create();
	
	//format the date in US english
	Locale loc = locale(1033);
	
	//format the time needed to run the tests
	int offset, len;
	real rSec = realOf(stopTime - startTime)/1000.;
	string fmtSec = rSec "";
	findPlainText(fmtSec, ".", offset, len, false);
	if(length(fmtSec) > offset+3){
		fmtSec = fmtSec[0:offset+3];
	}
	
    //print summary
	summary += "Start of test run: " stringOf(startOfTestRun, loc, "ddd, dd.MMM. yyyy")  "\n";
	summary += "Time needed for test run: " fmtSec " sec\n\n";

    if(countTests == countPassed){
    	summary += "All " countTests " tests passed succesfully.";
    }
    else{
    	string countFailed = (countTests - countPassed - countIgnored) "";
	    summary += "From " countTests " tests: ";
	    summary += countPassed " " duResultPassed " / ";
	    summary += countFailed " " duResultFailed " / ";
	    summary += countIgnored " " duResultIgnored ".\n";
    } 
	
	return summary;
}//GetTestSummary
/**************************************************************/
/** Log test results to a log file, if any given.

\param logFile The file to log to.
\param testSummary The summary of the single executed tests.
\param summary A summary of the test run.

\returns Any error.

\remarks Deletes the log file and creates a new one with the
results passed in as parameter.
***************************************************************/
string LogTestsToFile(string logFile, Buffer testSummary, Buffer summary){
	string err = "";
	//do we have to log?
	if(!null(logFile)){
	
		//also write to log file, if we have a stream
		Stream out = null;
		//check folder exists
		if(fileExists_(logFile)){
			//delete file if exists
			err = deleteFile(logFile);
		}
		//create file
		if(null(err)){
			Stream out = write(logFile);
			if(null(out)){ 
				//could not create stream
				err = "Could not create log file '" logFile "'.\n";
			}else{
				//write test results
				out << testSummary;
				out << duSummarySep;
				//and summary including the doors tester
				out << "Test run by: ";
				out << doorsname() "\n"; 
				out << summary;
				//close stream
				flush(out);
				close(out);
			}
		}else{
			err = "Error deleting log file '" logFile "': " err "\n";
		}
	}
	return err;
}//LogTestsToFile

/**************************************************************/
/** Run initializations before all tests are to be run.

\param fixture Either setup or teardown function.
\param code The code test code.
\param parameters Structure with the fixture method found result.

\returns Errors occured.

\remarks 
***************************************************************/
string RunFixture(string fixture, Buffer code, DxlObject parameters){
	string err = "";
	
	bool hasMethod = (bool parameters->fixture);
	//do we have a fixture method
	if(hasMethod){
	    Buffer call = create();
		call = code "\n";
	    //encapsulate fixture in error handler
	    call += HandleErrors(fixture);
	    //insert results
		call += "return_ GetAllLogging();\n";
		//first check if code is valid
		err = checkDXL(tempStringOf(call));
		
		//code valid, so run it
		if(null(err)){
		    //call fixture
			string result = eval_(tempStringOf(call));
			// split results to test errors, other errors and infos
			string testMethod = "";
			int  noAssertions = 0;
			int noValidAssertions = 0;
			string testResults = ""
			string otherErrors = "";
			string infos = "";
						
			//we consider only errors and return them
			SplitResults(result, testMethod, noAssertions, noValidAssertions, testResults, err, infos);
		}
		delete(call);
	}
	
	return err;
}//RunFixture
