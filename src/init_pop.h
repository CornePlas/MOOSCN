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

#ifndef _INIT_POP_H_
#define _INIT_POP_H_

#include <eoPop.h>
#include <eoInit.h>
#include <utils/eoRNG.h>

#include <SCN.h>

/**
 * initialization procedure for the population
 * @param _popsize Population size
 * @param pop Object which contains the whole population
 * @param _eptodc length of priority based for first part of the chain: EP to DC
 * @param _dctorm length of priority based part for the chain: DC to RM
 */
void init_pop(unsigned int _popsize, eoPop<SCN> & _pop, unsigned int _eptodc, unsigned int _dctorm)
{
	// declare constants for initialization
	const double same_type = 0.75; // percentage for using same type
	const double extreme_points = 0.5; // percentage for searching for extreme points
	const double extreme_costs = 0.75; // percentage for minimizing costs;;
	const double no_types_part1 = 4.999999; // used for rng.uniform [thus returns integers 0-4]
	const double no_types_part2 = 3.999999; // used for rng.uniform [thus returns integers 0-3]
	
	
	// create loop to initialize all chromosomes
	for (unsigned int number = 0; number < _popsize; number++)
	{ //start for-loop
		
		// temporary objects to initialize and add later to population
		SCN priority_first_part; SCN priority_second_part;
		SCN total; // object to combine different parts
		
		// first initialize priority-based parts to get the size
		eoInitPermutation<SCN> init_first(_eptodc,1); // length is no. EPs + DCs
		init_first(priority_first_part);
		
		// first do permutation for priority-based part to get size for transportation
		eoInitPermutation<SCN> init_second(_dctorm,1); // length is no. DCs + RMs;
		init_second(priority_second_part);
		
		// initialize transportation type
		if (rng.uniform(1.0) > same_type ) // determine whether random type of fixed type will be used
		{
			//first part
			for (unsigned k=0; k<priority_first_part.size(); k++)
			{
				int type_part1 = (int) ( rng.uniform(no_types_part1) );
				total.push_back( type_part1 );
			}
			
			// add first priority-based part to chromosome
			for (unsigned int index_first = 0; index_first < priority_first_part.size(); index_first++)
				total.push_back( priority_first_part[index_first] );
			
			//second part
			for (unsigned k=0; k<priority_second_part.size(); k++)
			{
				int type_part2 = (int) ( rng.uniform(no_types_part2) );
				total.push_back( type_part2 );
			}
		}
		else // use fixed transport type 
		{
			// declare variables
			int type_part1, type_part2;
			bool sameTransport = false;
			bool extremeCosts = false;
			
			// random change to search for a extreme point; this means either truck 1 / train is used or truck type 4
			if (rng.uniform(1.0) < extreme_points){
				sameTransport = true;
				if (rng.uniform(1.0) < extreme_costs){
					extremeCosts = true;
					if ( rng.flip() )
						type_part1 = 0;
					else 
						type_part1 = 4;
				}
				else{
					type_part1 = 3; // use truck 4; best for minimizing PM
				}
			}
			else{
				type_part1 = (int) ( rng.uniform(no_types_part1) );
			}
			
			// first part
			for (unsigned k=0; k<priority_first_part.size(); k++)
				total.push_back( type_part1 );

				
			// add first priority-based part to chromosome
			for (unsigned int index_first = 0; index_first < priority_first_part.size(); index_first++)
				total.push_back( priority_first_part[index_first] );
				
			//second part
			if (sameTransport)
			{
				if (type_part1 < 4)
					type_part2 = type_part1;
				else {
					if (extremeCosts)
						type_part2 = 0;
					else
						type_part2 = (int) ( rng.uniform(no_types_part2) );
				}
						
				for (unsigned k=0; k<priority_second_part.size(); k++)
					total.push_back( type_part2 );
			}
			else {
				for (unsigned k=0; k<priority_second_part.size(); k++)
					total.push_back( (int) rng.uniform(no_types_part2) );
			}
					
		}
		
		
		// second part priority-based encoding
		for (unsigned int index_second = 0; index_second < priority_second_part.size(); index_second++)
			total.push_back( priority_second_part[index_second] );
		
		_pop.push_back(total);
		
	} //end for-loop
	
	/*
	// create loop to initialize all chromosomes
	for (unsigned int number = 0; number < _popsize; number++)
	{ //start for-loop
	//manually added
	// temporary objects to initialize and add later to population
		SCN priority_first_part; SCN priority_second_part;
		SCN total; // object to combine different parts
		
		// first initialize priority-based parts to get the size
		eoInitPermutation<SCN> init_first(_eptodc,1); // length is no. EPs + DCs
		init_first(priority_first_part);
		
		// first do permutation for priority-based part to get size for transportation
		eoInitPermutation<SCN> init_second(_dctorm,1); // length is no. DCs + RMs;
		init_second(priority_second_part);
		
		
		total.push_back(0);
		total.push_back(0);
		total.push_back(0);
		total.push_back(4);
		total.push_back(4);
		total.push_back(4);
		total.push_back(0);
		total.push_back(0);
		total.push_back(4);
		total.push_back(4);
		total.push_back(4);
		total.push_back(0);
		total.push_back(0);
		total.push_back(0);
		total.push_back(0);
		total.push_back(0);
		total.push_back(4);
		total.push_back(4);
		total.push_back(0);
		
		
		//for (int i=0; i<priority_first_part.size(); i++)
		//	total.push_back(3); // change to zero for optimum costs
		// add first priority-based part to chromosome
		for (unsigned int index_first = 0; index_first < priority_first_part.size(); index_first++)
			total.push_back( priority_first_part[index_first] );
		for (int i=0; i<priority_second_part.size(); i++)
			total.push_back(0); // change to zero for optimum costs
		// second part priority-based encoding
		for (unsigned int index_second = 0; index_second < priority_second_part.size(); index_second++)
			total.push_back( priority_second_part[index_second] );
			
		_pop.push_back( total );
		
	}
	*/
	
} // end void

#endif 
