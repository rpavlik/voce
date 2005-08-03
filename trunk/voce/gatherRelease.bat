mkdir voce-0.9.0
cd voce-0.9.0
copy ..\readme.txt .
copy ..\changelog.txt .
copy ..\license-LGPL.txt .
copy ..\license-BSD.txt .
copy ..\vocabulary.txt .

mkdir lib
copy ..\lib\*.jar lib
copy ..\lib\readmt.txt lib
copy ..\lib\voce.config.xml lib
mkdir lib\gram
copy ..\lib\gram\*.gram lib\gram

mkdir samples

mkdir samples\playpen
copy ..\samples\playpen\readme.txt samples\playpen
mkdir samples\playpen\data
mkdir samples\playpen\data\blueprints
copy ..\samples\playpen\data\blueprints\*.xml samples\playpen\data\blueprints
mkdir samples\playpen\data\grammar
copy ..\samples\playpen\data\grammar\*.gram samples\playpen\data\grammar
mkdir samples\playpen\data\materials
copy ..\samples\playpen\data\materials\*.material samples\playpen\data\material
mkdir samples\playpen\data\models
copy ..\samples\playpen\data\models\*.mesh samples\playpen\data\models
mkdir samples\playpen\data\overlays
copy ..\samples\playpen\data\overlays\*.overlay samples\playpen\data\overlay
mkdir samples\playpen\data\packs
copy ..\samples\playpen\data\packs\*.zip samples\playpen\data\packs
mkdir samples\playpen\data\textures
copy ..\samples\playpen\data\textures\*.* samples\playpen\data\textures
mkdir samples\playpen\playpen
copy ..\samples\playpen\playpen\SConstruct samples\playpen\playpen
copy ..\samples\playpen\playpen\*.sln samples\playpen\playpen
copy ..\samples\playpen\playpen\*.suo samples\playpen\playpen
copy ..\samples\playpen\playpen\*.vcproj samples\playpen\playpen
copy ..\samples\playpen\playpen\*.cpp samples\playpen\playpen
mkdir samples\playpen\src
copy ..\samples\playpen\src\*.h samples\playpen\src
copy ..\samples\playpen\src\*.cpp samples\playpen\src

mkdir recognitionTest
mkdir recognitionTest\c++
copy ..\samples\recognitionTest\c++\SConstruct recognitionTest\c++
copy ..\samples\recognitionTest\c++\*.sln recognitionTest\c++
copy ..\samples\recognitionTest\c++\*.suo recognitionTest\c++
copy ..\samples\recognitionTest\c++\*.vcproj recognitionTest\c++
copy ..\samples\recognitionTest\c++\*.cpp recognitionTest\c++
mkdir recognitionTest\c++\grammar
copy ..\samples\recognitionTest\c++\grammar\*.gram recognitionTest\c++\grammar
mkdir recognitionTest\java
copy ..\samples\recognitionTest\java\build.* recognitionTest\java
copy ..\samples\recognitionTest\java\run.* recognitionTest\java
copy ..\samples\recognitionTest\java\*.java recognitionTest\java
mkdir recognitionTest\java\grammar
copy ..\samples\recognitionTest\java\grammar\*.gram recognitionTest\java\grammar

mkdir synthesisTest
mkdir synthesisTest\c++
copy ..\samples\synthesisTest\c++\SConstruct synthesisTest\c++
copy ..\samples\synthesisTest\c++\*.sln synthesisTest\c++
copy ..\samples\synthesisTest\c++\*.suo synthesisTest\c++
copy ..\samples\synthesisTest\c++\*.vcproj synthesisTest\c++
copy ..\samples\synthesisTest\c++\*.cpp synthesisTest\c++
mkdir synthesisTest\java
copy ..\samples\synthesisTest\java\build.* synthesisTest\java
copy ..\samples\synthesisTest\java\run.* synthesisTest\java
copy ..\samples\synthesisTest\java\*.java synthesisTest\java

mkdir src
mkdir src\c++
copy ..\src\c++\voce.h src\c++
mkdir src\java
mkdir src\java\voce
copy ..\src\java\voce\build.* src\java\voce
copy ..\src\java\voce\MANIFEST.MF src\java\voce
copy ..\src\java\voce\*.java src\java\voce
