//-----------------------------------------------------------------------bl-
//--------------------------------------------------------------------------
//
// Antioch - A Gas Dynamics Thermochemistry Library
//
// Copyright (C) 2014-2016 Paul T. Bauman, Benjamin S. Kirk,
//                         Sylvain Plessis, Roy H. Stonger
//
// Copyright (C) 2013 The PECOS Development Team
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the Version 2.1 GNU Lesser General
// Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc. 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301  USA
//
//-----------------------------------------------------------------------el-

#ifndef ANTIOCH_IDEAL_GAS_MICRO_THERMO_H
#define ANTIOCH_IDEAL_GAS_MICRO_THERMO_H

// Antioch
#include "antioch/macro_micro_thermo_base.h"

namespace Antioch
{

  template <typename CoeffType>
  class TempCache;

  //! Deprecated, prefer IdealGasThermo class
  template <typename MacroThermo, typename CoeffType = double>
  class IdealGasMicroThermo : public MacroMicroThermoBase<CoeffType,IdealGasMicroThermo<MacroThermo,CoeffType> >
  {
  public:

    IdealGasMicroThermo(const MacroThermo & ext_thermo, const ChemicalMixture<CoeffType> & chem_mix)
      : MacroMicroThermoBase<CoeffType,IdealGasMicroThermo<MacroThermo,CoeffType> >(chem_mix),
      _ext_therm(ext_thermo)
    {antioch_deprecated();}

    virtual ~IdealGasMicroThermo(){}

    // Friend the base class so we can make the CRTP implementation functions
    // private.
    friend class  MacroMicroThermoBase<CoeffType,IdealGasMicroThermo<MacroThermo,CoeffType> >;

  private:

    const MacroThermo & _ext_therm;

    //! Implementation of species vibrational specific heat, [J/kg-K]
    template <typename StateType>
    const ANTIOCH_AUTO(StateType)
    cv_vib_impl(unsigned int s, const StateType & T) const
    ANTIOCH_AUTOFUNC(StateType, (this->_chem_mixture.chemical_species()[s]->n_tr_dofs() < CoeffType(2.))
                                ? zero_clone(T) : _ext_therm.cv(TempCache<StateType>(T),s) - this->cv_tr(s) )

    //! Implementation of normalized species vibrational specific heat.
    template <typename StateType>
    const ANTIOCH_AUTO(StateType)
    cv_vib_over_R_impl(unsigned int s, const StateType & T) const
    ANTIOCH_AUTOFUNC(StateType, (this->_chem_mixture.chemical_species()[s]->n_tr_dofs() < CoeffType(2.))
                                ? zero_clone(T) :  _ext_therm.cv_over_R(TempCache<StateType>(T),s) - this->cv_tr_over_R(s) )

    //! Implementation of species electronic specific heat, [J/kg-K]
    /*! For this class, we assume there is no population of electronic states, so cv_el is
        just zero. */
    template<typename StateType>
    StateType cv_el_impl (const unsigned int /*species*/, const StateType & T) const
    {
      return zero_clone(T);
    }

  };

} // end namespace Antioch


#endif
