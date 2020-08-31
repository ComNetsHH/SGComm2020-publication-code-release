#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/mobility-model.h"
#include <cmath>
#include "ns3/random-variable-stream.h"

#include "3GPP-OH-Hybrid-propagation-loss-model.h"
#include <ns3/mobility-building-info.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ThreeGPPOHHybridPropagationLossModel");

NS_OBJECT_ENSURE_REGISTERED (ThreeGPPOHHybridPropagationLossModel);

TypeId
ThreeGPPOHHybridPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ThreeGPPOHHybridPropagationLossModel")

    .SetParent<PropagationLossModel> ()
    .SetGroupName ("Buildings")
    .AddConstructor<ThreeGPPOHHybridPropagationLossModel> ()
    .AddAttribute ("Frequency",
                   "The Frequency  (default is 2.106 GHz).",
                   DoubleValue (2160e6),
                   MakeDoubleAccessor (&ThreeGPPOHHybridPropagationLossModel::m_frequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Environment",
                   "Environment Scenario",
                   EnumValue (UrbanEnvironment),
                   MakeEnumAccessor (&ThreeGPPOHHybridPropagationLossModel::m_environment),
                   MakeEnumChecker (UrbanEnvironment, "Urban",
                                    SubUrbanEnvironment, "SubUrban",
                                    OpenAreasEnvironment, "OpenAreas"));
  return tid;
}

// Finished
ThreeGPPOHHybridPropagationLossModel::ThreeGPPOHHybridPropagationLossModel ()
  : PropagationLossModel ()
{
  normRNG = CreateObject<NormalRandomVariable> ();

}

// Finished
ThreeGPPOHHybridPropagationLossModel::~ThreeGPPOHHybridPropagationLossModel ()
{
}

