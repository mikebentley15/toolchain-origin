#include <dyntypes.h>

#include <string>
#include <iostream>

void parse_binary(
    const std::string &infile,
    std::ostream &out = std::cout,
    Dyninst::Address start = 0);

