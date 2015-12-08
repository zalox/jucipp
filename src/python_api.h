#ifndef JUCI_PYTHON_API_
#define JUCI_PYTHON_API_

#include <pybind11/pybind11.h>

extern "C" PYBIND_EXPORT PyObject * init_juci_api();

#endif  // JUCI_PYTHON_API_