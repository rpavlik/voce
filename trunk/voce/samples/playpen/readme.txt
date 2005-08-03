Voce 'Playpen' Sample Application
voce.sourceforge.net

This is a more complex application that uses Voce's speech synthesis and recognition capabilities.

------------------------------------
Setup
------------------------------------
Before running this application, you must have the Java runtime installed (preferably version 1.5, though earlier versions might work).  You must have %JAVA_HOME%\bin\client (Windows environment variable syntax) in your PATH environment variable, where JAVE_HOME is the base directory of your Java runtime installation.  You also must have Ogre (www.ogre3d.org) and OPAL (opal.sf.net) installed.


------------------------------------
Instructions
------------------------------------
'Playpen' is a simple arena where objects can be created and manipulated.  Click and drag the left mouse button to move objects, arrow or 'wasd' keys to move the camera, 'h' to toggle shadows, and 'l' to toggle a second light source, and speech commands to create objects.  

'color' + 'object type' is the general speech grammar rule used, where 'color' is one of the following:

- red
- green
- blue
- yellow
- orange
- purple

... and 'object type' is one of the following:

- box
- sphere
- character
- wall
- tower

One additional speech command is 'reset', which clears all objects from the scene.  Make sure your microphone and speakers are setup properly before running this program.  Have fun!

------------------------------------
Tips for good results
------------------------------------
* For better speech recognition, use a good microphone, preferably one close to your mouth (e.g. a headset).  I saw substantial improvements in recognition accuracy when I switched to a cheap Labtec headset after using my laptop's built-in microphone.