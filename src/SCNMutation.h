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

#ifndef SCNMUTATION_H_
#define SCNMUTATION_H_

// include own files
#include <SCN.h>

// include paradiseo files
#include <utils/eoRNG.h>
#include <eoSwapMutation.h>
#include <eoShiftMutation.h>
#include <eoTwoOptMutation.h>

class SCNMutation : public eoMonOp<SCN>
{
	public:
		SCNMutation(unsigned int _point1, unsigned int _point2) : cutpoint1(_point1), cutpoint2(_point2)
		{}
		
		virtual std::string className() const{
			return "SCNMutation";
		}
		
		bool operator ()(SCN & _scn)
		{
			// variable to indicate whether something has changed
			bool anythingChanged = false;
			
			// variables to use for positions in chromosomes
			unsigned int start, end;
			
			// determine whether mutation should take place in transport part or priority-based part
			if ( rng.flip() )
			{ // transport part
				
				// determine to use first or second part of chain
				if ( rng.flip() )
				{ // use first
					start = 0;
					end = cutpoint1;
				}
				else
				{ // use second
					start = cutpoint1 * 2;
					end = cutpoint2 + start;
				}
				
			} // end-if
			else
			{ // priority-based part
				
				// determine to use first or second part of chain
				if ( rng.flip() )
				{ // use first
					start = cutpoint1;
					end = cutpoint1 + start;
				}
				else
				{ // use second
					start = cutpoint1 * 2 + cutpoint2;
					end = cutpoint2 + start;
				}
				
				// change bool variable
				anythingChanged = true;
				
			} // end-else
			
			// *apply mutation*
			// temporary object to apply mutation on
			SCN temp;
			// use for-loop to filter needed part
			for (unsigned int i=start; i<end; i++)
				temp.push_back(_scn[i]);

			// use random type of mutation
			int type = (int) rng.uniform(2.999999999);
			if ( type == 0 )
			{
				// apply swap-mutation
				eoSwapMutation<SCN> swap;
				swap(temp);
			}
			else if (type == 1)
			{
				// apply shift-mutation
				eoShiftMutation<SCN> shift;
				shift(temp);
			}
			else
			{
				// apply two-opt-mutation
				eoTwoOptMutation<SCN> twoOpt;
				twoOpt(temp);
			}
			
			// replace values in original object		
			for (unsigned int j=0; j<temp.size(); j++)
				_scn[j+start] = temp[j];
			
			// change bool
			anythingChanged = true;

			return anythingChanged;
			
		} // end operator
	
	private:
		unsigned int cutpoint1,cutpoint2;

}; // end class

#endif
