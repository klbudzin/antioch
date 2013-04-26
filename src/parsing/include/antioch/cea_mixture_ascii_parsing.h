//-----------------------------------------------------------------------bl-
//--------------------------------------------------------------------------
// 
// Antioch - A Gas Dynamics Thermochemistry Library
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
//
// $Id$
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

#ifndef ANTIOCH_CEA_MIXTURE_ASCII_PARSING_H
#define ANTIOCH_CEA_MIXTURE_ASCII_PARSING_H

// C++
#include <iostream>
#include <vector>

// Antioch
#include "antioch/input_utils.h"
#include "antioch/chemical_mixture.h"
#include "antioch/cea_mixture.h"

namespace Antioch
{
  // Forward declarations
  template <class NumericType>
  class CEAThermoMixture;

  // New declarations

  template<class NumericType>
  void read_cea_mixture_data_ascii( CEAThermoMixture<NumericType>& thermo, std::istream &in );

  template<class NumericType>
  void read_cea_mixture_data_ascii_default( CEAThermoMixture<NumericType>& thermo );

 
  /* ------------------------- Inline Functions -------------------------*/
  template<class NumericType>
  inline
  void read_cea_mixture_data_ascii( CEAThermoMixture<NumericType>& thermo, std::istream &in )
  {
    skip_comment_lines(in, '#');

    std::string name;
    unsigned int n_int;
    std::vector<NumericType> coeffs;
    NumericType h_form, val;

    const ChemicalMixture<NumericType>& chem_mixture = thermo.chemical_mixture();

    while (in.good())
      {
	in >> name;   // Species Name
	in >> n_int;  // Number of T intervals: [200-1000], [1000-6000], ([6000-20000])
	in >> h_form; // Formation Enthalpy @ 298.15 K

	coeffs.clear();
	for (unsigned int interval=0; interval<n_int; interval++)
	  {
	    for (unsigned int n=0; n<10; n++)
	      {
		in >> val, coeffs.push_back(val);
	      }
	  }

	// If we are still good, we have a valid set of thermodynamic
	// data for this species. Otherwise, we read past end-of-file 
	// in the section above
	if (in.good())
	  {
	    // Check if this is a species we want.
	    if( chem_mixture.active_species_name_map().find(name) !=
		chem_mixture.active_species_name_map().end() )
	      {
		thermo.add_curve_fit(name, coeffs);
	      }
	  }
      } // end while
    
    // Make sure we actually populated everything
    if( !thermo.check() )
      {
	std::cerr << "Error: CEA table not fully populated" << std::endl;
	antioch_error();
      }

    return;
  }


