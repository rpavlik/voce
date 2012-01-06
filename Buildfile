repositories.remote << 'https://repository.apache.org/content/groups/public/'

#'http://download.java.net/maven/2/' # for FreeTTS
#"http://maven.ow2.org/maven2/" # for sphinx4?

SPHINX4 = 'lib/sphinx4/sphinx4.jar'
FREETTS = ['lib/freetts/freetts.jar', 'lib/freetts/cmulex.jar', 'lib/freetts/cmu_us_kal.jar', 'lib/freetts/en_us.jar']
JSAPI = 'lib/jsapi/jsapi.jar'

define 'voce' do
  project.version = '0.9.1'
  compile.with JSAPI, SPHINX4, FREETTS
  package :jar
end
