/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Leonard Fisser <leonard.fisser@tuhh.de>
 */

// Simple LTE Includes
#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>
#include <ns3/config.h>

#include "ns3/config-store.h"

// EPC Includes
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

// Tracing Includes
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/seq-ts-header.h"
#include <iostream>

// REM Includes
#include <ns3/buildings-module.h>
#include <ns3/radio-environment-map-helper.h>
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/buildings-helper.h>

// Config and Animation Includes
//#include "ns3/gtk-config-store.h"
#include "ns3/netanim-module.h"
#include <string>

// Function includes
#include "ns3/helperfunctions.h"
#include "ns3/traceCallbacks.h"

using namespace ns3;

int runID = 1;

std::ofstream packetsRxFile;
std::ofstream packetsTxFile;

int main (int argc, char *argv[])
{

    packetsRxFile.open ("packetsRx.csv");
    packetsTxFile.open ("packetsTx.csv");

    // Add Meta data to all packets for further debug
    PacketMetadata::Enable();
    Packet::EnablePrinting();

    // Static Configuration
    int numEnodeBs = 3;
    int numUEs = 0;
    int DlEarfcn = 9895;
    int UlEarfcn;
    int rlcMode = 2;
    double interPacketIntervalDouble = 0.1;
    std::string scenarioStr;

    Config::SetDefault ("ns3::LteHelper::Scheduler", StringValue("ns3::TdBetFfMacScheduler"));
    Config::SetDefault ("ns3::RadioEnvironmentMapHelper::Earfcn", UintegerValue(100));

    Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue(6));
    Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue(6));

    Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(3)); // UM:2 AM:3
    Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue(640));
    Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue(38)); // 3GPP 
    Config::SetDefault ("ns3::LteEnbPhy::NoiseFigure", DoubleValue(2)); // LTE700
    Config::SetDefault ("ns3::LteEnbRrc::DefaultTransmissionMode", UintegerValue (0)); // 0: SISO, 1: MIMO Div, 2: MIMO PLEX

    Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue(23)); // Class3 3GPP
    Config::SetDefault ("ns3::LteUePhy::NoiseFigure", DoubleValue(7)); // handbook
    Config::SetDefault ("ns3::LteUePhy::EnableUplinkPowerControl", BooleanValue (false));

    Config::SetDefault ("ns3::LteEnbMac::NumberOfRaPreambles", UintegerValue(64));
    // Comand line parser and config store
    CommandLine cmd;
    cmd.AddValue("runID", "String identifier to store results.", runID );
    cmd.AddValue("numNodes", "Number of UEs taken from the nodelist.csv", numUEs);
    cmd.AddValue("DlEarfcn", "DlEarfcn", DlEarfcn);
    cmd.AddValue("RlcMode", "RlcMode", rlcMode);
    cmd.AddValue("Scenario", "Scenario", scenarioStr);
    cmd.AddValue("Interval", "Interval", interPacketIntervalDouble);


    cmd.Parse (argc, argv);
    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults ();
    // parse again so you can override default values from the command line
    cmd.Parse (argc, argv);

    if (DlEarfcn == 9895){
      UlEarfcn = 27785;
      Config::SetDefault ("ns3::ThreeGPPOHHybridPropagationLossModel::Frequency", DoubleValue(450000000));
    }
    else if (DlEarfcn == 100){
      UlEarfcn = 18100;
      Config::SetDefault ("ns3::ThreeGPPOHHybridPropagationLossModel::Frequency", DoubleValue(2110000000));

    }
    else{
      std::cout << "Earfcn not possibile, please check your configuraiton." << std::endl;
      return 0;
    }

    if(!strcmp(scenarioStr.c_str(),"Urban")){
      Config::SetDefault ("ns3::ThreeGPPOHHybridPropagationLossModel::Environment",  EnumValue(UrbanEnvironment));
      std::cout << "Urban" << std::endl;

    }
    else if (!strcmp(scenarioStr.c_str(),"Suburban")){
      Config::SetDefault ("ns3::ThreeGPPOHHybridPropagationLossModel::Environment",  EnumValue(SubUrbanEnvironment));
      std::cout << "Suburban" << std::endl;
    }
    else{
      Config::SetDefault ("ns3::ThreeGPPOHHybridPropagationLossModel::Environment",  EnumValue(OpenAreasEnvironment));
      std::cout << "Rural" << std::endl;
    }
      
    Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue(DlEarfcn)); // 9895
    Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue(UlEarfcn)); // 27785
    Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue(DlEarfcn)); // 9895
    Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(rlcMode)); // UM:2 AM:3

    Time interPacketInterval = Seconds(interPacketIntervalDouble);
    UintegerValue packetSizeDL = 8+4+2+1+8+2*9; // UDP header + timestamp (second) + Node ID + Control + APP timestamp (millisecond) + 8 x Doubles
    UintegerValue packetSizeUL = 8+4+2+1+8+8*9; // UDP header + timestamp (second) + Node ID + Control + APP timestamp (millisecond) + 2 x Doubles

    // Creates the LTE Helper object
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
    // Create p2p epc helper for epc network
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
    // Add p2p epc helper to lteHelper
    lteHelper->SetEpcHelper (epcHelper);
    // Add packet gate way
    Ptr<Node> pgw = epcHelper->GetPgwNode ();

    // Pathloss Model
    lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::ThreeGPPOHHybridPropagationLossModel"));
    lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");
    lteHelper->SetFfrAlgorithmType ("ns3::LteFrHardAlgorithm");

    // Create a single RemoteHost as a server behind the packet gate way
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create (1);
    Ptr<Node> remoteHost = remoteHostContainer.Get (0);
    // Install the Internet stack into the remote host
    InternetStackHelper internet;
    internet.Install (remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
    p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (0)));
    // create internetDevices and install a p2p connection between the packet gate way and the remote host
    NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
    
    // Create Ipv4 helper and configure with server and mask
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
    // Assign addresses to PGW and remote host
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
    // interface 0 is localhost, 1 is the p2p device (nics)
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

    // Create ipv4 routing helper
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    // Get static routes from the remoteHost
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
    remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
    
    // Create nodes 
    NodeContainer enbNodes;
    enbNodes.Create (numEnodeBs);
    NodeContainer ueNodes;
    ueNodes.Create (numUEs);

    MobilityHelper mobility;
    
    Ptr<Node> sgw = epcHelper->GetSgwNode ();
    // Place PGW below eNodeB
    Ptr<ListPositionAllocator> positionAllocPGW = CreateObject<ListPositionAllocator> ();
    positionAllocPGW->Add (Vector (0, 10, 25));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positionAllocPGW);
    mobility.Install (pgw);
    
    // Place remote host below PGW
    Ptr<ListPositionAllocator> positionAllocHost = CreateObject<ListPositionAllocator> ();
    positionAllocHost->Add (Vector (0, 20, 10));
    mobility.SetPositionAllocator(positionAllocHost);
    mobility.Install (remoteHost);

    Ptr<ListPositionAllocator> positionAllocSGW = CreateObject<ListPositionAllocator> ();
    positionAllocSGW->Add (Vector (0, 30, 10));
    mobility.SetPositionAllocator(positionAllocSGW);
    mobility.Install (sgw);

    Ptr<ListPositionAllocator> positionAllocMME = CreateObject<ListPositionAllocator> ();
    positionAllocMME->Add (Vector (0, 40, 10));
    mobility.SetPositionAllocator(positionAllocMME);
    auto MME =  NodeList::GetNode(2);
    mobility.Install(MME);
    
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (ueNodes);
    
    double xMax = 0;
    double yMax = 0;
    std::vector<std::pair<std::string, std::vector<double>>> positionMatrix = read_csv("../../scratch/simulation-model/nodeLocation" + scenarioStr + ".csv");
    for (uint n=0 ; n < positionMatrix[0].second.size() ; n++)
    {
        double x = positionMatrix[0].second[n];
        double y = positionMatrix[1].second[n];
        if(x > xMax){
          xMax = x;
        }
        if(y > yMax){
          yMax = y;
        }

        if( n < ueNodes.GetN()){
          Ptr<ConstantPositionMobilityModel> mob = ueNodes.Get(n)->GetObject<ConstantPositionMobilityModel>();
          mob->SetPosition(Vector(x,y,1.5));
        }
    }

    Ptr<ListPositionAllocator> positionAllocEnodeB = CreateObject<ListPositionAllocator> ();
    positionAllocEnodeB->Add (Vector (xMax/2, yMax/2, 35));
    mobility.SetPositionAllocator(positionAllocEnodeB);
    mobility.Install (enbNodes);
    
    // Install the LTE enb stack into the empty enbNodes
    NetDeviceContainer enbDevs;
    lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
    lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (0));
    lteHelper->SetEnbAntennaModelAttribute ("Beamwidth",   DoubleValue (63));
    lteHelper->SetEnbAntennaModelAttribute ("MaxGain",     DoubleValue (17.0));
    lteHelper->SetFfrAlgorithmAttribute ("FrCellTypeId", UintegerValue (1));
    lteHelper->SetFfrAlgorithmAttribute ("DlSubBandOffset", UintegerValue (0));
    lteHelper->SetFfrAlgorithmAttribute ("DlSubBandwidth", UintegerValue (2));
    lteHelper->SetFfrAlgorithmAttribute ("UlSubBandOffset", UintegerValue (0));
    lteHelper->SetFfrAlgorithmAttribute ("UlSubBandwidth", UintegerValue (2));
    enbDevs =  lteHelper->InstallEnbDevice (enbNodes.Get (0));

    lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (360/3));
    lteHelper->SetFfrAlgorithmAttribute ("FrCellTypeId", UintegerValue (2));
    lteHelper->SetFfrAlgorithmAttribute ("DlSubBandOffset", UintegerValue (2));
    lteHelper->SetFfrAlgorithmAttribute ("UlSubBandOffset", UintegerValue (2));
    enbDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (1)));

    lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (2*360/3));
    lteHelper->SetFfrAlgorithmAttribute ("FrCellTypeId", UintegerValue (3));
    lteHelper->SetFfrAlgorithmAttribute ("DlSubBandOffset", UintegerValue (4));
    lteHelper->SetFfrAlgorithmAttribute ("UlSubBandOffset", UintegerValue (4));
    enbDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (2)));

    // Install the LTE ue stack into the empty ueNodes
    NetDeviceContainer ueDevs;
    ueDevs = lteHelper->InstallUeDevice (ueNodes);

    // Install the IP stack on the UEs
    internet.Install (ueNodes);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

    // Assign IP address to UEs, and install applications
    for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);

      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      // the default route for all packets will be the PGW through the eNodeB
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

    // Install and start applications on UEs and remote host
    // ports
    uint16_t dlPort = 1100;
    uint16_t ulPort = 2000;
    // Create application containers
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;
    
    Ptr<UniformRandomVariable> uniRNG = CreateObject<UniformRandomVariable> ();
    uniRNG->SetAttribute ("Min", DoubleValue (0));
    uniRNG->SetAttribute ("Max", DoubleValue (0.1*interPacketIntervalDouble*1000));
    //double timingIncrement = interPacketIntervalDouble/double(numUEs);

    std::ofstream addressFile;
    addressFile.open ("addressLocations.csv");
    // Take every UE
    for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
      {
        //Create UDP packet sink
        PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (ueIpIface.GetAddress(u), dlPort));
        // Install packet sink app on ue and add to server apps
        serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get (u)));

        // Create UDP packet source with the adderss from the UE sink app as destination and port
        UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
        // set parameters
        dlClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
        dlClient.SetAttribute ("MaxPackets", UintegerValue (4294967295U));
        dlClient.SetAttribute ("MaxPackets", UintegerValue (1));

        dlClient.SetAttribute ("PacketSize", packetSizeDL);
        //dlClient.SetAttribute ("StartTime", TimeValue(MilliSeconds(300 + 0)));
        dlClient.SetAttribute ("StartTime", TimeValue(Minutes(300 + 0)));

        // Add UDP packet source to the server behind the pgw
        clientApps.Add (dlClient.Install (remoteHost));

        // increment port to ensure unique port numbers
        ++ulPort;
        
        // Create UDP packet sink
        PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (remoteHostAddr, ulPort));
        // Install packet sink app on server and add to server apps
        serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
    
        // Create UDP packet source with the adderss from the server sink app as destination and port
        UdpClientHelper ulClient (remoteHostAddr, ulPort);
        // set parameters
        ulClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
        ulClient.SetAttribute ("MaxPackets", UintegerValue (4294967295U));
        ulClient.SetAttribute ("MaxPackets", UintegerValue (4294967295U));

        ulClient.SetAttribute ("PacketSize", packetSizeUL);
        //ulClient.SetAttribute ("StartTime", TimeValue(MilliSeconds(300 + 1000*timingIncrement*u)));
        ulClient.SetAttribute ("StartTime", TimeValue(MilliSeconds(2000 + uniRNG->GetInteger())));
        // Find position of UE in nodeList and pass to the application
        //ulClient.SetAttribute ("NumberUE", UintegerValue(ueNodes.Get (u)->GetId()));

        // Add UDP packet source to the UEs
        clientApps.Add (ulClient.Install (ueNodes.Get(u)));

        AddressValue mac;
        ueNodes.Get (u)->GetApplication(0)->GetAttribute("Local", mac);
        remoteHost->GetApplication(2*u)->SetAttribute("RemoteAddressTrace", AddressValue(mac.Get()));
        Ptr<ConstantPositionMobilityModel> mob = ueNodes.Get(u)->GetObject<ConstantPositionMobilityModel>();
        Ptr<LteUeNetDevice> netd = ueNodes.Get(u)->GetDevice(0)->GetObject<LteUeNetDevice>();

        //netd->GetMac()->SetAttribute("lastValue", UintegerValue(u*runID));
        

        addressFile << mac.Get() << ", " << netd->GetImsi () << ", " << mob->GetPosition().x << ", " << mob->GetPosition().y << ", " << positionMatrix[2].second[u] << ", " << positionMatrix[3].second[u] << std::endl;
        remoteHost->GetApplication(2*u+1)->GetAttribute("Local", mac);
        ueNodes.Get (u)->GetApplication(1)->SetAttribute("RemoteAddressTrace", AddressValue(mac.Get()));
        addressFile << mac.Get() << ", " << netd->GetImsi () << ", " << mob->GetPosition().x << ", " << mob->GetPosition().y << ", " << positionMatrix[2].second[u] << ", " << positionMatrix[3].second[u] << std::endl;
        clientApps.Get(u)->GetAttribute("RemoteAddress", mac);
      }

    addressFile << 0 << ", " << 0 << ", " << xMax/2 << ", " << yMax/2 << ", " << 0 << ", " << 0 << std::endl;
    addressFile.close();

    // Start the apps after 0.5s to allow for initialization
    serverApps.Start (MilliSeconds (300));
    // Disable serverApps    
    clientApps.Stop(Minutes(10));
    
    // Tracing
    // Custom Tracing
    Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::PacketSink/RxWithAddresses", MakeCallback (&RcvdTrace));
    Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::UdpClient/TxWithAddress", MakeCallback (&TransTrace));

    Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/ComponentCarrierMap/*/LteEnbMac/DlScheduling", MakeCallback (&MCSTraceDL));
    Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/ComponentCarrierMap/*/LteEnbMac/UlScheduling", MakeCallback (&MCSTraceUL));

    Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/UlSpectrumPhy/UlPhyReception", MakeCallback (&PhyReceptionTrace));
    Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/DlSpectrumPhy/DlPhyReception", MakeCallback (&PhyReceptionTrace));
    
    Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/UlSpectrumPhy/DlPhyReception", MakeCallback (&PhyReceptionTrace));
    Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/DlSpectrumPhy/UlPhyReception", MakeCallback (&PhyReceptionTrace));

    lteHelper->EnablePhyTraces ();
    lteHelper->EnableMacTraces ();

    // Attach the UEs to the eNodeB, this will configure the UEs according to the configuration of the eNodeB and creates RRC connections between both
    // side effect: the default EPS bearer will be activated
    lteHelper->Attach(ueDevs);

    // Clarify If thoose are needed?
    BuildingsHelper::Install(enbNodes);
    Ptr<EpcTft> tft = Create<EpcTft> ();
    EpcTft::PacketFilter ulpf;
    ulpf.remotePortStart = ulPort;
    ulpf.remotePortEnd = ulPort;
    tft->Add (ulpf);
    EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
    lteHelper->ActivateDedicatedEpsBearer (ueDevs, bearer, tft);
    lteHelper->AddX2Interface (enbNodes);

    // Run and finish simulation
    Simulator::Stop (Seconds(20));
    Simulator::Run ();

    packetsRxFile.close();
    packetsTxFile.close();

    Simulator::Destroy ();
    return 0;
}