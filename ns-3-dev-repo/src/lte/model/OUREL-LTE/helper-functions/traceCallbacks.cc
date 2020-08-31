#include "traceCallbacks.h"

std::string runID;
std::ofstream packetsRxFile;
std::ofstream packetsTxFile;

void RcvdTrace (Ptr<const Packet> packet, const Address &srcAddress, const Address &destAddress)
{
  SeqTsHeader header;
  packet->PeekHeader(header);

  //std::streambuf myfile;
  //myfile.open ("packetsRx.csv", std::fstream::app);

  //std::cout << "Time Now: " << Simulator::Now().GetSeconds() << std::endl;
  //std::cout << "Packet: " << packet->GetUid() << std::endl ;
  //std::cout << "Delay: " << (Simulator::Now().GetSeconds() - header.GetTs().GetSeconds()) << std::endl;
  packetsRxFile << srcAddress << ", " << destAddress << ", " << std::fixed << std::setprecision(6) << header.GetTs().GetSeconds() << ", " << (Simulator::Now().GetSeconds() - header.GetTs().GetSeconds()) << ", " << header.GetSeq() << std::endl;
  //myfile.close();
  //packet->Print(std::cout);
  //std::cout << std::endl << "Src Address: " << srcAddress;
  //std::cout << std::endl << "Dest Address: " << destAddress;
  //std::cout << std::endl << "+++++++++++++++++++++++++++++" << std::endl;
} 


void TransTrace (Ptr<const Packet> packet, const Address &destAddress){
  SeqTsHeader header;
  packet->PeekHeader(header);

  //std::ofstream myfile;
  //myfile.open ("packetsTx.csv", std::fstream::app);
  packetsTxFile << destAddress << ", " << std::fixed << std::setprecision(6) << header.GetTs().GetSeconds() << ", " << header.GetSeq() << std::endl;
  //myfile.close();
}



void RLCTrace(uint16_t rnti, uint8_t lcid, uint32_t bytes, uint64_t delay){
  //std::cout << "I AM HERE" << std::endl;
}
/*
void MCSTrace(const uint32_t frame, const uint32_t subframe, const uint16_t rnti, const uint8_t mcs0, const uint16_t tbs0Size, const uint8_t mcs1, const uint16_t tbs1Size, const uint8_t ccId){
  std::cout << "Frame Number: " << frame << std::endl;
  std::cout << "Subframe Numer: " << subframe << std::endl;
  std::cout << "RNTI Number: " << rnti << std::endl;
  std::cout << "MCS0 Numer: " << mcs0 << std::endl;
  std::cout << "MCS1 Number: " << mcs1 << std::endl;
}
ns3::DlSchedulingCallbackInfo
*/
void MCSTraceDL(ns3::DlSchedulingCallbackInfo info){
  /*
  std::cout << "Frame Number: " << info.frameNo << std::endl;
  std::cout << "Subframe Numer: " << info.subframeNo << std::endl;
  std::cout << "RNTI Number: " << info.rnti << std::endl;
  */
 //std::cout << "MCS0 Number: " << int(info.mcsTb1) << std::endl;
  /*std::cout << "TBS0 Number: " << int(info.sizeTb1) << std::endl;
  std::cout << "MCS1 Number: " << info.mcsTb2 << std::endl;
  std::cout << "TBS1 Number: " << info.sizeTb2 << std::endl;
  */
}

void MCSTraceUL(const uint32_t frame, const uint32_t subframe, const uint16_t rnti, const uint8_t mcs0, const uint16_t tbs0Size, const uint8_t ccID){
  //std::cout << "Frame Number: " << double(mcs0) << std::endl;
  //std::cout << "I AM HERE" << std::endl;
  /*std::cout << "Subframe Numer: " << info.subframeNo << std::endl;
  std::cout << "RNTI Number: " << info.rnti << std::endl;
  */
 //std::cout << "MCS0 Number: " << int(mcs0) << std::endl;
  /*std::cout << "TBS0 Number: " << info.sizeTb1 << std::endl;
  */
}


void PhyReceptionTrace (const PhyReceptionStatParameters params){
  //std::cout << "IMSI: " << int(params.m_imsi) << std::endl;
  //std::cout << "HARQ: " << int(params.m_rv) << std::endl;
  //std::cout << "No Error: " << int(params.m_correctness) << std::endl;
  //std::cout << "MCS: " << int(params.m_mcs) << std::endl;
  //std::cout << "***********************" << std::endl;
}