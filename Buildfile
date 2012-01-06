repositories.remote << 'http://repository.apache.org/' << "http://repo2.maven.org/maven2"

define 'voce' do
  project.version = '0.9.1'
  package :jar
end
