#include "binparse.h"

#include <CodeObject.h>
#include <CodeSource.h>
#include <CFG.h>
#include <InstructionDecoder.h>

#include <memory>

using std::string;
using std::ostream;
using std::endl;
using std::unique_ptr;

using Funclist = Dyninst::ParseAPI::CodeObject::funclist;

namespace {
void print_function(Dyninst::ParseAPI::Function* func, ostream &out) {
  out << func->name() << endl
      << "  Address:   0x" << std::hex << func->addr() << std::dec << endl
      //<< "  Is leaf:   " << func->_is_leaf_function << endl
      //<< "  Ret addr:  " << func->_ret_addr << endl
      << "  Is parsed: " << func->parsed() << endl
      << "  # blocks:  " << func->num_blocks() << endl
      << "  Block expansion: " << endl;
  for (auto blk : func->blocks()) {
    auto addr = blk->start();
    const char* buf = reinterpret_cast<char*>(blk->region()->getPtrToInstruction(addr));
    if (buf == nullptr) {
      continue;
    }
    out << "    Block size: " << blk->size() << endl;
    
    // TODO: look at block interface
    Dyninst::InstructionAPI::InstructionDecoder
      dec(buf, blk->size(), blk->region()->getArch());
    
    // TODO: look at instruction
    auto instruction = dec.decode();
    while (instruction != nullptr) {
      auto op = instruction->getOperation();
      out << "      +("
          << op.getPrefixID() << ":" << op.getID() << ":" << op.format()
          << ","
          << instruction->size()
          << ","
          << (instruction->readsMemory() ? "t" : "f")
          << ","
          << (instruction->writesMemory() ? "t" : "f")
          << ","
          << (instruction->isValid() ? "t" : "f")
          << ","
          << (instruction->isLegalInsn() ? "t" : "f")
          << ","
          << (instruction->getControlFlowTarget() != nullptr ? "t" : "f")
          << ","
          << static_cast<int>(instruction->getCategory())
          << ")"
          << endl;
      instruction = dec.decode();
    }
  }
  out << endl << endl;
}

template <class SetType, class UnaryPredicate>
void remove_if_set (SetType &set, UnaryPredicate pred)
{
  auto start = set.begin();
  auto end = set.end();
  auto current = start;
  while (current != end) {
    if (!pred(*current)) {
      current = set.erase(current);
    } else {
      ++current;
    }
  }
}

bool string_ends_with(const string &full_string, const string &ending) {
    if (full_string.length() >= ending.length()) {
        return (0 == full_string.compare (full_string.length() - ending.length(),
                                         ending.length(), ending));
    } else {
        return false;
    }
}

} // End of unnamed namespace

void parse_binary(const string &infile, ostream &out, Dyninst::Address start) {
  // Copy the infile string, since the SymtabCodeSource constructor
  // takes a non-const char*, which is dumb
  // TODO: change it in dyninst and create a pull request
  auto len = infile.length();
  unique_ptr<char> infile_copy(new char[len + 1]);
  strcpy(infile_copy.get(), infile.c_str());

  out << "Reading in " << infile << endl;
  Dyninst::ParseAPI::SymtabCodeSource source(infile_copy.get());
  Dyninst::ParseAPI::CodeObject obj(&source);
  if (start == 0) {
    obj.parse();
  } else {
    // TODO: what does this actually do?
    obj.parse(start, true);
  }

  Funclist funcs(obj.funcs());
  //for (auto func : funcs) {
  //  print_function(func, out);
  //}

  Funclist unittests(funcs);
  remove_if_set(unittests, [] (const Dyninst::ParseAPI::Function* func) {
      return string_ends_with(func->name(), "::run_impl");
      });
  for (auto func : unittests) {
    print_function(func, out);
  }
  // TODO: analyze what was parsed
}
