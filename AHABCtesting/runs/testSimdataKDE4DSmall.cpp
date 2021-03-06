/* MCT - Markov Chains on Trees.


   Copyright (C) 2012 Jennifer Harlow

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/*! \file
\brief Basic testing of AHABC using Simdata sampler, 
2 parameters, 2 summary stats, small prior range,
and using function approximation to kde rather
than average of mcmc histograms.


The MultiLociSimDataSampler uses libsequence
to simulate snp data for given parameters.  

*/

#include "testing_toolsAHABC_KDE.hpp"

#include "automcmc_factory.hpp"

#include "config.h" // mct

#include "utilities.hpp"

#include <mscplusplus/population_structure.hpp>

#include <boost/shared_ptr.hpp>

#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

#define REDUCE_PRIOR_LIMS // to reduce the prior limits to the observed data

using namespace std;
	

int main()
{
	int myPid = getpid();
	std::cout << "ahabc with kde function approximation for simdata 4d with small prior range" << std::endl;
	std::cout << "This process id is " << myPid << std::endl;
	
	std::string baseOutputDir("../figures/");
	
	std::string thisDir("SimDataKDE4DSmallNewParams");
	#ifdef REDUCE_PRIOR_LIMS
		thisDir += "Rdcd";
	#endif
	
	std::string descriptionString("simdata4DKDE");
	
	// make the output dir and get back path
	std::string path = mct_utilities::makeDir(baseOutputDir, thisDir);
	
	string prefix = path + "/" + descriptionString;
	
	cxsc::real maxMeasure(0.0001);// to use for approx to kde
		
	size_t n_reps = 2000; // number of joint observations to generate
	
	size_t n_obs = 50; // number of 'observed values' to have
	
	size_t nsam = 10; // number of individuals in a sample
			
	// loci in each multi-loci SimData
	size_t nloci = 1; 
	
	size_t nsites =25000; // number of sites in each locus 
	
	size_t Nzero = 1000000; // effective popn size (per sub-pop)
	double unit = 100000000.0; // 10^8
	
	int seed = 2345;
	
	//true mutation rate
	//mutation rates PER SITE ie 4N0(mu/site)
	double mu = 5.0; //before scaling by dividing by unit
	double theta_per_site = mu*4*Nzero/unit;
	
	//true growth rates
	double growth = 50.0;
	
	std::vector < double > trueParams (1, theta_per_site);
	trueParams.push_back(growth);
		
	/* Create an empty population structure */
	boost::shared_ptr< hudson_ms::PopulationStructure > pop
	(new hudson_ms::PopulationStructure());
	/* Add a  simple one-subpop population structure for nsam samples, growth */
	pop->add(nsam, growth);
	
	cout << "\n\nn_reps = " << n_reps << ", nsites = " << nsites << ", nsam = " << nsam << ", n_obs = " << n_obs << endl;
	
	cout << "true theta per site " << theta_per_site << ", true growth " << growth << endl;
	
	string spec;
	{
		std::ostringstream stm;
		stm << "_s_" << nsam << "_ns_" << nsites << "_nr_" << n_reps;
		stm.precision(mct_utilities::fitPrecision(theta_per_site));
		stm <<   fixed;
		stm << "_th_" << theta_per_site;
		stm.precision(mct_utilities::fitPrecision(growth));
		stm << "_g_" << growth;
		stm.precision(mct_utilities::fitPrecision(_double(maxMeasure)));
		stm <<   fixed;
		stm << "_tol_" << _double(maxMeasure);
		
				
		spec = stm.str();
	}
	
	#ifdef REDUCE_PRIOR_LIMS
		std::vector< std::pair<double, double> > priorLims;
		// prior limits for theta_per_site
		priorLims.push_back( pair<double, double>
				//I think I should use 0.50*4 etc as min here
				(0.50*4*Nzero/unit, 50.0*4*Nzero/unit));
		// prior limits for growth
		priorLims.push_back( pair<double, double>
			(30.00, 70.0));
	
		bool reducePriorLims = true;
	#else
		std::vector< std::pair<double, double> > priorLims;
		// prior limits for theta_per_site
		priorLims.push_back( pair<double, double>
				//I think I should use 0.50*4 etc as min here
				(0.50*4*Nzero/unit, 50.0*4*Nzero/unit));
		// prior limits for growth
		priorLims.push_back( pair<double, double>
			(30.00, 70.0));
		
		bool reducePriorLims = false;
	#endif
	
	
	// a container for some other 'interesting' sets of parameters
	std::vector < std::vector < double > > otherParams;
	{
		double ptl = priorLims.front().first;
		double ptt = priorLims.front().second;
		double t1 = 4.0*4*Nzero/unit;
		if (!(t1 > ptl)) t1 = ptl + (theta_per_site - ptl)*0.8;
		assert(t1 > 0.0); 
		double t2 = 6.0*4*Nzero/unit;
		if (!(t2 < ptt)) t2 = theta_per_site + (ptt - theta_per_site)*0.05;
		assert(t2 > 0.0); 
		
		double pgl = priorLims.back().first;
		double pgt = priorLims.back().second;
		double g1 = 40.0;
		double g2 = 60.0;
		if (g1 < pgl) g1 = pgl;
		if (g2 > pgt) g2 = pgt;
		
		{
		std::vector < double > otherP(1, t1);
		otherP.push_back(g1);
		otherParams.push_back(otherP);
		}
		{
			std::vector < double > otherP(1, t1);
			otherP.push_back(g2);
			otherParams.push_back(otherP);
		}
		{
			std::vector < double > otherP(1, t2);
			otherP.push_back(g1);
			otherParams.push_back(otherP);
		}
		{
			std::vector < double > otherP(1, t2);
			otherP.push_back(g2);
			otherParams.push_back(otherP);
		}
		
	}

	
	// number of repetitions in each comparison sample generated for each parameter particle
	size_t n_reps_per_particle = 1; 
	
	
	doAHABC_KDEHeterozygosityAndSegStats(
				prefix,
				spec,
				n_obs,
				n_reps,
				nloci,
				nsites,
				priorLims,
				pop,
				n_reps_per_particle,
				trueParams,
				otherParams,
				seed,
				maxMeasure,
				reducePriorLims);
	
	return 0;
	
}