double
ThreeGPPOHHybridPropagationLossModel::GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  double loss = 0.0;
  double breakingDistance = 0.0;
  double fghz = m_frequency / 1e9;
  double fmhz = m_frequency / 1e6;

  // get ut and bs height from coordinates
  double hbs = (a->GetPosition ().z > b->GetPosition ().z ? a->GetPosition ().z : b->GetPosition ().z);
  double hut = (a->GetPosition ().z < b->GetPosition ().z ? a->GetPosition ().z : b->GetPosition ().z);

  double distm = a->GetDistanceFrom (b);
 // double distkm = distm/1e3;

  switch (m_environment)
  {
  case UrbanEnvironment:
    {
      double he = 1;
      double hbsA = hbs-he;
      double hutA = hut-he;
      breakingDistance = 4*hbsA*hutA*m_frequency/3e8;

      // Too close, basically LOS and no loss
      if (distm < 10)
        {
          loss = 1;
        }
      // Pure 3GPP
      else if (distm < breakingDistance && distm < 5e3)
        {
          loss = 28.0 + 22*log10(distm) + 20*log10(fghz);
        }
      // Transistion between 3GPP and OH
      else if (distm < 5e3)
        {
          double x1 = breakingDistance;
          double x2 = 5e3;
          double y1 = 28.0 + 40*log10(x1) + 20*log10(fghz) - 9*log10(pow(breakingDistance,2) + pow(hbs-hut,2));

          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          double y2 = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(x2/1e3) - 13.82*log10(hbs) - ahm;
        
          double a = (y1-y2)/log10(x1/x2);
          double b = pow(10,((y2*log10(x1)-y1*log10(x2))/(y1-y2)));
        
          loss = a*log10(b*distm); 
        }
      // Pure OH
      else if (distm < 20e3)
        {
          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          loss = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(distm/1e3) - 13.82*log10(hbs) - ahm;
        }
      // Too far, take OH(20 000) as a constant, never actually used
      else
        {
          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          loss = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(20e3/1e3) - 13.82*log10(hbs) - ahm;
        }
      loss += normRNG->GetValue(0,4); 
      break;
    }

  case SubUrbanEnvironment:
    {
      double he = 1;
      double hbsA = hbs-he;
      double hutA = hut-he;
      breakingDistance = 4*hbsA*hutA*m_frequency/3e8;

      // Too near, basically LOS and no loss
      if (distm < 10)
        {
          loss = 1;
        }
      // Pure 3GPP
      else if (distm < breakingDistance && distm < 5e3)
        {
          loss = 28.0 + 22*log10(distm) + 20*log10(fghz);
        }
      // Transistion between 3GPP and OH
      else if (distm < 5e3)
        {
          double x1 = breakingDistance;
          double x2 = 5e3;
          double y1 = 28.0 + 40*log10(x1) + 20*log10(fghz) - 9*log10(pow(breakingDistance,2) + pow(hbs-hut,2));

          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          double y2 = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(x2/1e3) - 13.82*log10(hbs) - ahm;

          // Suburban Correction Term
          y2 =y2 - 2*pow(log10(fmhz/28),2) - 5.4;
        
          double a = (y1-y2)/log10(x1/x2);
          double b = pow(10,((y2*log10(x1)-y1*log10(x2))/(y1-y2)));
        
          loss = a*log10(b*distm); 
        }
      // Pure OH
      else if (distm < 20e3)
        {
          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          loss = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(distm/1e3) - 13.82*log10(hbs) - ahm;
          // Rural Correction Term
          loss = loss - 2*pow(log10(fmhz/28),2) - 5.4;
        }
      // Too far, take OH(20 000) as a constant
      else
        {
          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          loss = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(20e3/1e3) - 13.82*log10(hbs) - ahm;
          // Rural Correction Term
          loss = loss - 2*pow(log10(fmhz/28),2) - 5.4;
        }
      loss += normRNG->GetValue(0,4); 
      break;
    }

  case OpenAreasEnvironment:
    {
      double avgBuildingHeight = 5;
      breakingDistance = 2*M_PI*hbs*hut*m_frequency/3e8;

      // Too close, basically LOS and no loss
      if (distm < 10)
        {
          loss = 1;
        }
      // Pure 3GPP
      else if (distm < breakingDistance && distm < 5e3)
        {
          loss = 20*log10(40*M_PI*distm*fghz/3) + std::min(0.03*pow(avgBuildingHeight,1.72),10.0)*log10(distm) - std::min(0.044*pow(avgBuildingHeight,1.72),14.77) + 0.002*log10(avgBuildingHeight)*distm;
        }
      // Transistion between 3GPP and OH
      else if (distm < 5e3)
        {
          double x1 = breakingDistance;
          double x2 = 10e3;
          double y1 = 20*log10(40*M_PI*breakingDistance*fghz/3) + std::min(0.03*pow(avgBuildingHeight,1.72),10.0)*log10(breakingDistance) - std::min(0.044*pow(avgBuildingHeight,1.72),14.77) + 0.002*log10(avgBuildingHeight)*breakingDistance;

          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          double y2 = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(x2/1e3) - 13.82*log10(hbs) - ahm;
        
          // Rural Correction Term
          y2 += 18.33*log10(fmhz) -4.78*pow(log10(fmhz),2) - 40.94;

          double a = (y1-y2)/log10(x1/x2);
          double b = pow(10,((y2*log10(x1)-y1*log10(x2))/(y1-y2)));
        
          loss = a*log10(b*distm);  
        }
      // Pure OH
      else if (distm < 20e3)
        {
          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          loss = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(distm/1e3) - 13.82*log10(hbs) - ahm;
          // Rural Correction Term
          loss += 18.33*log10(fmhz) -4.78*pow(log10(fmhz),2) - 40.94;
        }
      // Too far, take OH(20 000) as a constant
      else
        {
          double ahm = (1.1*log10(fmhz)-0.7)*hut-(1.56*log10(fmhz)-0.8);
          loss = 69.55 + 26.16*log10(fmhz) + (44.9 - 6.55*log10(hbs))*log10(20e3/1e3) - 13.82*log10(hbs) - ahm;
          // Rural Correction Term
          loss += 18.33*log10(fmhz) -4.78*pow(log10(fmhz),2) - 40.94;
        }
        loss += normRNG->GetValue(0,6); 
      break;
    }

  default:
    loss = 0;
    break;
  }

  // Add building entry loss
  if(fmhz <= 800) {
    loss+= 21.5;
  }
  else if(fmhz <= 1800) {
    loss+= 23.8;
  }
  else if(fmhz <= 2600) {
    loss+= 29.3;
  }
  else if(fmhz <= 3500) {
    loss+= 33.4;
  }
  else if(fmhz <= 5800) {
    loss+= 36.8;
  }
  else{
    loss += 46.1;
  }
  return loss;
}

// Finished
double 
ThreeGPPOHHybridPropagationLossModel::DoCalcRxPower (double txPowerDbm,
						Ptr<MobilityModel> a,
						Ptr<MobilityModel> b) const
{  
  return (txPowerDbm - GetLoss (a, b));
}

// Finished
int64_t
ThreeGPPOHHybridPropagationLossModel::DoAssignStreams (int64_t stream)
{
  return 0;
}


} // namespace ns3
