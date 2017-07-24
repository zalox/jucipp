#include <api.h>
#include <pybind11/pybind11.h>
#include <pygobject.h>

inline pybind11::module pyobject_from_gobj(gpointer ptr) {
  auto obj = G_OBJECT(ptr);
  if (obj)
    return pybind11::reinterpret_steal<pybind11::module>(pygobject_new(obj));
  return pybind11::reinterpret_steal<pybind11::module>(Py_None);
}
