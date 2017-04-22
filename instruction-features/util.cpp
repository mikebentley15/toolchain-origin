#include "util.h"

#include <dyntypes.h>
#include <CodeObject.h>
#include <Instruction.h>
#include <InstructionDecoder.h>

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

void for_each_instruction(Dyninst::ParseAPI::Function* func,
    const std::function<
      void(boost::shared_ptr<Dyninst::InstructionAPI::Instruction>,
           Dyninst::Address)
      > &operation)
{
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

std::string instruction_to_string(
    boost::shared_ptr<Dyninst::InstructionAPI::Instruction> instruction,
    Dyninst::Address addr)
{
  std::ostringstream out;
  auto op = instruction->getOperation();
  auto target = instruction->getControlFlowTarget();
  std::vector<Dyninst::InstructionAPI::Operand> args;
  instruction->getOperands(args);
  out << "Instruction("
      << op.getPrefixID() << ":" << op.getID()
      << ","
      << op.format()
      << ","
      << instruction->size()
      << ","
      << static_cast<int>(instruction->getCategory())
      << ","
      << (instruction->readsMemory() ? "t" : "f")
      << ","
      << (instruction->writesMemory() ? "t" : "f")
      << ","
      << (instruction->isValid() ? "t" : "f")
      << ","
      << (instruction->isLegalInsn() ? "t" : "f")
      << ","
      << (target != nullptr ? "t" : "f")
      << ","
      << (target != nullptr ? target->format() : "?")
      << ","
      << instruction->format(addr)
      << ")";
  auto toaddr = get_call_address(instruction, addr);
  if (toaddr > 0) {
    out << " -> " << toaddr;
  }
  addr += instruction->size();
  return out.str();
}

std::string instruction_feature_header =
  "prefixId,id,size,category,isRead,isWrite,isValid,isLegal";

std::string instruction_to_feature_string(
    boost::shared_ptr<Dyninst::InstructionAPI::Instruction> instruction,
    Dyninst::Address addr)
{
  (void)addr; // unused
  std::ostringstream out;
  auto op = instruction->getOperation();
  auto target = instruction->getControlFlowTarget();
  std::vector<Dyninst::InstructionAPI::Operand> args;
  instruction->getOperands(args);
  out << op.getPrefixID()
      << ","
      << op.getID()
      << ","
      << instruction->size()
      << ","
      << static_cast<int>(instruction->getCategory())
      << ","
      << instruction->readsMemory()
      << ","
      << instruction->writesMemory()
      << ","
      << instruction->isValid()
      << ","
      << instruction->isLegalInsn();
  return out.str();
}




