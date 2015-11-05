#include "api.h"
#include <boost/python.hpp>
namespace py = boost::python;

BOOST_PYTHON_MODULE(jucipy) {
    py::class_<terminal>("terminal", py::init<>())
    .def("println",&terminal::println)
    ;
};