  template<class NumericType>
  inline
  void read_cea_mixture_data_ascii_default( CEAThermoMixture<NumericType>& thermo )
  {
    static const std::string
      default_thermo_data
      ("#-----------------------------------------------------------------------------\n"
      "# Cp, H, and S curve fit parameters from CEA program (1994)\n"
      "# \n"
      "# File Format:\n"
      "# \n"
      "# First line:\n"
      "# \n"
      "# 	Species Name\n"
      "# 	Number of T intervals: [200-1000], [1000-6000], ([6000-20000])\n"
      "# 	Formation Enthalpy @ 298.15K\n"
      "# \n"
      "# Lines 2-4 (2-8):\n"
      "# \n"
      "# 	a0-a4; a5-a9 for each interval.\n"
      "# \n"
      "# \n"
      "# Cp/R =  a0*T^-2   + a1*T^-1     + a2     + a3*T   + a4*T^2   + a5*T^3   + a6*T^4\n"
      "# H/RT = -a0*T^-2   + a1*T^-1*lnT + a2     + a3*T/2 + a4*T^2/3 + a5*T^3/4 + a6*T^4/5 + a8/T\n"
      "# S/R  = -a0*T^-2/2 - a1*T^-1     + a2*lnT + a3*T   + a4*T^2/2 + a5*T^3/3 + a6*T^4/4 + a9\n"
      "# \n"
      "# Note that a7 is always zero and is not used in the calculation of Cp,H, or S.\n"
      "# The constant is included simply for compatibility with cea.\n"
      "# \n"
      "# \n"
      "# Sources:\n"
      "# \n"
      "# All values come directly from cea400 program (05/98)\n"
      "# Note that for monotomics in which the only non-zero parameter is a2, cea \n"
      "# stores this value in the 1st position. This must be changed for FIN-S to \n"
      "# work correctly.\n"
      "# [PB]: HO2/H2O2 values taken from  http://www.grc.nasa.gov/WWW/CEAWeb/ceaThermoBuild.htm \n"
      "#-------------------------------------------------------------------------------\n"
      "\n"
      "Ar  3		0.000                                                            \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00 -7.45375000e+02  4.37967491e+00\n"
      "  2.01053848e+01 -5.99266107e-02  2.50006940e+00 -3.99214116e-08  1.20527214e-11\n"
      " -1.81901558e-15  1.07857664e-19  0.00000000e+00 -7.44993961e+02  4.37918011e+00\n"
      " -9.95126508e+08  6.45888726e+05 -1.67589470e+02  2.31993336e-02 -1.72108091e-06\n"
      "  6.53193846e-11 -9.74014773e-16  0.00000000e+00 -5.07830034e+06  1.46529848e+03\n"
      "										 \n"
      "Ar+  3		1526778.407					 	         \n"
      " -5.73120917e+04  7.93079147e+02 -1.71712122e+00  1.04418402e-02 -1.18020750e-05\n"
      "  6.52813478e-09 -1.44755813e-12  0.00000000e+00  1.79057223e+05  2.94915095e+01\n"
      " -3.83596540e+05  8.16201970e+02  2.30134263e+00 -4.95298377e-06  1.20510848e-08\n"
      " -2.18505029e-12  1.26549390e-16  0.00000000e+00  1.77181146e+05  7.94750748e+00\n"
      "  1.01737284e+07 -6.68510410e+03  4.46095379e+00 -3.03416529e-04  2.62347956e-08\n"
      " -1.20511583e-12  2.30380688e-17  0.00000000e+00  2.35435354e+05 -1.04451930e+01\n"
      "										 \n"
      "Air  2		-125.530						         \n"
      "  1.00995016e+04 -1.96827561e+02  5.00915511e+00 -5.76101373e-03  1.06685993e-05\n"
      " -7.94029797e-09  2.18523191e-12  0.00000000e+00 -1.76796731e+02 -3.92150098e+00\n"
      "  2.41521443e+05 -1.25787460e+03  5.14455867e+00 -2.13854179e-04  7.06522784e-08\n"
      " -1.07148349e-11  6.57780015e-16  0.00000000e+00  6.46226319e+03 -8.14740866e+00\n"
      "										 \n"
      "CPAir  3		-125.530						 \n"
      "  0.              0.              3.5             0.              0.            \n"
      "  0.              0.              0.              0.              0.            \n"
      "  0.              0.              3.5             0.              0.            \n"
      "  0.              0.              0.              0.              0.            \n"
      "  0.              0.              3.5             0.              0.            \n"
      "  0.              0.              0.              0.              0.            \n"
      "										 \n"
      "C  3		716680.000							 \n"
      "  6.49503147e+02 -9.64901086e-01  2.50467548e+00 -1.28144803e-05  1.98013365e-08\n"
      " -1.60614403e-11  5.31448341e-15  0.00000000e+00  8.54576311e+04  4.74799922e+00\n"
      " -1.28913647e+05  1.71952857e+02  2.64604439e+00 -3.35306895e-04  1.74209274e-07\n"
      " -2.90281783e-11  1.64218239e-15  0.00000000e+00  8.41059785e+04  4.13012235e+00\n"
      "  4.43252801e+08 -2.88601841e+05  7.73710832e+01 -9.71528189e-03  6.64959533e-07\n"
      " -2.23007878e-11  2.89938870e-16  0.00000000e+00  2.35527344e+06 -6.40512241e+02\n"
      "										 \n"
      "C+  3		1809444.482							 \n"
      "  2.25853318e+03 -1.57454401e+00  2.50363759e+00 -5.20253954e-06  4.51647956e-09\n"
      " -2.18115104e-12  4.49430845e-16  0.00000000e+00  2.16895191e+05  4.34577527e+00\n"
      "  1.25511271e+04 -3.41187502e+01  2.54338322e+00 -2.80512097e-05  9.75164222e-09\n"
      " -1.73685542e-12  1.24619194e-16  0.00000000e+00  2.17100179e+05  4.06398843e+00\n"
      "  5.61815495e+05 -6.04705988e+03  5.88454166e+00 -7.21189472e-04  6.82348422e-08\n"
      " -2.59987863e-12  3.63386841e-17  0.00000000e+00  2.58137054e+05 -2.28001243e+01\n"
      "										 \n"
      "C2  3		830457.000							 \n"
      "  5.55956757e+05 -9.98000622e+03  6.68153991e+01 -1.74341173e-01  2.44849356e-04\n"
      " -1.70344706e-07  4.68447130e-11  0.00000000e+00  1.44586386e+05 -3.44818817e+02\n"
      " -9.68890957e+05  3.56099020e+03 -5.06351169e-01  2.94509281e-03 -7.13928908e-07\n"
      "  8.67047006e-11 -4.07681670e-15  0.00000000e+00  7.68186000e+04  3.33990355e+01\n"
      "  6.31616742e+06  1.36534413e+04 -3.99672321e+00  1.93752303e-03 -1.58441823e-07\n"
      "  5.52076555e-12 -7.25360969e-17  0.00000000e+00  9.39290526e+03  6.61413426e+01\n"
      "										 \n"
      "C2H  3		566205.472						         \n"
      "  7.65369105e+03 -4.18740132e+02  7.25427188e+00 -5.03701933e-03  8.23519623e-06\n"
      " -4.61676824e-09  9.50926320e-13  0.00000000e+00  6.85068967e+04 -1.58602839e+01\n"
      "  3.99586409e+06 -1.19690929e+04  1.71383304e+01 -3.33695957e-03  6.00328130e-07\n"
      " -5.49724012e-11  2.02273188e-15  0.00000000e+00  1.43142317e+05 -9.28591870e+01\n"
      "  0.              0.              7.59195493e+00  0.              0.            \n"
      "  0.              0.              0.              6.41223400e+04 -2.03895009e+01\n"
      "										 \n"
      "C2H2  2	228200.000							 \n"
      "  1.59813325e+05 -2.21667505e+03  1.26572561e+01 -7.98017014e-03  8.05580151e-06\n"
      " -2.43394461e-09 -7.50945461e-14  0.00000000e+00  3.71263379e+04 -5.24443365e+01\n"
      "  1.71379212e+06 -5.92897095e+03  1.23611564e+01  1.31470625e-04 -1.36286904e-07\n"
      "  2.71274606e-11 -1.30208685e-15  0.00000000e+00  6.26648973e+04 -5.81886602e+01\n"
      "										 \n"
      "C3  3		839948.500							 \n"
      " -4.35456285e+04  6.66010414e+02  1.45101513e+00  7.43442531e-03 -3.81010954e-06\n"
      " -2.33675605e-11  4.40699631e-13  0.00000000e+00  9.63517401e+04  2.02514928e+01\n"
      "  4.50804398e+06 -1.46101600e+04  2.28194701e+01 -8.54423692e-03  2.14604329e-06\n"
      " -2.10384214e-10  6.35151106e-15  0.00000000e+00  1.91196492e+05 -1.27185429e+02\n"
      "  5.62195627e+08 -4.80014820e+05  1.48773604e+02 -1.86380758e-02  1.25369345e-06\n"
      " -4.23152054e-11  5.69651073e-16  0.00000000e+00  3.77769020e+06 -1.22634937e+03\n"
      "										 \n"
      "CF  2		242300.000							 \n"
      " -4.85659087e+04  8.37066845e+02 -1.58605380e+00  1.37329926e-02 -1.54543720e-05\n"
      "  8.89879494e-09 -2.08319768e-12  0.00000000e+00  2.41920950e+04  3.37110765e+01\n"
      " -7.30301173e+04 -3.10432552e+02  4.68755965e+00 -1.75388618e-05  1.17998295e-08\n"
      " -1.88283316e-12  1.89811962e-16  0.00000000e+00  2.93490294e+04 -2.32175022e+00\n"
      "										 \n"
      "CF2  2		 -186600.00						         \n"
      " -4.28469549e+04  9.42241347e+02 -3.83847378e+00  2.84787584e-02 -3.61855678e-05\n"
      "  2.30750321e-08 -5.91498489e-12  0.00000000e+00 -2.77993759e+04  4.66760006e+01\n"
      " -1.98592186e+06  4.47486108e+03  2.05367791e+00  2.54212017e-03 -6.46636536e-07\n"
      "  7.98822582e-11 -3.82452258e-15  0.00000000e+00 -5.43485110e+04  2.28899734e+01\n"
      "										 \n"
      "CF3  2		-467400.000						         \n"
      " -2.89509314e+04  7.08259104e+02 -3.49874496e+00  3.60052779e-02 -4.51845638e-05\n"
      "  2.83046097e-08 -7.12428016e-12  0.00000000e+00 -6.05579954e+04  4.50043769e+01\n"
      " -3.00992537e+05 -1.04577415e+03  1.07783067e+01 -3.11222805e-04  6.88233084e-08\n"
      " -7.90018181e-12  3.66510631e-16  0.00000000e+00 -5.42636700e+04 -3.41619073e+01\n"
      "										 \n"
      "CF4  2		-933500.000						         \n"
      "  9.81534964e+03  1.16366074e+02 -1.28852758e+00  3.95962388e-02 -4.99633605e-05\n"
      "  3.12541430e-08 -7.84194211e-12  0.00000000e+00 -1.13896083e+05  2.93876064e+01\n"
      " -4.16418379e+05 -1.41489926e+03  1.40513863e+01 -4.19996407e-04  9.28167764e-08\n"
      " -1.06499640e-11  4.93955439e-16  0.00000000e+00 -1.09514188e+05 -5.48720115e+01\n"
      "										 \n"
      "CH  3		597338.000							 \n"
      "  2.22056413e+04 -3.40537175e+02  5.53138641e+00 -5.79489639e-03  7.96946173e-06\n"
      " -4.46585989e-09  9.59622884e-13  0.00000000e+00  7.24039052e+04 -9.10756700e+00\n"
      "  2.06073377e+06 -5.39612964e+03  7.85618794e+00 -7.96575798e-04  1.76427377e-07\n"
      " -1.97634586e-11  5.03028668e-16  0.00000000e+00  1.06219245e+05 -3.15471110e+01\n"
      " -8.06826755e+08  4.57539322e+05 -9.84385128e+01  1.23522859e-02 -8.48550223e-07\n"
      "  3.04037261e-11 -4.40026028e-16  0.00000000e+00 -3.59580957e+06  8.95336505e+02\n"
      "										 \n"
      "CH2  2		388800.000						         \n"
      "  3.52591418e+04 -3.36765436e+02  4.50893691e+00  2.52106603e-03 -5.31151087e-06\n"
      "  6.65421308e-09 -2.61007296e-12  0.00000000e+00  4.73772550e+04 -3.80734012e+00\n"
      "  2.51041594e+06 -7.95393239e+03  1.23882957e+01 -1.77590968e-03  3.21740029e-07\n"
      " -3.06886344e-11  1.20490060e-15  0.00000000e+00  9.60102856e+04 -6.15597799e+01\n"
      "										 \n"
      "CH3  2		146900.000						         \n"
      " -2.87728611e+04  5.09505607e+02  1.99073056e-01  1.36396516e-02 -1.43455892e-05\n"
      "  1.01396738e-08 -3.02843889e-12  0.00000000e+00  1.41109873e+04  2.02339462e+01\n"
      "  2.76090427e+06 -9.33655279e+03  1.48769863e+01 -1.43922769e-03  2.44392393e-07\n"
      " -2.22384579e-11  8.39158849e-16  0.00000000e+00  7.48476479e+04 -7.91948334e+01\n"
      "										 \n"
      "CH4  2		-74600.000						         \n"
      " -1.76654573e+05  2.78547782e+03 -1.20193547e+01  3.91462588e-02 -3.61165608e-05\n"
      "  2.01838794e-08 -4.95577215e-12  0.00000000e+00 -2.33101156e+04  8.90108100e+01\n"
      "  3.74626570e+06 -1.38885134e+04  2.05402982e+01 -1.94419693e-03  4.32387145e-07\n"
      " -4.06101278e-11  1.64315927e-15  0.00000000e+00  7.56598868e+04 -1.22297711e+02\n"
      "										 \n"
      "Cl  3		 121301.000							 \n"
      "  2.27621585e+04 -2.16841329e+02  2.74518512e+00  2.45110169e-03 -5.45801199e-06\n"
      "  4.41798688e-09 -1.28813400e-12  0.00000000e+00  1.50135707e+04  3.10295076e+00\n"
      " -1.69793293e+05  6.08172646e+02  2.12866409e+00  1.30736703e-04 -2.64488360e-08\n"
      "  2.84250478e-12 -1.25291173e-16  0.00000000e+00  9.93438740e+03  8.84475941e+00\n"
      " -7.13968707e+07  4.49993633e+04 -9.26431535e+00  1.65743796e-03 -1.32621940e-07\n"
      "  5.53399887e-12 -8.39030188e-17  0.00000000e+00 -3.40533303e+05  1.06911130e+02\n"
      "										 \n"
      "Cl2  2		 0.000							         \n"
      "  3.46281724e+04 -5.54712949e+02  6.20759103e+00 -2.98963673e-03  3.17303416e-06\n"
      " -1.79363467e-09  4.26005863e-13  0.00000000e+00  1.53407075e+03 -9.43835303e+00\n"
      "  6.09256675e+06 -1.94962688e+04  2.85453491e+01 -1.44996828e-02  4.46388943e-06\n"
      " -6.35852403e-10  3.32735931e-14  0.00000000e+00  1.21211722e+05 -1.69077832e+02\n"
      "										 \n"
      "CN  3		438683.448							 \n"
      "  3.94910175e+03 -1.39157387e+02  4.93077604e+00 -6.30459473e-03  1.25682136e-05\n"
      " -9.87818175e-09  2.84310304e-12  0.00000000e+00  5.22845470e+04 -2.76308000e+00\n"
      " -2.22797608e+06  5.04066445e+03 -2.12179448e-01  1.35488122e-03  1.32592277e-07\n"
      " -6.93693400e-11  5.49489162e-15  0.00000000e+00  1.78454233e+04  3.28251898e+01\n"
      " -1.79477307e+08  1.05433143e+05 -1.72959888e+01  2.19486383e-03 -8.50880168e-08\n"
      "  9.31846830e-13  6.35821408e-18  0.00000000e+00 -7.96247645e+05  1.91311273e+02\n"
      "										 \n"
      "CN+  3		1800490.876						         \n"
      " -8.30280101e+05  8.77557203e+03 -2.97740322e+01  4.97682660e-02 -1.30219681e-05\n"
      " -2.05830943e-08  1.12683268e-11  0.00000000e+00  1.70579088e+05  2.03989171e+02\n"
      " -7.15337451e+06  1.85722746e+04 -1.08452053e+01  6.10660526e-03 -1.19119357e-06\n"
      "  1.18483369e-10 -4.79977687e-15  0.00000000e+00  9.26204124e+04  1.13532651e+02\n"
      " -1.81294631e+08  1.12880452e+05 -2.28730839e+01  3.48458573e-03 -2.20976256e-07\n"
      "  6.57976539e-12 -7.55649586e-17  0.00000000e+00 -6.78420857e+05  2.36263389e+02\n"
      "										 \n"
      "CO  3		-110530.000							 \n"
      "  1.48902756e+04 -2.92225095e+02  5.72445841e+00 -8.17613694e-03  1.45688598e-05\n"
      " -1.08773325e-08  3.02790549e-12  0.00000000e+00 -1.30306970e+04 -7.85914715e+00\n"
      "  4.61915856e+05 -1.94468575e+03  5.91664709e+00 -5.66423407e-04  1.39880257e-07\n"
      " -1.78766498e-11  9.62085040e-16  0.00000000e+00 -2.46573844e+03 -1.38739939e+01\n"
      "  8.86855241e+08 -7.50028541e+05  2.49544449e+02 -3.95630299e-02  3.29773210e-06\n"
      " -1.31839398e-10  1.99891378e-15  0.00000000e+00  5.70135124e+06 -2.06067958e+03\n"
      "										 \n"
      "CO+  3		1247790.876						         \n"
      " -2.17873999e+04  1.28881819e+02  3.76902289e+00 -3.43171391e-03  8.19387911e-06\n"
      " -6.46375790e-09  1.80371143e-12  0.00000000e+00  1.48234825e+05  3.99043782e+00\n"
      "  2.31681660e+05 -1.05763263e+03  4.55420231e+00  4.49546934e-04 -2.48947782e-07\n"
      "  5.26750391e-11 -3.28947110e-15  0.00000000e+00  1.55505203e+05 -3.87341085e+00\n"
      " -3.03555723e+08  2.39308392e+05 -7.03490196e+01  1.13953628e-02 -8.31506401e-07\n"
      "  2.86366814e-11 -3.80321984e-16  0.00000000e+00 -1.68859083e+06  6.29189365e+02\n"
      "										 \n"
      "CO2  3		-393510.000						         \n"
      "  4.94378364e+04 -6.26429208e+02  5.30181336e+00  2.50360057e-03 -2.12470010e-07\n"
      " -7.69148680e-10  2.84997991e-13  0.00000000e+00 -4.52818986e+04 -7.04876965e+00\n"
      "  1.17696943e+05 -1.78880147e+03  8.29154353e+00 -9.22477831e-05  4.86963541e-09\n"
      " -1.89206384e-12  6.33067509e-16  0.00000000e+00 -3.90834501e+04 -2.65268192e+01\n"
      " -1.54440623e+09  1.01683614e+06 -2.56137710e+02  3.36936393e-02 -2.18115791e-06\n"
      "  6.99132485e-11 -8.84221214e-16  0.00000000e+00 -8.04312850e+06  2.25415326e+03\n"
      "										 \n"
      "F  3		79380.000							 \n"
      "  1.13740909e+03 -1.45339280e+02  4.07740361e+00 -4.30336014e-03  5.72889774e-06\n"
      " -3.81931290e-09  1.01832251e-12  0.00000000e+00  9.31111012e+03 -3.55898265e+00\n"
      "  1.47350623e+04  8.14992736e+01  2.44437182e+00  2.12021003e-05 -4.54691862e-09\n"
      "  5.10952873e-13 -2.33389465e-17  0.00000000e+00  8.38837465e+03  5.47871064e+00\n"
      " -2.92672476e+07  1.77567492e+04 -1.81446326e+00  5.35120028e-04 -3.51362080e-08\n"
      "  1.10151969e-12 -1.06129384e-17  0.00000000e+00 -1.31964272e+05  4.24881456e+01\n"
      "										 \n"
      "F2  2		0.000								 \n"
      "  1.01817754e+04  2.27422347e+01  1.97135403e+00  8.15160123e-03 -1.14895968e-05\n"
      "  7.95864947e-09 -2.16707863e-12  0.00000000e+00 -9.58693453e+02  1.13059974e+01\n"
      " -2.94116687e+06  9.45659515e+03 -7.73861346e+00  7.64471159e-03 -2.24100723e-06\n"
      "  2.91584473e-10 -1.42503371e-14  0.00000000e+00 -6.07100397e+04  8.42383313e+01\n"
      "										 \n"
      "H  3		217998.828							 \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  2.54737080e+04 -4.46682853e-01\n"
      "  6.07877425e+01 -1.81935442e-01  2.50021182e+00 -1.22651286e-07  3.73287633e-11\n"
      " -5.68774456e-15  3.41021020e-19  0.00000000e+00  2.54748640e+04 -4.48191777e-01\n"
      "  2.17375769e+08 -1.31203540e+05  3.39917420e+01 -3.81399968e-03  2.43285484e-07\n"
      " -7.69427554e-12  9.64410563e-17  0.00000000e+00  1.06763809e+06 -2.74230105e+02\n"
      "										 \n"
      "H+  3		1536245.928							 \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  1.84021488e+05 -1.14064664e+00\n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  1.84021488e+05 -1.14064664e+00\n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  1.84021488e+05 -1.14064664e+00\n"
      "										 \n"
      "H2  3		0.000								 \n"
      "  4.07832281e+04 -8.00918545e+02  8.21470167e+00 -1.26971436e-02  1.75360493e-05\n"
      " -1.20286016e-08  3.36809316e-12  0.00000000e+00  2.68248438e+03 -3.04378866e+01\n"
      "  5.60812338e+05 -8.37149134e+02  2.97536304e+00  1.25224993e-03 -3.74071842e-07\n"
      "  5.93662825e-11 -3.60699573e-15  0.00000000e+00  5.33981585e+03 -2.20276405e+00\n"
      "  4.96671613e+08 -3.14744812e+05  7.98388750e+01 -8.41450419e-03  4.75306044e-07\n"
      " -1.37180973e-11  1.60537460e-16  0.00000000e+00  2.48835466e+06 -6.69552419e+02\n"
      "										 \n"
      "H2+  3		1494672.430						         \n"
      " -3.12089221e+04  2.30462995e+02  3.33556119e+00 -2.41904915e-03  7.00601261e-06\n"
      " -5.61000425e-09  1.56416804e-12  0.00000000e+00  1.77410460e+05 -8.27833845e-01\n"
      "  1.67222618e+06 -6.59518544e+03  1.27932197e+01 -5.50934546e-03  2.03066946e-06\n"
      " -3.35102754e-10  1.94608913e-14  0.00000000e+00  2.18999958e+05 -6.79271109e+01\n"
      " -1.82206202e+08  1.01819129e+05 -1.24582075e+01  1.07663464e-03 -3.93220966e-08\n"
      "  6.28514344e-13 -2.09437896e-18  0.00000000e+00 -6.51306155e+05  1.47140561e+02\n"
      "										 \n"
      "H2O  2		-241826.000						         \n"
      " -3.94795999e+04  5.75572977e+02  9.31783351e-01  7.22271091e-03 -7.34255448e-06\n"
      "  4.95504134e-09 -1.33693261e-12  0.00000000e+00 -3.30397425e+04  1.72420539e+01\n"
      "  1.03497224e+06 -2.41269895e+03  4.64611114e+00  2.29199814e-03 -6.83683007e-07\n"
      "  9.42646842e-11 -4.82238028e-15  0.00000000e+00 -1.38428625e+04 -7.97815119e+00\n"
      "										 \n"
      "H2O2 2           -135880.000                                                     \n"
      " -9.279533580E+04  1.564748385E+03 -5.976460140E+00  3.270744520E-02 -3.932193260E-05 \n"
      "  2.509255235E-08 -6.465045290E-12  0.000000000E+00 -2.494004728E+04  5.877174180E+01 \n"
      "  1.489428027E+06 -5.170821780E+03  1.128204970E+01 -8.042397790E-05 -1.818383769E-08 \n"
      "  6.947265590E-12 -4.827831900E-16  0.000000000E+00  1.418251038E+04 -4.650855660E+01 \n"
      "										 \n"
      "HCl  2		-92310.000						         \n"
      "  2.06258829e+04 -3.09336886e+02  5.27541885e+00 -4.82887422e-03  6.19579460e-06\n"
      " -3.04002378e-09  4.91679003e-13  0.00000000e+00 -1.06778230e+04 -7.30931775e+00\n"
      "  9.15774951e+05 -2.77055021e+03  5.97353979e+00 -3.62981006e-04  4.73552919e-08\n"
      "  2.81026205e-12 -6.65610422e-16  0.00000000e+00  5.67495805e+03 -1.64282706e+01\n"
      "										 \n"
      "HCN  2		135000.000						         \n"
      "  9.09829783e+04 -1.23865966e+03  8.72132229e+00 -6.52828128e-03  8.87274723e-06\n"
      " -4.80890877e-09  9.31791773e-13  0.00000000e+00  2.12197899e+04 -2.74668335e+01\n"
      "  1.23689825e+06 -4.44676184e+03  9.73891163e+00 -5.85539810e-04  1.07285501e-07\n"
      " -1.01340360e-11  3.34873666e-16  0.00000000e+00  4.24459481e+04 -4.00579742e+01\n"
      "										 \n"
      "He  3		0.000								 \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00 -7.45375000e+02  9.28723974e-01\n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00 -7.45375000e+02  9.28723974e-01\n"
      "  3.39684542e+06 -2.19403765e+03  3.08023188e+00 -8.06895755e-05  6.25278491e-09\n"
      " -2.57499007e-13  4.42996022e-18  0.00000000e+00  1.65051896e+04 -4.04881439e+00\n"
      "										 \n"
      "He+  3		2378521.473						         \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  2.85323374e+05  1.62166556e+00\n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  2.85323374e+05  1.62166556e+00\n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  2.85323374e+05  1.62166556e+00\n"
      "										 \n"
      "HO2   2           12020.000                                                        \n"
      " -7.598882540E+04  1.329383918E+03 -4.677388240E+00  2.508308202E-02 -3.006551588E-05 \n"
      "  1.895600056E-08 -4.828567390E-12  0.000000000E+00 -5.873350960E+03  5.193602140E+01 \n"
      " -1.810669724E+06  4.963192030E+03 -1.039498992E+00  4.560148530E-03 -1.061859447E-06 \n"
      "  1.144567878E-10 -4.763064160E-15  0.000000000E+00 -3.200817190E+04  4.066850920E+01 \n"
      "										 \n"
      "N  3		472680.000							 \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  5.61046378e+04  4.19390932e+00\n"
      "  8.87650138e+04 -1.07123150e+02  2.36218829e+00  2.91672008e-04 -1.72951510e-07\n"
      "  4.01265788e-11 -2.67722757e-15  0.00000000e+00  5.69735133e+04  4.86523579e+00\n"
      "  5.47518105e+08 -3.10757498e+05  6.91678274e+01 -6.84798813e-03  3.82757240e-07\n"
      " -1.09836771e-11  1.27798602e-16  0.00000000e+00  2.55058562e+06 -5.84876971e+02\n"
      "										 \n"
      "N+  3		1882127.624							 \n"
      "  5.23707921e+03  2.29995832e+00  2.48748882e+00  2.73749075e-05 -3.13444758e-08\n"
      "  1.85011133e-11 -4.44735098e-15  0.00000000e+00  2.25628474e+05  5.07683507e+00\n"
      "  2.90497037e+05 -8.55790861e+02  3.47738929e+00 -5.28826719e-04  1.35235031e-07\n"
      " -1.38983412e-11  5.04616628e-16  0.00000000e+00  2.31080998e+05 -1.99414226e+00\n"
      "  1.64609215e+07 -1.11316522e+04  4.97698664e+00 -2.00539358e-04  1.02248136e-08\n"
      " -2.69143086e-13  3.53993159e-18  0.00000000e+00  3.13628470e+05 -1.70664595e+01\n"
      "										 \n"
      "N2  3		0.000								 \n"
      "  2.21037122e+04 -3.81846145e+02  6.08273815e+00 -8.53091381e-03  1.38464610e-05\n"
      " -9.62579293e-09  2.51970560e-12  0.00000000e+00  7.10845911e+02 -1.07600320e+01\n"
      "  5.87709908e+05 -2.23924255e+03  6.06694267e+00 -6.13965296e-04  1.49179819e-07\n"
      " -1.92309442e-11  1.06194871e-15  0.00000000e+00  1.28320618e+04 -1.58663484e+01\n"
      "  8.30971200e+08 -6.42048187e+05  2.02020507e+02 -3.06501961e-02  2.48685558e-06\n"
      " -9.70579208e-11  1.43751673e-15  0.00000000e+00  4.93850663e+06 -1.67204791e+03\n"
      "										 \n"
      "CPN2  3	0.000            						 \n"
      "  0.              0.              3.5             0.              0.            \n"
      "  0.              0.              0.              0.              0.            \n"
      "  0.              0.              3.5             0.              0.            \n"
      "  0.              0.              0.              0.              0.            \n"
      "  0.              0.              3.5             0.              0.            \n"
      "  0.              0.              0.              0.              0.            \n"
      "										 \n"
      "N2+  3		1509508.424						         \n"
      " -3.47404104e+04  2.69621543e+02  3.16491970e+00 -2.13224760e-03  6.73048636e-06\n"
      " -5.63731151e-09  1.62175773e-12  0.00000000e+00  1.79000446e+05  6.83295935e+00\n"
      " -2.84559747e+06  7.05888915e+03 -2.88488265e+00  3.06867527e-03 -4.36164780e-07\n"
      "  2.10250882e-11  5.41202519e-16  0.00000000e+00  1.34038874e+05  5.09089472e+01\n"
      " -3.71283095e+08  3.13928800e+05 -9.60351986e+01  1.57119350e-02 -1.17506567e-06\n"
      "  4.14444172e-11 -5.62189377e-16  0.00000000e+00 -2.21736248e+06  8.43627257e+02\n"
      "										 \n"
      "NCO  2		131843.928						         \n"
      "  9.50796917e+03 -1.95732986e+02  4.23512791e+00  3.90371915e-03  3.85514012e-07\n"
      " -2.85351957e-09  1.23451381e-12  0.00000000e+00  1.55696257e+04  2.04252484e+00\n"
      "  7.39292978e+04 -1.61725838e+03  8.57491953e+00 -3.76455498e-04  7.03829586e-08\n"
      " -6.68653218e-12  2.98893417e-16  0.00000000e+00  2.28940706e+04 -2.55276588e+01\n"
      "										 \n"
      "NH  3		357032.001							 \n"
      "  1.35965062e+04 -1.90029565e+02  4.51849625e+00 -2.43277540e-03  2.37758524e-06\n"
      " -2.59278055e-10 -2.65968566e-13  0.00000000e+00  4.28097215e+04 -3.88655464e+00\n"
      "  1.95814005e+06 -5.78285659e+03  9.33573752e+00 -2.29290818e-03  6.07608715e-07\n"
      " -6.64793603e-11  2.38423143e-15  0.00000000e+00  7.89890925e+04 -4.11696671e+01\n"
      "  9.52452031e+07 -8.58576221e+04  2.98043076e+01 -2.97954712e-03  1.65632380e-07\n"
      " -4.74475838e-12  5.57010457e-17  0.00000000e+00  6.96138236e+05 -2.22901475e+02\n"
      "										 \n"
      "NH+  3		1665795.114						         \n"
      "  4.25377807e+03 -2.45823599e+02  6.70892578e+00 -1.03849090e-02  1.50901050e-05\n"
      " -9.58052447e-09  2.33321001e-12  0.00000000e+00  2.00108653e+05 -1.39506084e+01\n"
      "  1.40570994e+06 -4.13621679e+03  7.63201549e+00 -1.22832624e-03  2.72118887e-07\n"
      " -2.01009966e-11  3.71725697e-17  0.00000000e+00  2.25898469e+05 -2.78678557e+01\n"
      "  2.39294879e+08 -1.74166248e+05  5.10791647e+01 -5.56763349e-03  3.27339769e-07\n"
      " -9.79799764e-12  1.18654583e-16  0.00000000e+00  1.55470927e+06 -4.06597642e+02\n"
      "										 \n"
      "NH2  2		189134.662						         \n"
      " -3.11601645e+04  4.75073546e+02  1.37457969e+00  6.29944545e-03 -5.97605729e-06\n"
      "  4.48239827e-09 -1.41043554e-12  0.00000000e+00  1.92910370e+04  1.53894335e+01\n"
      "  1.66900789e+06 -5.61038294e+03  9.97188164e+00 -1.17828134e-03  4.23396549e-07\n"
      " -6.53976978e-11  3.61987805e-15  0.00000000e+00  5.68905973e+04 -4.37935788e+01\n"
      "										 \n"
      "NH3  2		-45940.000						         \n"
      " -7.68122642e+04  1.27095161e+03 -3.89322933e+00  2.14598848e-02 -2.18376679e-05\n"
      "  1.31738577e-08 -3.33232227e-12  0.00000000e+00 -1.26488643e+04  4.36601505e+01\n"
      "  2.45238816e+06 -8.04089084e+03  1.27134588e+01 -3.98017165e-04  3.55246584e-08\n"
      "  2.53096956e-12 -3.32272323e-16  0.00000000e+00  4.38618973e+04 -6.46232791e+01\n"
      "										 \n"
      "NO  3		91269.110							 \n"
      " -1.14391658e+04  1.53646774e+02  3.43146865e+00 -2.66859213e-03  8.48139877e-06\n"
      " -7.68511079e-09  2.38679758e-12  0.00000000e+00  9.09794974e+03  6.72872795e+00\n"
      "  2.23903708e+05 -1.28965624e+03  5.43394039e+00 -3.65605546e-04  9.88101763e-08\n"
      " -1.41608327e-11  9.38021642e-16  0.00000000e+00  1.75029422e+04 -8.50169908e+00\n"
      " -9.57530764e+08  5.91243671e+05 -1.38456733e+02  1.69433998e-02 -1.00735146e-06\n"
      "  2.91258526e-11 -3.29511091e-16  0.00000000e+00 -4.67750329e+06  1.24208166e+03\n"
      "										 \n"
      "NO+  3		990807.104						         \n"
      "  1.39852659e+03 -1.59049494e+02  5.12291732e+00 -6.39444005e-03  1.12392489e-05\n"
      " -7.98862419e-09  2.10739504e-12  0.00000000e+00  1.18749224e+05 -4.39855768e+00\n"
      "  6.06984843e+05 -2.27838831e+03  6.08031794e+00 -6.06681584e-04  1.43199471e-07\n"
      " -1.74798063e-11  8.93496500e-16  0.00000000e+00  1.32270602e+05 -1.51987490e+01\n"
      "  2.67640091e+09 -1.83294905e+06  5.09925021e+02 -7.11382025e-02  5.31766050e-06\n"
      " -1.96320842e-10  2.80526850e-15  0.00000000e+00  1.44330920e+07 -4.32404518e+03\n"
      "										 \n"
      "NO2  2		34193.022						         \n"
      " -5.64204584e+04  9.63309734e+02 -2.43451851e+00  1.92776414e-02 -1.87456362e-05\n"
      "  9.14553637e-09 -1.77766146e-12  0.00000000e+00 -1.54793043e+03  4.06785541e+01\n"
      "  7.21271176e+05 -3.83253763e+03  1.11395534e+01 -2.23801440e-03  6.54762164e-07\n"
      " -7.61120803e-11  3.32829926e-15  0.00000000e+00  2.50244718e+04 -4.30507224e+01\n"
      "										 \n"
      "Ne  3		0.000								 \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00 -7.45375000e+02  3.35532272e+00\n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00 -7.45375000e+02  3.35532272e+00\n"
      " -1.23825275e+07  6.95857958e+03  1.01670929e+00  1.42466456e-04 -4.80393393e-09\n"
      " -1.17021318e-13  8.41515365e-18  0.00000000e+00 -5.66393363e+04  1.64843870e+01\n"
      "										 \n"
      "O  3		249175.003							 \n"
      " -7.95361130e+03  1.60717779e+02  1.96622644e+00  1.01367031e-03 -1.11041542e-06\n"
      "  6.51750750e-10 -1.58477925e-13  0.00000000e+00  2.84036244e+04  8.40424182e+00\n"
      "  2.61902026e+05 -7.29872203e+02  3.31717727e+00 -4.28133436e-04  1.03610459e-07\n"
      " -9.43830433e-12  2.72503830e-16  0.00000000e+00  3.39242806e+04 -6.67958535e-01\n"
      "  1.77900426e+08 -1.08232826e+05  2.81077837e+01 -2.97523226e-03  1.85499753e-07\n"
      " -5.79623154e-12  7.19172016e-17  0.00000000e+00  8.89094263e+05 -2.18172815e+02\n"
      "										 \n"
      "O+  3		1568787.228							 \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00  1.87935284e+05  4.39337676e+00\n"
      " -2.16651321e+05  6.66545615e+02  1.70206436e+00  4.71499281e-04 -1.42713182e-07\n"
      "  2.01659590e-11 -9.10715776e-16  0.00000000e+00  1.83719197e+05  1.00569038e+01\n"
      " -2.14383538e+08  1.46951852e+05 -3.68086454e+01  5.03616454e-03 -3.08787385e-07\n"
      "  9.18683487e-12 -1.07416327e-16  0.00000000e+00 -9.61420896e+05  3.42619308e+02\n"
      "										 \n"
      "O2  3		0.000								 \n"
      " -3.42556269e+04  4.84699986e+02  1.11901159e+00  4.29388743e-03 -6.83627313e-07\n"
      " -2.02337478e-09  1.03904064e-12  0.00000000e+00 -3.39145434e+03  1.84969912e+01\n"
      " -1.03793994e+06  2.34483275e+03  1.81972949e+00  1.26784887e-03 -2.18807142e-07\n"
      "  2.05372411e-11 -8.19349062e-16  0.00000000e+00 -1.68901253e+04  1.73871835e+01\n"
      "  4.97515261e+08 -2.86602339e+05  6.69015464e+01 -6.16971869e-03  3.01623757e-07\n"
      " -7.42087888e-12  7.27744063e-17  0.00000000e+00  2.29348755e+06 -5.53044968e+02\n"
      "										 \n"
      "O2+  3		1171828.436						         \n"
      " -8.17746071e+04  1.00476232e+03 -3.36540054e-01  6.10537849e-03 -2.70414396e-06\n"
      " -3.01196202e-10  4.16272255e-13  0.00000000e+00  1.34792023e+05  2.78321591e+01\n"
      "  7.36613050e+04 -8.45858362e+02  4.98541998e+00 -1.61328191e-04  6.43445548e-08\n"
      " -1.50600823e-11  1.57904421e-15  0.00000000e+00  1.44631062e+05 -5.81278407e+00\n"
      " -1.56225883e+09  1.16148087e+06 -3.30267037e+02  4.71112832e-02 -3.35458085e-06\n"
      "  1.16800722e-10 -1.58980527e-15  0.00000000e+00 -8.85846055e+06  2.85218069e+03\n"
      "										 \n"
      "OH  3		39347.106							 \n"
      " -1.99886366e+03  9.30014295e+01  3.05085385e+00  1.52953035e-03 -3.15789256e-06\n"
      "  3.31544734e-09 -1.13876303e-12  0.00000000e+00  3.24004397e+03  4.67411290e+00\n"
      "  1.01739349e+06 -2.50995748e+03  5.11654809e+00  1.30529875e-04 -8.28431902e-08\n"
      "  2.00647550e-11 -1.55699342e-15  0.00000000e+00  2.04452334e+04 -1.10128250e+01\n"
      "  2.84724229e+08 -1.85953704e+05  5.00825057e+01 -5.14238586e-03  2.87554326e-07\n"
      " -8.22883906e-12  9.56725603e-17  0.00000000e+00  1.46864630e+06 -4.02356407e+02\n"
      "										 \n"
      "Si  3		450000.000							 \n"
      "  9.83614081e+01  1.54654452e+02  1.87643667e+00  1.32063800e-03 -1.52972006e-06\n"
      "  8.95056277e-10 -1.95287349e-13  0.00000000e+00  5.26351031e+04  9.69828888e+00\n"
      " -6.16929885e+05  2.24068393e+03 -4.44861932e-01  1.71005632e-03 -4.10771416e-07\n"
      "  4.55888478e-11 -1.88951535e-15  0.00000000e+00  3.95355876e+04  2.67966806e+01\n"
      " -9.28654894e+08  5.44398989e+05 -1.20673974e+02  1.35966270e-02 -7.60649866e-07\n"
      "  2.14974607e-11 -2.47411677e-16  0.00000000e+00 -4.29379212e+06  1.08638284e+03\n"
      "										 \n"
      "SiO  2		-98842.860						         \n"
      " -4.60579335e+04  7.88008654e+02 -1.52373799e+00  1.41931317e-02 -1.66800664e-05\n"
      "  9.99331149e-09 -2.42820390e-12  0.00000000e+00 -1.65800032e+04  3.29397543e+01\n"
      " -2.29083217e+05  6.00671324e+01  4.47768446e+00 -8.55465297e-05  1.02072020e-07\n"
      " -3.23332906e-11  3.52510129e-15  0.00000000e+00 -1.41706507e+04 -7.14499047e-01\n"
      "										 \n"
      "e  3		0.000								 \n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00 -7.45375000e+02 -1.17208127e+01\n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00 -7.45375000e+02 -1.17208127e+01\n"
      "  0.00000000e+00  0.00000000e+00  2.50000000e+00  0.00000000e+00  0.00000000e+00\n"
      "  0.00000000e+00  0.00000000e+00  0.00000000e+00 -7.45375000e+02 -1.17208127e+01\n"
      );

    std::istringstream buf(default_thermo_data);

    read_cea_mixture_data_ascii(thermo,buf);

    return;
  }


} // end namespace Antioch

#endif // ANTIOCH_CEA_MIXTURE_ASCII_PARSING_H