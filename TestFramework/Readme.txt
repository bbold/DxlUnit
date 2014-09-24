DxlUnit Framework
=================
This project aims to be a fill the gap between TDD and and the dxl language used in 
IBM Rational Doors.
Developed overabout 4 years and tested in several projects for differnet customers
I think it is now stable enough to help others to work with it and maybe bring up
some new ideas to improve it.

Installation instructions
=========================
Overview
--------
(a) The DxlUnit Framework expects to be placed in a folder named 'TestFramework'
to be able to run. This path must be available in your Doors environment. 

(b) The test scripts need to be run the full path in your system, not the relative one.
To run your scripts with short paths just change the Base_Path in the
Workspace.inc file to the path of your test script folder. 

Details
-------
(a)
Here an example, if you place the framework in your local drive in the addins folder:
1. Go to the folder of your local Doors installation, to the addins folder.
   This is for Doors 9.3: 'C:\Program Files\IBM\Rational\DOORS\9.3\lib\dxl\addins'

2. Create inside a new folder and name it 'TestFramework'.

3. Copy all files into that folder.

If you place the files on another location, make sure, to have a 'TestFramework' 
folder and that the TestFramework folder is part of your addins path.

(b)
1. Open the 'Workspace.inc' file in the 'TestFramework' folder.

2. Change the 'Base_Path' to your development path for test scripts. 
   If this is in your local documents folder, here an example:
   
   BASE_PATH = "C:/<user>/My Documents/Doors/DxlWorkspace/";
   
   You then can easily move your test scripts to other locations, when using the
   Base_Path variable in your test scripts.


Remark
The DxlFramework is defined to run with Doors 9.x, but is easy to change to make it
running in Doors 8.3.

yours

Bernhard