#ifndef JUCI_API_H_
#define JUCI_API_H_

#include <pythoncpp.h>
#include <gtkmm.h>


PYBIND11_DECLARE_HOLDER_TYPE(T, Glib::RefPtr<T>)

namespace pybind11 {
namespace detail {
template <class T>
struct holder_helper<::Glib::RefPtr<T>> { // <-- specialization
  static const T *get(const ::Glib::RefPtr<T> &p) { return p.operator->(); }
};
}
}

namespace api {
  class jucipp {
    public:
      pybind11::module static create();
   };
   class gtk {
    public:
      pybind11::module static create();
   };
   class glib {
    public:
      pybind11::module static create();
   };
   class gio {
    public:
      pybind11::module static create();
   };
};

// auto module = pybind11::reinterpret_steal<pybind11::module>(pygobject_init(-1, -1, -1));
// pybind11::module api("jucipp", "Python bindings for juCi++");
//         api.def("get_juci_home", []() { return Config::get().home_juci_path.string(); })
//             .def("get_plugin_folder", []() { return Config::get().python.plugin_path; });
//         api.def_submodule("editor")
//            .def("get_current_gtk_source_view", []() {
//                   auto view = Notebook::get().get_current_view();
//                   if (view)
//                     return pyobject_from_gobj(view->gobj());
//                   return pybind11::reinterpret_steal<pybind11::module>(Py_None);
//             })
//             .def("get_file_path", []() {
//                   auto view = Notebook::get().get_current_view();
//                   if (view)
//                         return view->file_path.string();
//                   return std::string();
//             });
//         api.def("get_gio_window_menu", []() { return pyobject_from_gobj(Menu::get().window_menu->gobj()); })
//             .def("get_gio_juci_menu", []() { return pyobject_from_gobj(Menu::get().juci_menu->gobj()); })
//             .def("get_gtk_notebook", []() { return pyobject_from_gobj(Notebook::get().gobj()); })
//             .def_submodule("terminal")
//             .def("get_gtk_text_view", []() { return pyobject_from_gobj(Terminal::get().gobj()); })
//             .def("println", [](const std::string &message) { Terminal::get().print(message + "\n"); });
//         api.def_submodule("directories")
//             .def("get_gtk_treeview", []() { return pyobject_from_gobj(Directories::get().gobj()); })
//             .def("open", [](const std::string &directory) { Directories::get().open(directory); })
//             .def("update", []() { Directories::get().update(); });
//         return api.ptr();
//   };
// auto sys = pybind11::module::import("sys");
// auto list = pybind11::list(sys.attr("path"));
// list.append(Config::get().python.plugin_path);
// sys.attr("path") = list;


#endif // JUCI_API_H_