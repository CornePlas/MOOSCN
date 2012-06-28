/**
  * This file is part MOOSCN 
  * Copyright (C) 2011-12 Corne vd Plas
  * 
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */
  
// paradiseo files
#include <moeo> // NSGA-II algorithm (+ other algorithms)
#include <archive/moeoArchive.h> // Filter non-dominated solutions
#include <es/eoRealOp.h> // for testing purpose
#include <es/eoRealInitBounded.h> // for testing purpose
//#include <moeoNormalizedSolutionVsSolutionBinaryMetric.h>

// own files
#include <SCN.cpp> // supplychain object
#include <SCNObjectiveVectorTraits.cpp> // min-max goal of objective
#include <init_pop.h> // init population
#include <SCNCrossover.h> // crossover
#include <SCNMutation.h> // mutation
#include <SCNEval.h> // eval fitness

// c++ library
#include <stdlib.h> // EXIT_SUCCESS
#include <iostream> // INPUT/OUTPUT
using namespace std;

int main(int argc,char* argv[])
{
	cout << "** Genetic Algorithm for Multi-Objective Optimization **" << std::endl; // welcome message
	
	try{ 
		// first read parameter settings from file
		eoParser parser(argc, argv); //for user-parameter reading
		//eoState state; // to keep all things allocated
		
		// get variables
		unsigned int POP_SIZE = parser.createParam((unsigned int)(500), "popSize", "Population size",'P',"Param").value();
		unsigned int MAX_GEN = parser.createParam((unsigned int)(50), "maxGen", "Maximum number of generations",'G',"Param").value();
		double P_CROSS = parser.createParam(0.7, "pCross", "Crossover probability",'C',"Param").value();
		double P_MUT = parser.createParam(0.3, "pMut", "Mutation probability",'M',"Param").value();
		unsigned int EPs = parser.createParam((unsigned int)(3),"noEPs", "Number of Entry Points",'I',"Param").value();
		unsigned int DCs = parser.createParam((unsigned int)(16),"noDCs", "Number of Distribution Centers",'J',"Param").value();
		unsigned int RMs = parser.createParam((unsigned int)(15),"noRMs", "Number of Regional Markets",'R',"Param").value();
		string OUTPUT_FILE = parser.createParam(std::string("scn_output"), "outputFile", "Path of the output file",'O',"Output").value();
		unsigned int NB_OBJ= parser.createParam((unsigned int)(3), "nbObj", "Number of Objective",'N',"Param").value();
		
		// show values
		cout << "\nParameters:\n Population: " << POP_SIZE << "\n Max no. generations: ";
		cout << MAX_GEN << "\n Crossover prob: " << P_CROSS << "\n Mutation prob: " << P_MUT << "\n" << endl; 
		
		// SET OBJECTIVES
		std::vector <bool> bObjectives(NB_OBJ);
        for (unsigned int i=0; i<NB_OBJ ; i++)
            bObjectives[i]=true;
        moeoObjectiveVectorTraits::setup(NB_OBJ,bObjectives);
		
		
		// create evaluation (fitness) object
		SCNEval eval( (EPs+DCs), (DCs+RMs) ); //priority-based encoding
		//SCNEvalPrufer eval( (EPs+DCs), DCs+RMs) ); // Prüfer number encoding
		
		// crossover object
		SCNCrossover xover( (EPs+DCs), (DCs+RMs) );
		
		// mutation object
		SCNMutation mut( (EPs+DCs), (DCs+RMs) );
		
		// init population
		eoPop<SCN> pop; // empty object to fill
		init_pop(POP_SIZE, pop, (EPs+DCs), (DCs+RMs) ); // initialize a population of size POP_SIZE in object eoPop<SCN> pop
		
		// ** TEST ** //
		double M_EPSILON = 0.01;
		//eoUniformMutation<SCN> mut (M_EPSILON);
		//eoQuadCloneOp<SCN> xover;
		//eoInitPermutation<SCN> init(8,1);
		//eoPop<SCN> pop (POP_SIZE, init);
		
		// build NSGA-II
		moeoNSGAII<SCN> nsgaII (MAX_GEN, eval, xover, P_CROSS, mut, P_MUT);
		
		// run algorithm
		cout << "Run Algorithm\n" << endl;
		nsgaII (pop);
		
		// build IBEA
		//moeoAdditiveEpsilonBinaryMetric<SCNObjectiveVector> indicator;
		//moeoHypervolumeBinaryMetric<SCNObjectiveVector> indicator;
		//moeoIBEA<SCN> ibea (MAX_GEN, eval, xover, P_CROSS, mut, P_MUT, indicator);
		//ibea (pop);
		
		// help?
		make_help(parser);
		
		// create object to filter non-dominated solutions
		moeoUnboundedArchive<SCN> arch;
		
		// filter non-dominated solutions
		//pop.sort();
		arch(pop);
		arch.sort();

		
		// print output
		arch.sortedPrintOn(std::cout);
		
		
		// print output to file
		NB_OBJ = 3;					 					// 3 = no. objectives
		ofstream outfile(OUTPUT_FILE.c_str(), ios::app);
        if ((unsigned int)outfile.tellp() != 0)
            outfile << endl;

        for (unsigned int i=0 ; i < arch.size(); i++) {
            for (int j=0 ; j<NB_OBJ; j++) {
                outfile << arch[i].objectiveVector()[j];
                if (j != NB_OBJ -1)
                    outfile << ";";
                else{
					outfile << ";" << arch[i].getEP();
					outfile << ";" << arch[i].getDC();
				}
            }
            outfile << endl;
        }

        outfile.close();
        
		
	} catch (exception& e){
		cout << e.what() << endl;
	}
	
	cout << "** FINISHED SUCCESSFULL **" << endl;
	return EXIT_SUCCESS; // program finished
}

