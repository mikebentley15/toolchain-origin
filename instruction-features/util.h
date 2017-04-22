#ifndef UTIL_H
#define UTIL_H

#include <Instruction.h>

#include <boost/shared_ptr.hpp>

Dyninst::Address get_call_address(
    boost::shared_ptr<Dyninst::InstructionAPI::Instruction> inst,
    Dyninst::Address base);

#endif // UTIL_H
