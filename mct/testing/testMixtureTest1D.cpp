/* MCT - Markov Chains on Trees.


   Copyright (C) 2011, 2012 Jennifer Harow

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
\brief Testing MixtureTest (1-d).

A simple aabc example with an analysable target.  See the toy model in
 Beaumont et al, 'Adaptive approximate Bayesian computation', 
Biometrika (2009), 96(4), pp.983-990
*  
*/

#include "config.h"

#include "mixture_test_sampler.hpp"
#include "mixture_test.hpp"

#include "parameter_particle_validator_generic_data_object.hpp"
#include "pop_mc_abc_simulator_adaptive.hpp"

#include "parameter_prior_uniform.hpp"
#include "parameter_prior.hpp"
#include "parameter_prior_set.hpp"


#include "prng.hpp"
#include "prng_gsl.hpp"

#include <boost/shared_ptr.hpp>


#include <iostream>
#include <stack>
#include <sstream>


using namespace std;
using namespace aabc;


void miscTesting1();

int main()
{
	miscTesting1();
	
	
	return 0;
	
}

	

// test MixtureTest
void miscTesting1()
{
	try {
		std::cout << "This process id is " << getpid() << std::endl;
		
		int seed = 2345;
		boost::shared_ptr < mct::PRNGen > rr ( new mct::PRNGenGSL(seed) );
		
		// number of repetitions in each comparison sample generated for each parameter particle
		size_t n_reps = 1; 
		
		size_t iterations = 1000;
		
		// the percentile distance on which to base calculations of epsilon
		double baseEpsilonPercentile = 0.2;
		
		ParameterPriorSet pset(1);
		pset.at(0) = boost::shared_ptr < ParameterPrior > ( new ParameterPriorUniform(-10.0,10.0,rr));
		
		boost::shared_ptr < MixtureTest > ref( new MixtureTest(0.0));
		
		boost::shared_ptr < MixtureTestSampler > sampler ( new MixtureTestSampler(rr) );
		
		boost::shared_ptr < ParameterParticleValidatorGenericDataObject > 
			validator( 
			new ParameterParticleValidatorGenericDataObject(ref, sampler, n_reps) 
					);
		
		PopMCabcSimulatorAdaptive simulator(pset,
								validator,
								baseEpsilonPercentile,
								rr);
		double mydoubles[] = {0.1,0.5,0.75,1.0}; //0.01, 0.1, 0.5,1.0,1.5,2.0
		std::stack < double > epsilonMults (std::deque <double>(mydoubles, mydoubles + sizeof(mydoubles) / sizeof(double) ));
		
		std::ostringstream stm;
		stm << "MixtureTest_nr" << n_reps << "_ni_" << iterations;
		stm.precision(2);
		stm <<   fixed;
		stm <<  "_bE_" << baseEpsilonPercentile;
		std::string filenameStart = stm.str();
		std::cout << filenameStart << std::endl;
		
		//iterate
		
		while (!epsilonMults.empty()) {
			
			std::cout << "\nepsilonMult = " << epsilonMults.top() << std::endl;
			
			simulator.iterate(epsilonMults.top(), iterations);
			
			boost::shared_ptr < ParameterParticleSet > particles = simulator.getCurrentParticles();
			/*
			std::cout << "Particles:" << std::endl;
			std::cout << particles->toString() << std::endl;
			
			boost::shared_ptr < SummaryStatisticSet > stats = simulator.getCurrentSummaryStatistics();
			
			std::cout << "SummaryStatistics:" << std::endl;
			std::cout << stats->toString() << std::endl;
			
			*/
			double epsilon = simulator.getCurrentEpsilon();
			std::ostringstream stmf;
			stmf.precision(3);
			stmf <<   fixed;
		
			stmf << filenameStart << "_em_" << epsilonMults.top() 
				<< "_e_" << epsilon << ".txt";
			std::string filename = stmf.str();
			
			#if (0)
				std::cout << "outputting to file = " << filename << std::endl;
				particles->outputToFile(filename, false); // overwrite the file
			#endif
			
			epsilonMults.pop();
			
		}
		std::cout << "\n\nEnd of testing\n" << std::endl;
	
	}
	catch (std::exception& e) {
	
		std::cout << "std::exception::\n" + std::string(e.what()) << std::endl;
	}
	catch (...) {
	
		std::cout << "Unknown error\n" << std::endl;
	}
}