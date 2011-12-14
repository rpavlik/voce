#!/bin/sh

if [ -x "${JDK_HOME}/bin/javac" ]; then
	export JAVAC="${JDK_HOME}/bin/javac"
else
	export JAVAC=$(which javac)
fi

if [ -x "${JDK_HOME}/bin/jar" ]; then
	export JAR="${JDK_HOME}/bin/jar"
else
	export JAR=$(which jar)
fi

echo "Using this javac:	${JAVAC}"
echo "Using this jar  :	${JAR}"
echo

${JAVAC} -classpath ../../../lib/freetts.jar:../../../lib/freetts-jsapi10.jar:../../../lib/jsapi.jar:../../../lib/sphinx4.jar Utils.java SpeechSynthesizer.java SpeechRecognizer.java SpeechInterface.java && \
cd .. && \
${JAR} cmvf voce/MANIFEST.MF ../../lib/voce.jar voce/*.class && \
cd ../../lib && \
${JAR} uvf voce.jar gram/*.gram && \
${JAR} i voce.jar && \
cd ../src/java/voce
