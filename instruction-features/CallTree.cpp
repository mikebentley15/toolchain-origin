#include "CallTree.h"
#include "util.h"

#include <CodeObject.h>
#include <InstructionDecoder.h>

#include <algorithm>

using std::endl;

using Dyninst::ParseAPI::Function;
using Dyninst::InstructionAPI::Instruction;
using Address = Dyninst::Address;

/*** Static Methods ***/

void CallTree::inline_functions_rec(
    std::vector<boost::shared_ptr<Instruction>> &instructions,
    std::vector<std::string> backtrace,
    const std::map<Address, Function*> &func_map,
    Function* root,
    const std::function<bool(Function*)> &filter)
{
  // Go through the instructions and find the ones that make function calls.
  backtrace.push_back(root->name());
  for_each_instruction(root,
      [&instructions, &backtrace, &func_map, &filter]
      (boost::shared_ptr<Instruction> instr, Dyninst::Address addr) {
        auto toaddr = get_call_address(instr, addr);
        // if it is a call instruction, try to expand
        if (toaddr > 0) {
          auto op = instr->getOperation();
          auto called = func_map.find(toaddr);
          if (called != func_map.end()) {
            if (filter(called->second)) {
              inline_functions_rec(instructions, backtrace, func_map,
                  called->second, filter);
            }
          }
        } else { // otherwise, it is a normal instruction
          instructions.emplace_back(instr);
        }
      });
}


/*** Public Methods ***/

CallTree::CallTree(const std::map<Dyninst::Address, Function*> &func_map, Function* root)
  : CallTree(func_map, root, nullptr, {}) {}

CallTree::CallTree(
    const std::map<Dyninst::Address, Function*> &func_map,
    Function* function,
    CallTree* parent,
    std::vector<std::string> backtrace)
  : _function(function)
  , _children()
  , _parent(parent)
{
  // Go through the instructions and find the ones that make function calls.
  if (_function->name().find("std::") != 0) {
    backtrace.push_back(_function->name());
    for_each_instruction(_function,
        [&func_map, this, &backtrace]
        (boost::shared_ptr<Instruction> instr, Dyninst::Address addr) {
          auto toaddr = get_call_address(instr, addr);
          if (toaddr > 0) {
            auto op = instr->getOperation();
            auto called = func_map.find(toaddr);
            if (called != func_map.end()) {
              if (std::find(backtrace.begin(), backtrace.end(), called->second->name())
                  == backtrace.end())
              {
                this->_children.push_back(
                    new CallTree(func_map, called->second, this, backtrace));
              }
            }
          }
        });
  }
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

