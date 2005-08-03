Voce
voce.sourceforge.net

This file contains basic installation and usage info, including:
1. Package contents
2. List of dependencies
3. Setting up dependencies
4. Build instructions
5. Using Voce in your application
6. Tips for good results

For more documentation, visit the Voce website.

------------------------------------
Package contents
------------------------------------
samples - directory containing example applications that show how to use Voce
src - directory containing the Voce source code for Java and C++
license-BSD.txt - BSD Open Source license
license-LGPL.txt - LGPL Open Source license
readme.txt - you're reading it
SConstruct - SCons build system

------------------------------------
List of dependencies
------------------------------------
1. FreeTTS - freetts.sourceforge.net
2. CMUSphinx4 - cmusphinx.sourceforge.net
3. Java 1.5 runtime environment; Java SDK required to build Voce from sources
4. Java Native Interface (JNI), usually included in the Java SDK; required for C++ applications


------------------------------------
Setting up dependencies
------------------------------------
The term 'javadir' here refers to the root of your Java runtime environment installation.  If you are using the Java SDK, the runtime environment is usually included in the 'jre' directory.

The term 'class path' refers to the place where all Voce class files (packaged into jar files) are located.  This can be anywhere, as long as you tell Voce about the class path.  (The C++ and Java interfaces handle the class path in different ways, though.  This is explained below.)

1. FreeTTS
	- several jar files must be created and copied to the class path.

3. Java runtime environment
	- 
	- Make sure the jvm library can be located at runtime.  On win32, the file 'jvm.dll' (usually in javadir\bin\client) must be in your path.  In UNIX, the file 'libjvm.so' must be in your library path.  Do NOT move these libraries out of their default locations; as of Java 2 SDK v1.2, they look for other runtime environment libraries relative to their own locations.

------------------------------------
Build instructions
------------------------------------
Java

C++

------------------------------------
Using Voce in your application
------------------------------------
Java

C++

------------------------------------
Tips for good results
------------------------------------
* A good microphone makes a world of difference.  I saw substantial improvements in recognition accuracy when I switched to a cheap Labtec headset after using my laptop's built-in microphone.
