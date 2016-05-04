# juCi++ API doc

## Prerequisites:
 * doxygen
 * plantuml 
  * install via apt-get or download from http://de.plantuml.com/
  * see also http://de.plantuml.com/starting.html
  * if downloaded either copy the jar file to /usr/bin or set the environment variable PLANTUML_PATH to point to the path containing the jar file)

## How to build the API doc:
```sh
mkdir jucipp/build
cd jucipp/build
cmake -DCMAKE_CXX_COMPILER=g++ ..
make doc
```

## Where is the generated API documentation
Open jupicpp/build/src/html/index.html
