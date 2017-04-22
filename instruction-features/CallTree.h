#ifndef CALLTREE_H
#define CALLTREE_H

#include <CFG.h>

#include <iostream>
#include <map>

/** Represents a call tree from the static instructions */
class CallTree {
public:
  /// TODO: what should I do about recursive cycles?

  /** Parses the function and builds the CallTree */
  CallTree(std::map<Dyninst::Address, Dyninst::ParseAPI::Function*> &func_map,
           Dyninst::ParseAPI::Function* function);

  /** This is the main constructor used internally
   *
   * If the function is already found in the tree previously, then don't
   * expand it again.
   */
  CallTree(
      std::map<Dyninst::Address, Dyninst::ParseAPI::Function*> &func_map,
      Dyninst::ParseAPI::Function* function,
      CallTree* parent,
      std::vector<CallTree*> all_ancestors);

  ~CallTree();

  // Getters
  CallTree* parent() { return _parent; }
  Dyninst::ParseAPI::Function* function() { return _function; }
  const std::vector<CallTree*>& children() { return _children; }
 
  /** Prints the function names in a tree-like fashion. */
  void print_names(int indent, std::ostream &out = std::cout) const;

  /** Prints the instructions in a sequential fashion. */
  void print_instructions(std::ostream &out = std::cout) const;

private:
  Dyninst::ParseAPI::Function* _function;
  std::vector<CallTree*> _children;
  CallTree* _parent;
  std::vector<CallTree*> _ancestors;
};

#endif // CALLTREE_H
