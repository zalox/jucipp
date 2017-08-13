#include <iostream>
#include <pythoncpp.h>

py::Interpreter::Interpreter() {
  if (!Py_IsInitialized())
    Py_Initialize();
}

py::Interpreter::~Interpreter() {
  if (Py_IsInitialized())
    Py_Finalize();
  if (err::occurred())
    std::cerr << pybind11::error_already_set().what() << std::endl;
}

pybind11::module py::import::add_module(const std::string &module_name) {
  return pybind11::reinterpret_borrow<pybind11::module>(PyImport_AddModule(module_name.c_str()));
}

pybind11::dict py::import::get_module_dict() {
  return pybind11::reinterpret_borrow<pybind11::dict>(PyImport_GetModuleDict());
}

pybind11::module py::import::reload(pybind11::module &module) {
  auto reload = pybind11::reinterpret_steal<pybind11::module>(PyImport_ReloadModule(module.ptr()));
  if (!reload)
    throw pybind11::error_already_set();
  return reload;
}

pybind11::object py::err::occurred() {
  return pybind11::reinterpret_borrow<pybind11::object>(PyErr_Occurred());
}
