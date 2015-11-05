#ifndef JUCI_API_H_
#define JUCI_API_H_

#include <string>
#include "terminal.h"

class Saus {
public:
  Saus(Terminal *t);
  static Terminal *api_terminal;
};

class terminal {
public:
  void println(const std::string &message);
};

#endif