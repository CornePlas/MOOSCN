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

#include <SCN.h>

std::string SCN::className() const{
	return "SupplyChainNetwork";
}

void SCN::setEP(int _ep){
	EPs = _ep;
}

int SCN::getEP(){
	return EPs;
}

void SCN::setDC(int _dc){
	DCs = _dc;
}

int SCN::getDC(){
	return DCs;
}

