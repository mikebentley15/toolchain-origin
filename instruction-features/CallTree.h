#ifndef CALLTREE_H
#define CALLTREE_H

#include <CFG.h>
#include <Instruction.h>
#include <dyntypes.h>

#include <boost/shared_ptr.hpp>

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

/** Represents a call tree from the static instructions */
class CallTree {
public:

  /** Parses the function and builds the CallTree */
  CallTree(const std::map<Dyninst::Address, Dyninst::ParseAPI::Function*> &func_map,
           Dyninst::ParseAPI::Function* function);

  /** This is the main constructor
   *
   * If the function is already found in the tree previously, then don't
   * expand it again.
   */
  CallTree(
      const std::map<Dyninst::Address, Dyninst::ParseAPI::Function*> &func_map,
      Dyninst::ParseAPI::Function* function,
      CallTree* parent,
      std::vector<std::string> backtrace);

  ~CallTree();

  // Getters
  CallTree* parent() { return _parent; }
  Dyninst::ParseAPI::Function* function() { return _function; }
  const std::vector<CallTree*>& children() { return _children; }
 
  /** Prints the function names in a tree-like fashion. */
  void print_names(int indent, std::ostream &out = std::cout) const;

  /** Takes a function and returns a vector of instructions after inlining
   *
   * This version is a convenience for calling print_instructions when
   * you do not want to filter any of the functions out.
   *
   * @param func_map - map from Address -> Function*
   * @param root - Starting function
   */
  static std::vector<boost::shared_ptr<Dyninst::InstructionAPI::Instruction>>
  inline_functions(
      const std::map<Dyninst::Address, Dyninst::ParseAPI::Function*> &func_map,
      Dyninst::ParseAPI::Function* root)
  {
    return inline_functions(
        func_map, root, [](Dyninst::ParseAPI::Function*) { return true; });
  }

  /** Takes a function and returns a vector of instructions after inlining
   *
   * You may provide a filter function that takes a Function* and returns a
   * boolean.
   *
   * Note: no function calls show up in the output.  The ones that are recursed
   * are inlined, and the ones that are filtered away simply disappear.
   *
   * @param func_map - map from Address -> Function*
   * @param root - Starting function
   * @param filter - Filter function to not recurse past certain functions
   *    An output of true means it should be expanded.
   */
  static std::vector<boost::shared_ptr<Dyninst::InstructionAPI::Instruction>>
  inline_functions(
      const std::map<Dyninst::Address, Dyninst::ParseAPI::Function*> &func_map,
      Dyninst::ParseAPI::Function* root,
      const std::function<bool(Dyninst::ParseAPI::Function*)> &filter)
  {
    std::vector<boost::shared_ptr<Dyninst::InstructionAPI::Instruction>> instructions;
    inline_functions_rec(instructions, {}, func_map, root, filter);
    return instructions;
  }

private:
  /** Recursive implementation of inline_functions() */
  static void inline_functions_rec(
      std::vector<boost::shared_ptr<Dyninst::InstructionAPI::Instruction>> &instructions,
      std::vector<std::string> backtrace,
      const std::map<Dyninst::Address, Dyninst::ParseAPI::Function*> &func_map,
      Dyninst::ParseAPI::Function* root,
      const std::function<bool(Dyninst::ParseAPI::Function*)> &filter);

private:
  Dyninst::ParseAPI::Function* _function;
  std::vector<CallTree*> _children;
  CallTree* _parent;
};

#endif // CALLTREE_H
