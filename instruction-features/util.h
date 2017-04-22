#ifndef UTIL_H
#define UTIL_H

#include <Instruction.h>
#include <CFG.h>

#include <boost/shared_ptr.hpp>

#include <functional>

Dyninst::Address get_call_address(
    boost::shared_ptr<Dyninst::InstructionAPI::Instruction> inst,
    Dyninst::Address base);

/** Convenience function.  Map operation over Instructions in Function.
 *
 * Runs the given operation over every instruction found within that function.
 *
 * @param func The Function pointer to operate on
 * @param operation A callable item with two parameters:
 *   (1) boost::shared_ptr<Instruction> - the instruction itself
 *   (2) Dyninst::Address - the address of the instruction
 */
void for_each_instruction(Dyninst::ParseAPI::Function* func,
    const std::function<
      void(boost::shared_ptr<Dyninst::InstructionAPI::Instruction>,
           Dyninst::Address)
      > &operation);

#endif // UTIL_H
