#include "binparse.h"

#include <CodeObject.h>
#include <CodeSource.h>
#include <CFG.h>

#include <memory>

using std::string;
using std::ostream;
using std::endl;
using std::unique_ptr;

void parse_binary(const string &infile, ostream &out, Dyninst::Address start) {
  // Copy the infile string, since the SymtabCodeSource constructor
  // takes a non-const char*, which is dumb
  // TODO: change it in dyninst and create a pull request
  auto len = infile.length();
  unique_ptr<char> infile_copy(new char[len + 1]);
  strcpy(infile_copy.get(), infile.c_str());

  out << "Testing" << endl
      << "Would have read in " << infile << endl;
  Dyninst::ParseAPI::SymtabCodeSource source(infile_copy.get());
  Dyninst::ParseAPI::CodeObject obj(&source);
  if (start == 0) {
    obj.parse();
  } else {
    obj.parse(start, false); // true);
  }

  auto funcs = obj.funcs();
  for (auto func : funcs) {
    out << func->name() << endl
        << "  Address:   " << func->addr() << endl
        << "  Is leaf:   " << func->_is_leaf_function << endl
        << "  Ret addr:  " << func->_ret_addr << endl
        << "  Is parsed: " << func->parsed() << endl
        << "  # blocks:  " << func->num_blocks() << endl
        << endl;
  }

  // TODO: analyze what was parsed
}
