#include "binparse.h"

#include <dyntypes.h>

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::unique_ptr;
using std::vector;
using std::ostream;
using std::ofstream;

void print_usage(std::ostream &out = cout) {
  out << "Usage:" << endl
      << "  feature-parse [-v] [-s <func addr>] [-o <out file path>] <binary>" << endl
      << "  feature-parse --help" << endl
      << endl
      << "Description:" << endl
      << endl
      << "  Parses a binary and generates features useful for training with" << endl
      << "  sequence-style learning models in instruction-level sequences." << endl
      << endl
      << "Options:" << endl
      << endl
      << "  -o <file> | --output <file>" << endl
      << "              Where to output the results.  By default this is to" << endl
      << "              stdout." << endl
      << endl
      << "  -s <function-address> | --start <function-address>" << endl
      << "              If specified, only parse from the given function" << endl
      << "              address rather than parsing everything.  This is done" << endl
      << "              recursively to find all function calls." << endl
      << endl
      << "              This address should be specified in hex, with or" << endl
      << "              without the 0x prefix." << endl
      << endl
      << "  -v | --verbose" << endl
      << "              Output debugging information." << endl
      << endl
      << endl;
}

Dyninst::Address hex_to_address(const char* hex_string) {
  Dyninst::Address x;   
  std::stringstream ss;
  ss << std::hex << hex_string;
  ss >> x;
  return x;
}

int main(int arg_count, char *arg_list[]){
  if (arg_count <= 1) {
    cerr << "Error: not enough arguments" << endl
         << "  Call with --help for more information" << endl;
    return 1;
  }

  ostream* out = &cout;
  unique_ptr<ofstream> fout;
  string infile;
  string outfile("stdout");
  Dyninst::Address startaddr = 0;
  bool verbose = false;

  for (int i = 1; i < arg_count; i++) {
    auto arg = string(arg_list[i]);
    if (arg == "--help" || arg == "-h") {
      print_usage();
      return 0;
    } else if (arg == "-o" || arg == "--output") {
      if (i + 1 >= arg_count) {
        cerr << "Error: you must provide an argument to " << arg << endl
             << "  Use --help for more information" << endl;
        return 1;
      }
      outfile = arg_list[++i];
      fout.reset(new ofstream(outfile));
      out = fout.get();
    } else if (arg == "-s" || arg == "--start") {
      if (i + 1 >= arg_count) {
        cerr << "Error: you must provide an argument to " << arg << endl
             << "  Use --help for more information" << endl;
        return 1;
      }
      startaddr = hex_to_address(arg_list[++i]);
    } else if (arg == "-v" || arg == "--verbose") {
      verbose = true;
    } else {
      if (i + 1 != arg_count) {
        cerr << "Error: too many arguments" << endl
             << "  Use --help for more information" << endl;
        return 1;
      }
      infile = arg;
    }
  }

  if (infile.empty()) {
    cerr << "Error: you must provide an input binary file" << endl
         << "  Use --help for more information" << endl;
    return 1;
  }
  
  if (verbose) {
    cout << "Verbose:         true" << endl
         << "Input file:      " << infile << endl
         << "Output file:     " << outfile << endl
         << "Start address:   " << startaddr << endl
         << endl;
  }

  parse_binary(infile, *out, startaddr);

  return 0;
}
