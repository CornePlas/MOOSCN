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

#ifndef EOUINTXOVER_H_
#define EOUINTXOVER_H_

#include <utils/eoRNG.h>

template<class Chrom> class eoUIntXover: public eoQuadOp<Chrom>
{
  public:
	
	eoUIntXover(const float& _preference = 0.5): preference(_preference)
    {
		if ( (_preference <= 0.0) || (_preference >= 1.0) )
			std::runtime_error("UxOver --> invalid preference");
	}
	
	virtual std::string className() const { return "eoUIntXover"; }
 
	bool operator()(Chrom& chrom1, Chrom& chrom2)
    {
		if ( chrom1.size() != chrom2.size())
			std::runtime_error("UxOver --> chromosomes sizes don't match" );
		
		bool changed = false;
       
        for (unsigned int i=0; i<chrom1.size(); i++)
        {
			if (chrom1[i] != chrom2[i] && rng.flip(preference))
            {
				int tmp = chrom1[i];
				chrom1[i]=chrom2[i];
				chrom2[i] = tmp;
				changed = true;
            }
         }
         
     return changed;
	}
	
	private:
		float preference;

};

#endif
