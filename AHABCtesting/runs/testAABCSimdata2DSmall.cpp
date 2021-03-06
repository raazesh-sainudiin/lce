/* MCT - Markov Chains on Trees.


   Copyright (C) 2011, 2012 Jennifer Harlow

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
\brief Do a number of AABC runs using a mct::MultiLociSimDataSampler.

Just heterozygosity and theta, with small prior range
* 
Aim is to compare 'product likelihood' of separate results with 
posterior for all loci together

* Uses theta, and heterozygosity

*/

#include "testing_toolsSimdata.hpp"
#include "testing_toolsAABC_GR.hpp"
#include "automcmc_factory.hpp"

#include "config.h" // mct

#include "summary_statistic_set.hpp"

#include "multi_loci_polytable_set.hpp"

#include "prng.hpp"
#include "prng_gsl.hpp"

#include "utilities.hpp"

#include <mscplusplus/population_structure.hpp>

#include <boost/shared_ptr.hpp>



#include <iostream>
#include <vector>
#include <stack>
#include <utility>

using namespace std;
using namespace mct;


void runAABC_Simdata();

int main()
{
	try {
		runAABC_Simdata();
	
	
		return 0;
	}
	catch (std::exception& e) {
		std::cout << "Exception " << e.what() << std::endl; 
		throw;
	}
	catch (...) {
		std::cout << "Unknown error " << std::endl; 
		throw;
	}

	
}

	

void runAABC_Simdata()
{
	
	
		
	int myPid = getpid();
	std::cout << "Simdata aabc 2d small" << std::endl;
	std::cout << "This process id is " << myPid << std::endl;
	
	
	std::string baseOutputDir("../figures/");
	
	std::string thisDir("AABCSimData2DSmallNewParamsNew");
	
	std::string descriptionString("AABCsimdata2DGR");
	
	int seed = 2345;
	boost::shared_ptr < PRNGen > r ( new PRNGenGSL(seed) );
	
	/* How many iterations we want in each generation - ie how many members of the 
	 * population we are aiming for in each generation */
	size_t iterations = 10000;
	
	bool doIndividual = true; // whether to do individual posteriors
	size_t n_obs = 10; // number of 'observed values' to have
	
	size_t nsam = 10; // number of individuals in a sample
			
	// make the output dir and get back path
	if (!doIndividual) thisDir += "Conly";
	std::string path = mct_utilities::makeDir(baseOutputDir, thisDir);
	string prefix = path + "/" + descriptionString;
	
	// loci in each multi-loci SimData
	size_t nloci = 1; 

	size_t nsites = 25000; // number of sites in each locus 
	
	size_t Nzero = 1000000; // effective popn size (per sub-pop)
	double unit = 100000000.0; // 10^8
	
	//true mutation rate
	//mutation rates PER SITE ie 4N0(mu/site)
	double mu = 5.0; //before scaling by dividing by unit
	double theta_per_site = mu*4*Nzero/unit;
	
	//true growth rates
	double growth = 50.0;
	
	std::vector < double > trueParams (1, theta_per_site);

	std::vector< std::pair<double, double> > priorLims;
	// prior limits for theta_per_site
	priorLims.push_back( pair<double, double>
			//I think I should use 0.50*4 etc as min here
			(0.50*4*Nzero/unit, 50.0*4*Nzero/unit));



	/* Create an empty population structure */
	boost::shared_ptr< hudson_ms::PopulationStructure > pop
	(new hudson_ms::PopulationStructure());
	/* Add a  simple one-subpop population structure for nsam samples, growth */
	pop->add(nsam, growth);

	int trueseed = seed + 1;

	boost::shared_ptr < MultiLociPolyTableSet > refObjSetPtr
							= makeSimDataRefSet(
										n_obs,
										nloci,
										nsites,
										pop,
										trueParams,
										trueseed);
	
	/* and get a set of reference summary statistics from the values,
	getting just the heterozygosity */
	boost::shared_ptr < mct::SummaryStatisticSet > refset_ptr_tmp =
					refObjSetPtr->getSummaryStatisticSet();

	std::vector < size_t > reqStats(1, 2);
	boost::shared_ptr < mct::SummaryStatisticSet > refSetSumStatsPtr =
				refset_ptr_tmp->makeSelectedSummaryStatisticSet(reqStats);

	// stuff for the aabc simulation
	// number of repetitions in each comparison sample generated for each parameter particle
	size_t n_reps_per_particle = 1; 
	
	// the percentile distance on which to base calculations of epsilon
	double baseEpsilonPercentile = 0.1;
	
	/* In each generation, we get tighter and tighter in our 
	 * idea of 'close enough'.  These multipliers are applied 
	 * progressively, ie 1.0 * the value we get from the base 
	 * epsilon criteria for the first loop, then 0.75 * that, then
	 * 0.75 * that, then 0.5 * that */
	double mydoubles[] = {0.5, 0.75, 0.75, 1.0}; 
	std::stack < double > epsilonMults (std::deque <double>(
			mydoubles, mydoubles + sizeof(mydoubles) / sizeof(double) ));

	ahabc::AutoMCMCFactory::AUTOTYPE autotype = ahabc::AutoMCMCFactory::ALLTHREE;
	
	string spec;
	{
		std::ostringstream stm;
		#if(0)
			stm << ahabc::AutoMCMCFactory::getDiagnosticName(autotype);
		#endif
		stm << "_s_" << nsam << "_ns_" << nsites;
		stm.precision(mct_utilities::fitPrecision(theta_per_site));
		stm <<   fixed;
		stm << "_th_" << theta_per_site;
		stm << "_ni_" << iterations;
		
		spec = stm.str();
	}
	
	// note that we do the heterozygosity simulation here
	doAABCSimsMultLociPolytableHeterozygosity(prefix,
				spec,
				r,
				iterations,
				n_obs,
				nloci,
				nsites,
				priorLims,
				pop,
				refSetSumStatsPtr,
				refObjSetPtr,
				n_reps_per_particle,
				baseEpsilonPercentile,
				epsilonMults,
				autotype,
				doIndividual);
}


