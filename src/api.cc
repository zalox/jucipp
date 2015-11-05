#include "api.h"

Terminal *Saus::api_terminal = nullptr;

Saus::Saus(Terminal *ptr) {
  api_terminal = ptr;
}

void terminal::println(const std::string &message) {
  Saus::api_terminal->print(message);
}