
#ifndef THREEGPPOHHYBRID_H
#define THREEGPPOHHYBRID_H

#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-environment.h>

namespace ns3 {

/**
 * This class implements the hyrid 3GPP reference / OH LOS path loss model used for the TUHH OUREL project.
 */
class ThreeGPPOHHybridPropagationLossModel : public PropagationLossModel
{

public:

  ThreeGPPOHHybridPropagationLossModel ();
  virtual ~ThreeGPPOHHybridPropagationLossModel ();

  // inherited from Object
  static TypeId GetTypeId (void);

  /** 
   * \param a the first mobility model
   * \param b the second mobility model
   * 
   * \return the loss in dBm for the propagation between
   * the two given mobility models
   */
  double GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

private:
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  ThreeGPPOHHybridPropagationLossModel (const ThreeGPPOHHybridPropagationLossModel &);
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   * \returns
   */
  ThreeGPPOHHybridPropagationLossModel & operator = (const ThreeGPPOHHybridPropagationLossModel &);

  Ptr<NormalRandomVariable> normRNG; // rng generator used for shadow fading computations

  // inherited from PropagationLossModel
  virtual double DoCalcRxPower (double txPowerDbm,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const;
  virtual int64_t DoAssignStreams (int64_t stream);
  
  // channel model parameters
  double m_frequency; ///< frequency in MHz
  EnvironmentType m_environment;  //!< Environment Scenario
};

} // namespace ns3


#endif // THREEGPPOHHYBRID_H

 
