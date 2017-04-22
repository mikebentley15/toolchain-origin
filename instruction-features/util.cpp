#include "util.h"

#include "dyntypes.h"

using Dyninst::InstructionAPI::Instruction;
using Dyninst::MachRegister;
using Dyninst::InstructionAPI::Expression;
using Dyninst::InstructionAPI::RegisterAST;
using Dyninst::InstructionAPI::Result;

using Address = Dyninst::Address;

Dyninst::Address get_call_address(
    boost::shared_ptr<Instruction> inst,
    Address base)
{
  if (inst->getCategory() != Dyninst::InstructionAPI::c_CallInsn) {
    return 0;
  }

  auto target = inst->getControlFlowTarget();
  Dyninst::Address addr = 0;
  auto arch = inst->getArch();

  Expression::Ptr thePC(new RegisterAST(MachRegister::getPC(arch)));
  target->bind(thePC.get(), Result(Dyninst::InstructionAPI::u32, base));
  Result res = target->eval();
  if (res.defined) {
    addr = res.convert<uintmax_t>();
  }

  return addr;
}

