#include "binparse.h"

#include <CodeObject.h>
#include <CodeSource.h>

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
  if (start == 0)
    obj.parse();
  else
    obj.parse(start, true);

  

  // TODO: analyze what was parsed
}