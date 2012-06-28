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

#ifndef SCN_H_
#define SCN_H_

#include <SCNObjectiveVector.h>
#include <core/moeoVector.h>

class SCN : public moeoVector<SCNObjectiveVector, unsigned int, double>
{
	private:
		int EPs;
		int DCs;
		//SCN() { } // private default constructor
	
	public:
		std::string className() const;
		void setEP(int _ep);
		int getEP();
		void setDC(int _dc);
		int getDC();
		
};

#endif
