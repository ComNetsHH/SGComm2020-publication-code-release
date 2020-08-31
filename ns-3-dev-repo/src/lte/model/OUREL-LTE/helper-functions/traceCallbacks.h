#include <iostream>
#include <ns3/core-module.h>
// Tracing Includes
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/seq-ts-header.h"
#include "ns3/node.h"
#include "ns3/lte-common.h"
#include <iomanip>

using namespace ns3;
//std::stringstream transTraceStr;

void RcvdTrace (Ptr<const Packet> packet, const Address &srcAddress, const Address &destAddress);
void TransTrace (Ptr<const Packet> packet, const Address &destAddress);

void RLCTrace (uint16_t rnti, uint8_t lcid, uint32_t bytes, uint64_t delay);

void MCSTraceDL(ns3::DlSchedulingCallbackInfo info);
void MCSTraceUL(const uint32_t frame, const uint32_t subframe, const uint16_t rnti, const uint8_t mcs0, const uint16_t tbs0Size, const uint8_t ccId);

void PhyReceptionTrace (const PhyReceptionStatParameters params);