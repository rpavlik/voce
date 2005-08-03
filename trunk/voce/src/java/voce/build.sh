${JDK_HOME}/bin/javac -classpath ../../../lib/freetts.jar:../../../lib/jsapi.jar:../../../lib/sphinx4.jar Utils.java SpeechSynthesizer.java SpeechRecognizer.java SpeechInterface.java
cd ..
${JDK_HOME}/bin/jar cmvf voce/MANIFEST.MF ../../lib/voce.jar voce/*.class
cd ../../lib
${JDK_HOME}/bin/jar uvf voce.jar gram/*.gram
${JDK_HOME}/bin/jar i voce.jar
cd ../src/java/voce
