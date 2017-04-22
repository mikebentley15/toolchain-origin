#ifndef CALLTREE_H
#define CALLTREE_H

#include <CFG.h>

#include <iostream>

class CallTree {
public:
  /// Parses the function and builds the CallTree
  /// TODO: what should I do about recursive cycles?
  CallTree(
      Dyninst::ParseAPI::Function* function,
      CallTree* parent = nullptr);

  ~CallTree();

  CallTree* parent() { return _parent; }
  Dyninst::ParseAPI::Function* function() { return _function; }
  const std::vector<CallTree*>& children() { return _children; }
  
  void print_names(int indent, std::ostream &out = std::cout) const;
  void print_instructions(std::ostream &out = std::cout) const;

private:
  /// This is a private constructor to deal with recursive cycles.
  /// If the function is already found in the tree previously, then don't
  /// expand it again.
  CallTree(
      Dyninst::ParseAPI::Function* function,
      CallTree* parent,
      std::vector<CallTree*> all_ancestors);

private:
  Dyninst::ParseAPI::Function* _function;
  std::vector<CallTree*> _children;
  CallTree* _parent;
  std::vector<CallTree*> _ancestors;
};

#endif // CALLTREE_H
