#include "api.h"
#include <pybind11/pybind11.h>
#include "singletons.h"
#include "juci.h"

PYBIND_PLUGIN(libjuci) {
  pybind11::module m("libjuci", "Python API for juCi++");
  m.def("directories_open", [](const char * dir) {
    boost::filesystem::path path(dir);
    if (boost::filesystem::is_directory(path))
      Singleton::directories->open(path);
  }, "Opens directory in file tree", pybind11::arg("String representation of the path"));
  m.def("get_highlighted_word", []() {
    auto g_application = g_application_get_default();
    auto gio_application = Glib::wrap(g_application, true);
    auto application = Glib::RefPtr<Application>::cast_static(gio_application);
    if (application) {
      auto view = application->window->notebook.get_current_view();
      if (view != nullptr) {
        if (view->get_token) {
          auto token = view->get_token();
          if (token.type != -1) {
            return token.spelling;
          }
        }
      }
    }
    return std::string(""); // const char * not viable
  });
  m.def("get_highlighted_word_type", [] () {
    auto g_application = g_application_get_default();
    auto gio_application = Glib::wrap(g_application, true);
    auto application = Glib::RefPtr<Application>::cast_static(gio_application);
    if (application) {
      auto view = application->window->notebook.get_current_view();
      if (view != nullptr) {
        if (view->get_token) {
          auto token = view->get_token();
          return token.type;
          }
        }
      }
    return -1;
  });
  m.def("terminal_print", [](const char * message) {
    return Singleton::terminal->print(message);
  });
  return m.ptr();
}