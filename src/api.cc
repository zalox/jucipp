#include <api.h>
#include <config.h>
#include <memory>
#include <menu.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
// #include <pygobject.h>
#include <terminal.h>

// #include <pygobject.h>
// inline pybind11::module pyobject_from_gobj(gpointer ptr) {
//   auto obj = G_OBJECT(ptr);
//   if (obj)
//     return pybind11::reinterpret_steal<pybind11::module>(pygobject_new(obj));
//   return pybind11::reinterpret_steal<pybind11::module>(Py_None);
// }

pybind11::module api::jucipp::create() {
  pybind11::module api("jucipp", "Python bindings for juCi++");

  const auto path_to_str = [](const boost::filesystem::path &path) {
    return path.string();
  };

  const auto concat_str = [](const boost::filesystem::path &self, const std::string &str) {
    return self / str;
  };

  const auto concat_path = [](const boost::filesystem::path &self, const boost::filesystem::path &str) {
    return self / str;
  };

  auto api_path =
      pybind11::class_<boost::filesystem::path>(api, "Path")
          .def(pybind11::init<const std::string &>())
          .def("__repr__", path_to_str)
          .def("__str__", path_to_str)
          .def("__add__", concat_str, pybind11::is_operator())
          .def("__add__", concat_path, pybind11::is_operator());

  auto api_config =
      pybind11::class_<Config, std::unique_ptr<Config, pybind11::nodelete>>(api, "Config")
          .def_static("get", &Config::get, pybind11::return_value_policy::reference)
          .def_readwrite("project", &Config::project)
          .def_readwrite("menu", &Config::menu);

  auto api_config_project =
      pybind11::class_<Config::Project>(api_config, "Project")
          .def_readwrite("clear_terminal_on_compile", &Config::Project::clear_terminal_on_compile)
          .def_readwrite("save_on_compile_or_run", &Config::Project::save_on_compile_or_run)
          .def_readwrite("default_build_path", &Config::Project::default_build_path)
          .def_readwrite("debug_build_path", &Config::Project::debug_build_path)
          .def_readwrite("ctags_command", &Config::Project::ctags_command);

  auto api_config_menu =
      pybind11::class_<Config::Menu>(api_config, "Menu")
          .def_readwrite("keys", &Config::Menu::keys, pybind11::return_value_policy::reference);


  auto api_menu =
      pybind11::class_<Menu, std::unique_ptr<Menu, pybind11::nodelete>>(api, "Menu")
          // .def_readwrite("juci_menu", &Menu::juci_menu);
          .def("set_keys", &Menu::set_keys)
          .def("build", &Menu::build)
          .def("add_action", &Menu::add_action)
          .def_static("get", &Menu::get, pybind11::return_value_policy::reference);

  auto api_terminal =
      pybind11::class_<Terminal, std::unique_ptr<Terminal, pybind11::nodelete>>(api, "Terminal")
          .def("print", &Terminal::print)
          .def("process",
               (int (Terminal::*)(const std::string &, const boost::filesystem::path &, bool)) & Terminal::process)
          .def("async_process", &Terminal::async_process)
          .def("kill_last_async_process", &Terminal::kill_last_async_process)
          .def("kill_async_processes", &Terminal::kill_async_processes)
          .def("clear", &Terminal::clear)
          .def("configure", &Terminal::configure)
          .def_static("get", &Terminal::get, pybind11::return_value_policy::reference);

  auto api_terminal_in_progress =
      pybind11::class_<Terminal::InProgress>(api_terminal, "InProgress")
          .def(pybind11::init<const std::string &>())
          .def("done", &Terminal::InProgress::done)
          .def("cancel", &Terminal::InProgress::cancel);

  return api;
}
