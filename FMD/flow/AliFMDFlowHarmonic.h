// -*- mode: C++ -*-
/** @file 
    @brief Declaration of a Harmonic class */
#ifndef FLOW_HARMONIC_H
#define FLOW_HARMONIC_H
#include <flow/AliFMDFlowStat.h>

/** @defgroup a_basic Basic classes for doing Flow analysis. 
    @brief This group of class handles the low-level stuff to do
    flow analysis. */
//______________________________________________________
/** @class AliFMDFlowHarmonic flow/AliFMDFlowHarmonic.h <flow/AliFMDFlowHarmonic.h>
    @brief Calculate the @f$ n^{th}@f$ order harmonic
    @ingroup a_basic 

    Calculate the @f$ n^{th}@f$ order harmonic, given by 
    @f{eqnarray*}
    v_n       &=& \frac{v_n^{obs}}{R}\\
    v_n^{obs} &=& \frac1M\sum_i^M\cos(n (\varphi_i - \Psi))
    @f}
    where @f$ R@f$ is the resolution, @f$ i@f$ runs over all @f$
    M@f$ observations of @f$ \varphi_i@f$ in all events, and
    @f$\Psi@f$ is the estimated event plane. 

    The error on the corrected value is given by 
    @f{eqnarray*} 
    \delta^2v_n & = & \left(\frac{dv_n}{dv_n^{obs}}\right)^2\delta^2
    v_n^{obs} + \left(\frac{dv_n}{dR}\right)^2\delta^2R \\ 
    & = & \frac{\delta^2v_n^{obs} R^2 + \delta^2R (v_n^{obs})^2}
    {R^4}
    @f}
*/    
class AliFMDFlowHarmonic : public AliFMDFlowStat
{
public:
  /** Constructor 
      @param n Order of the harmonic */
  AliFMDFlowHarmonic(UShort_t n) : fOrder(n) {} 
  /** Destructor */ 
  virtual ~AliFMDFlowHarmonic() {}
  /** Add a data point 
      @param phi The absolute angle @f$ \varphi \in[0,2\pi]@f$ 
      @param psi The event plane angle @f$ \Psi \in[0,2\pi] @f$ */
  void Add(Double_t phi, Double_t psi);
  /** Get the harmonic. 
      @param r   Event plane resolution 
      @param er2 Square error on event plane resolution 
      @param e2  On return the square error 
      @return The harmonic value */
  Double_t Value(Double_t r, Double_t er2, Double_t& e2) const;
  /** Get the order of the harmonic */
  UShort_t Order() const { return fOrder; }
protected:
  /** the order */ 
  UShort_t fOrder;
  /** Define for ROOT I/O */ 
  ClassDef(AliFMDFlowHarmonic,1);
};


#endif
//
// EOF
//

