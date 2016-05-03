# juCi++ API doc

Prerequisites:
 * doxygen
 * plantuml (install via apt-get or download from http://de.plantuml.com/; if downloaded either copy the jar file to /usr/bin or set the environment variable PLANTUML_PATH to point to the path containing the jar file)

How to build the API doc:
```mkdir jucipp/build
cd jucipp/build
cmake -DCMAKE_CXX_COMPILER=g++ ..
make doc
```

Then open jupicpp/build/src/html/index.html
