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

#ifndef _SCNCROSSOVER_H_
#define _SCNCROSSOVER_H_

// own files
#include <SCN.h>
#include <eoUIntXover.h>

// paradiseo 
#include <utils/eoRNG.h>
#include <eoOrderXover.h>

class SCNCrossover : public eoQuadOp<SCN>
{
	
	public:
		SCNCrossover(unsigned int _point1, unsigned int _point2) : point1(_point1), point2(_point2)
		{}
		
		virtual std::string className() const{
			return "SCNCrossover";
		}
		
		bool operator ()(SCN & _scn1, SCN & _scn2)
		{
			// variable to indicate whether crossover is applied
			bool changed = false; 
			
			// temp objects to use for crossover
			SCN temp_1,temp_2;
			
			// variables to determine start and end positions
			int start, end;

			// determine whether transportation types or priority-based should be crossed-over
			if ( rng.flip() )
			{ // transportation types
			
				// uniform crossover for transport parts
				eoUIntXover<SCN> cross;
				
				// determine whether first or second part should be chosen for crossover
				if ( rng.flip() )
				{ // use first part
					start = 0;
					end = point1;
				} // end-if
				else
				{ // use second part
					start = point1 * 2;
					end = start + point2;
				} // end-else
			
				// for-loop to put parts in objects
				for (int index = start; index < end; index++)
				{
					temp_1.push_back(_scn1[index]);
					temp_2.push_back(_scn2[index]);
				} //end-for
					
				// apply crossover
				cross(temp_1, temp_2);
				
				// replace originals
				for (int index = start; index < end; index++)
				{
					_scn1[index] = temp_1[index-start];
					_scn2[index] = temp_2[index-start];
				} //end-for
				
				// change bool
				changed = true;
				
			} //end-if
			else
			{ // priority-based parts
			
				// both priority parts can use eoOrderXover
				eoOrderXover<SCN> cross;
				
				// determine whether first or second part of priority-based in individual should be used
				if (rng.flip() )
				{ // use first part
					start = point1;
					end = point1 + start;
				} //end-if
				else
				{ // use second part
					start = point1 * 2 + point2;
					end = point2 + start;
				} // end-else
				
				// for-loop to put parts in objects
				for (int index = start; index < end; index++)
				{
					temp_1.push_back(_scn1[index]);
					temp_2.push_back(_scn2[index]);
				} //end-for
					
				// apply crossover
				cross(temp_1, temp_2);
				
				// replace originals
				for (int index = start; index < end; index++)
				{
					_scn1[index] = temp_1[index-start];
					_scn2[index] = temp_2[index-start];
				} //end-for
				
				// change bool
				changed = true;
				
			} // end-else priority-based part
			
			return changed; // return variable if something has changed
		} // end operator
		
	private:
		unsigned int point1;
		unsigned int point2;

}; // end class

#endif
