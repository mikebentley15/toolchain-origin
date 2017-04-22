#include "CallTree.h"
#include "util.h"

#include <CodeObject.h>
#include <InstructionDecoder.h>

#include <iostream>

using std::endl;

using Dyninst::ParseAPI::Function;
using Dyninst::InstructionAPI::Instruction;

namespace {

/** Convenience function.  Map operation over Instructions in Function.
 *
 * Runs the given operation over every instruction found within that function.
 *
 * @param func The Function pointer to operate on
 * @param operation A callable item with two parameters:
 *   (1) boost::shared_ptr<Instruction> - the instruction itself
 *   (2) Dyninst::Address - the address of the instruction
 */
template<class Functor>
void for_each_instruction(Function* func, Functor operation) {
  for (auto blk : func->blocks()) {
    auto addr = blk->start();
    const char* buf = reinterpret_cast<char*>(blk->region()->getPtrToInstruction(addr));
    if (buf == nullptr) {
      continue;
    }

    auto arch = blk->region()->getArch();
    Dyninst::InstructionAPI::InstructionDecoder dec(buf, blk->size(), arch);

    auto instruction = dec.decode();
    while (instruction != nullptr) {
      operation(instruction, addr);
      addr += instruction->size();
      instruction = dec.decode();
    }
  }
}

} // End of unnamed namespace

/*** Public Methods ***/

CallTree::CallTree(std::map<Dyninst::Address, Function*> &func_map, Function* root)
  : CallTree(func_map, root, nullptr, {}) {}

CallTree::CallTree(
    std::map<Dyninst::Address, Function*> &func_map,
    Function* function,
    CallTree* parent,
    std::vector<CallTree*> all_ancestors)
  : _function(function)
  , _children()
  , _parent(parent)
  , _ancestors(all_ancestors)
{
  std::cout << "Building call tree for " << _function->name() << endl;
  if (parent != nullptr) {
    _ancestors.emplace_back(_parent);
  }

  // TODO: Now to parse _function to build the subtree.
  // Go through the instructions and find the ones that make function calls.
  if (_function->name().find("std::") != 0) {
    std::vector<CallTree*> new_ancestors(_ancestors);
    new_ancestors.push_back(this);
    for_each_instruction(_function,
        [&func_map, this, &new_ancestors]
        (boost::shared_ptr<Instruction> instr, Dyninst::Address addr) {
          auto toaddr = get_call_address(instr, addr);
          if (toaddr > 0) {
            auto op = instr->getOperation();
            std::cout << "  instr: +("
                      << op.getPrefixID() << ":" << op.getID()
                      << ","
                      << instr->getCategory()
                      << ","
                      << op.format()
                      << ")"
                      << endl;
            auto called = func_map.find(toaddr);
            if (called != func_map.end()) {
              this->_children.push_back(
                  new CallTree(func_map, called->second, this, new_ancestors));
            }
          }
        });
    
  }

  // Get the address of the called function.

  // Find the function pointer for the function at that address.

  // Recursively create a child node.
  
  std::cout << "-> Done building call tree for " << function->name() << endl;
}

CallTree::~CallTree() {
  for (auto child : _children) {
    delete child;
  }
  _children.clear();
}

void CallTree::print_names(int indent, std::ostream &out) const {
  // Print myself
  for (int i = 0; i < indent; i++) {
    out << " ";
  }
  out << _function->name() << std::endl;

  // Print my children
  for (auto child : _children) {
    child->print_names(indent + 1, out);
  }
}

void CallTree::print_instructions(std::ostream &out) const {
  out << "Would print instructions from " << _function->name() << endl;
  // TODO: print myself
  // TODO: print my children when I get to them
  // TODO: what format should I print these?
}

