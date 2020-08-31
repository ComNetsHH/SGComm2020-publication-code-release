#include <iostream>
#include <ns3/core-module.h>

using namespace ns3;

std::vector<std::pair<std::string, std::vector<double>>> read_csv(std::string filename);