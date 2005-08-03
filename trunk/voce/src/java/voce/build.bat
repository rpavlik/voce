javac -classpath ../../../lib/freetts.jar;../../../lib/jsapi.jar;../../../lib/sphinx4.jar *.java
cd ..
jar cmvf voce/MANIFEST.MF ../../lib/voce.jar voce/*.class
cd ../../lib
jar uvf voce.jar gram/*.gram
jar i voce.jar
cd ../src/java/voce
