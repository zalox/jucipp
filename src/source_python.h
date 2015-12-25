#ifndef JUCI_SOURCE_PYTHON_
#define JUCI_SOURCE_PYTHON_

#include "source.h"

namespace Source {
  class PythonView : public GenericView {
  public:
    PythonView(const boost::filesystem::path &file_path,
               const boost::filesystem::path &project_path,
               Glib::RefPtr<Gsv::Language> language);
    void parse_initialize();
  };
}  // namespace Source
#endif  // JUCI_SOURCE_PYTHON_