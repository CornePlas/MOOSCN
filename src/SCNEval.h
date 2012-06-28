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

#ifndef SCNEVAL_H_
#define SCNEVAL_H_

// paradiseo files
#include <core/moeoEvalFunc.h>
#include <utils/eoRNG.h>

// own files
#include <SCN.h>
#include <read_data.h>


// c++ files
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>

// This file is adjusted somehow because there are no capactiy restrictions for the warehouses, adjust the loop for the priorities in case there are restrictions
// and maybe some other adjustments are needed

class SCNEval : public moeoEvalFunc<SCN>
{
	public:
		SCNEval(unsigned int _point1, unsigned int _point2) : cutpoint1(_point1), cutpoint2(_point2)
		{
			
			// read data files
			folder = "../data/OptionB/";
			
			// base to EP [ship]
			bep = readData(folder + "bep.csv");
			// ep to dc [Heavy Duty Truck 1-4, Train and Block-Train]
			epdc_hd1_cost = readData(folder + "epdc_dt1_costs.csv");
			epdc_hd2_cost = readData(folder + "epdc_dt2_costs.csv");
			epdc_hd3_cost = readData(folder + "epdc_dt3_costs.csv");
			epdc_hd4_cost = readData(folder + "epdc_dt4_costs.csv");
			epdc_hd_co2 = readData(folder + "epdc_dt_co2.csv");
			epdc_hd1_pm = readData(folder + "epdc_dt1_pm.csv");
			epdc_hd2_pm = readData(folder + "epdc_dt2_pm.csv");
			epdc_hd3_pm = readData(folder + "epdc_dt3_pm.csv");
			epdc_hd4_pm = readData(folder + "epdc_dt4_pm.csv");
			epdc_train_cost = readData(folder + "costs_train.csv");
			epdc_train_co2 = readData(folder + "co2_train.csv");
			epdc_train_pm = readData(folder + "pm_train.csv");
			epdc_blocktrain_cost = readData(folder + "costs_blocktrain.csv");
			epdc_blocktrain_co2 = readData(folder + "co2_train.csv");
			epdc_blocktrain_pm = readData(folder + "co2_train.csv");
			// dc to rm [Delivery Truck 1-4]
			dcrm_dt1_cost = readData(folder + "dcrm_dt1_costs.csv");
			dcrm_dt2_cost = readData(folder + "dcrm_dt2_costs.csv");
			dcrm_dt3_cost = readData(folder + "dcrm_dt3_costs.csv");
			dcrm_dt4_cost = readData(folder + "dcrm_dt4_costs.csv");
			dcrm_dt_co2 = readData(folder + "dcrm_dt_co2.csv");
			dcrm_dt1_pm = readData(folder + "dcrm_dt1_pm.csv");
			dcrm_dt2_pm = readData(folder + "dcrm_dt2_pm.csv");
			dcrm_dt3_pm = readData(folder + "dcrm_dt3_pm.csv");
			dcrm_dt4_pm = readData(folder + "dcrm_dt4_pm.csv");
			// demand + leasing handling costs
			demand = readData(folder + "demand.csv"); 
			wrc = readData(folder + "wrc.csv"); 
			
			//for (unsigned i=0; i<3; i++){
			//	for (unsigned j=0; j<3; j++){
			//		std::cout << bep[i][j] << " ";
			//	}
			//	std::cout << std::endl;
			//}

		}
		
		void operator()(SCN & _scn)
		{
			if ( _scn.invalidObjectiveVector() )
			{
				trans = rng.uniform(1.0); //use rng.uniform(3.0) for random selection
				if (trans == 1)
					trans++;
				
					
				decode(_scn);
				unsigned int nbVar = _scn.size();
				SCNObjectiveVector objVec(nbVar); // create objective vector
				objVec[0] = f_costs(_scn); // calculate costs
				objVec[1] = f_co2(_scn); // calculate CO2
				objVec[2] = f_pm(_scn); // calculate PM
				_scn.objectiveVector(objVec); // add obj.vector to object
				
				unsigned int EP = 0;
				for (unsigned e=0; e<3; e++)
					if (demep[e] > 0)
						EP++;
				_scn.setEP(EP);
				
				unsigned int DC = 0;
				for (unsigned d=0; d<16; d++){
					for (unsigned r=0; r<15; r++){
						//std::cout << dcrmv[d][r] << " ";
						if (dcrmv[d][r] > 0){
							DC++;
							break;
						}
					}
					//std::cout << std::endl;
				}
				_scn.setDC(DC);
				
				//std::cout << "No. entry points: " << _scn.getEP() << std::endl;
				
			} //end-if
			
		} // end-operator
		
	private:
		const unsigned int cutpoint1, cutpoint2;
		
		/**
		 * function to decode chromosome in usuable tables
		 * @param _scn to decode
		 */
		void decode(SCN & _scn)
		{
			// reinitialize demand
			demand = readData(folder + "demand.csv"); 
			
			// split chromosomes in two different parts for the two stages
			//first part
			SCN firstpart;
			for (unsigned int index=0; index < (cutpoint1*2); index++)
				firstpart.push_back(_scn[index]);
			//second part
			SCN secondpart;
			for (unsigned int index=(cutpoint1*2); index < (cutpoint1*2 + cutpoint2*2); index++)
				secondpart.push_back(_scn[index]);
			
			// use empty arrays | first part of algorithm/procedure
			for (int i=0; i<3; i++){ // 3 = number of EPs
				for (int j=0; j<16; j++){ // 16 = number of DCs
					epdcv[i][j] = 0;
					epdcc[i][j] = -1;
				}
				demep[i] = 0;
			}
			for (int i=0;i<16;i++){
				for (int j=0;j<15;j++){
					dcrmv[i][j] = 0;
					dcrmc[i][j] = -1;
				}
				demdc[i] = 0; // this array contains the demand of the DCs (thus for transport from EP to DC)
			}
			
			// we first decode part two of the chromosomes
			//first store priorities in an array
			int priorities[31]; // DCs + RMs = 31
			for (unsigned int i=0; i<cutpoint2; i++)
				priorities[i] = secondpart[i+cutpoint2];
				
			//then create a loop to do whole part
			for (unsigned int i=16; i<cutpoint2; i++)
			{
				// loop - for - priorities
				//find highest priority | step 2 of algorithm/procedure
				int max_priority = -1;
				int max_index = -1;
				for (unsigned int j=16; j<cutpoint2; j++)
				{
					if (priorities[j] > max_priority)
					{
						max_priority = priorities[j];
						max_index = j;
					}
				}
				
				int x,y,t;
				t = secondpart[max_index];
				if (max_index < 16){ // DC will be used as source | step 3 of algorithm/procedure
					x = max_index;
					y = findYdcrm(x,t);
				}
				else // RM will be used as source
				{
					y = max_index - 16;
					x = findXdcrm(y,t);
				}
				
				// process demand
				demdc[x] += demand[0][y];
				dcrmv[x][y] += demand[0][y];
				demand[0][y] = 0;
				dcrmc[x][y] = t;
				
				// add some check for demand constraint
				
								
				// set priority to -1, so no more concern about this one
				priorities[max_index] = -1;	
			}
			
			// check demand .. 
			for (int i=0; i<15; i++)
				if (demand[0][i] > 0)
					std::runtime_error ( "Error; demand (rm) > 0. Constraint failed." );
			
			// first part of chromosome decoding
			//first store priorities in an array
			int prios[19]; // EPs + DCs = 19
			for (unsigned int i=0; i<cutpoint1; i++)
				prios[i] = firstpart[i+cutpoint1];
			
			// create loop to do first part
			for (unsigned int i=3; i<cutpoint1; i++)
			{
				// loop - for - priorities
				//find highest priority | step 2 of algorithm/procedure
				int max_priority = -1;
				int max_index = -1;
				for (unsigned int j=3; j<cutpoint1; j++)
				{
					if (prios[j] > max_priority)
					{
						max_priority = prios[j];
						max_index = j;
					}
				}
				
				int x,y,t;
				t = firstpart[max_index];
				if (max_index < 3){ // DC will be used as source | step 3 of algorithm/procedure
					x = max_index;
					y = findYepdc(x,t);
				}
				else // RM will be used as source
				{
					y = max_index - 3;
					x = findXepdc(y,t);
				}
				
				// process demand
				//std::cout << demdc[y] << std::endl;
				epdcv[x][y] += demdc[y];
				demep[x] += demdc[y];
				demdc[y] = 0;
				epdcc[x][y] = t;
				
				// set priority to -1, so it won't be considered anymore
				prios[max_index] = -1;
			}
			
			// check supply constraint
			for (int i=0; i<16; i++)
				if (demdc[i] > 0)
					std::runtime_error ( "Error; demand (dc) > 0. Flow restriction failed." );
		}

		int findYdcrm(int _x, int _type)
		{
			int y;
			double rm[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
			int indices[15] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
			//double sum = 0;
			//if (opt_obj == -1)
			//	trans = (int) rng.uniform(2.9999999);
			//else
			//	trans = opt_obj;
			
			double min = 9999999;
			if (_type < 4){
				if (trans == 0){
					for (unsigned int i=0; i<dcrm_dt1_cost[_x].size(); i++)
					{
						rm[i] = dcrm_dt1_cost[_x][i];
						//sum += dcrm_dt1_cost[_x][i];
						if (dcrm_dt1_cost[_x][i] < min)
						{
							min = dcrm_dt1_cost[_x][i];
							y = i;
						}
					}
				}
				else if (trans == 1){
					for (unsigned int i=0; i<dcrm_dt_co2[_x].size(); i++)
					{
						rm[i] = dcrm_dt_co2[_x][i];
						//sum += dcrm_dt_co2[_x][i];
						if (dcrm_dt_co2[_x][i] < min)
						{
							min = dcrm_dt_co2[_x][i];
							y = i;
						}
					}
				}
				else if (trans == 2){
					for (unsigned int i=0; i<dcrm_dt1_pm[_x].size(); i++)
					{
						rm[i] = dcrm_dt1_pm[_x][i];
						//sum += dcrm_dt1_pm[_x][i];
						if (dcrm_dt1_pm[_x][i] < min)
						{
							min = dcrm_dt1_pm[_x][i];
							y = i;
						}
					}
				}
			}
			else{
				std::cout << "This transportation type is not supported!" << std::endl;
			}
			
			int mini; 
			double temp; int tempi;
			for (int index = 0; index < 14; index++)
			{
				mini = index;
				for (int scan = index + 1; scan < 15; scan++)
				{
					if (rm[scan] - rm[mini] < 0)
						mini = scan;
				}
				
				temp = rm[mini]; tempi = indices[mini];
				rm[mini] = rm[index]; indices[mini] = indices[index];
				rm[index] = temp; indices[index] = tempi;
			}
			
			/**
			double cumulative = 0.0;
			std::cout << std::endl;
			for (int i=0; i<5; i++){
				rm[i] = (double) rm[i] / (double) sum;
				cumulative += rm[i];
				rm[i] = cumulative;
			}*/
			
			
			
			double rnd = rng.uniform(1.0);
			if (rnd < 0.75)
				y = indices[0];
			else if (rnd < 0.9)
				y = indices[1];
			//else if (rnd < 0.9)
				//y = indices[2];
			else
				y = indices[2];
			
			return y;
		}
		
		int findYepdc(int _x, int _type)
		{
			int yvalue;
			//if (opt_obj == -1)
			//	trans = (int) rng.uniform(2.9999999);
			//else
			//	trans = opt_obj;
			
			
			double min = 9999999;
			if (_type < 4){
				if (trans == 0){
					for (unsigned int i=0; i<epdc_hd1_cost[_x].size(); i++)
					{
						if ((bep[0][_x] + epdc_hd1_cost[_x][i]) < min && demep[_x] != 0)
						{
							min = (bep[0][_x] + epdc_hd1_cost[_x][i]);
							yvalue = i;
						}
					}
				}
				else if (trans == 1){
					for (unsigned int i=0; i<epdc_hd_co2[_x].size(); i++)
					{
						if ((bep[1][_x] + epdc_hd_co2[_x][i]) < min && demep[_x] != 0)
						{
							min = bep[1][_x] + epdc_hd_co2[_x][i];
							yvalue = i;
						}
					}
				}
				else if (trans == 2){
					for (unsigned int i=0; i<epdc_hd1_pm[_x].size(); i++)
					{
						if ((bep[2][_x] + epdc_hd1_pm[_x][i]) < min && demep[_x] != 0)
						{
							min = bep[2][_x] + epdc_hd1_pm[_x][i];
							yvalue = i;
						}
					}
				}
			}
			else if (_type == 4){
				if (trans == 0){
					for (unsigned int i=0; i<epdc_train_cost[_x].size(); i++)
					{
						if ((bep[0][_x] + epdc_train_cost[_x][i]) < min && demep[_x] != 0)
						{
							min = (bep[0][i] + epdc_train_cost[_x][i]);
							yvalue = i;
						}
					}
				}
				else if (trans == 1){
					for (unsigned int i=0; i<epdc_train_co2[_x].size(); i++)
					{
						if ((bep[1][_x] + epdc_train_co2[_x][i]) < min && demep[_x] != 0)
						{
							min = (bep[1][i] + epdc_train_co2[_x][i]);
							yvalue = i;
						}
					}
				}
				else if (trans == 2){
					for (unsigned int i=0; i<epdc_train_pm[_x].size(); i++)
					{
						if ((bep[2][_x] + epdc_train_pm[_x][i]) < min && demep[_x] != 0)
						{
							min = (bep[0][_x] + epdc_train_pm[_x][i]);
							yvalue = i;
						}
					}
				}
			
			}
			else{
				std::cout << "This transportation type is not supported!" << std::endl;
			}
			
			return yvalue;
		}
		
		int findXdcrm(int _y, int _type)
		{
			int x;
			double dc[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
			double indices[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
			//double sum = 0;
			//if (opt_obj == -1)
			//	trans = (int) rng.uniform(2.9999999);
			//else
			//	trans = opt_obj;
			
			double min = 9999999;
			if (_type < 4){
				if (trans == 0){
					for (unsigned int i=0; i<dcrm_dt1_cost.size(); i++)
					{
						dc[i] = dcrm_dt1_cost[i][_y];
						//sum += dcrm_dt1_cost[i][_y];
						if (dcrm_dt1_cost[i][_y] < min)
						{
							min = dcrm_dt1_cost[i][_y];
							x = i;
						}
					}
				}
				else if (trans == 1){
					for (unsigned int i=0; i<dcrm_dt1_cost.size(); i++)
					{
						dc[i] = dcrm_dt_co2[i][_y];
						//sum += dcrm_dt_co2[i][_y];
						if (dcrm_dt_co2[i][_y] < min)
						{
							min = dcrm_dt_co2[i][_y];
							x = i;
						}
					}
				}
				else if (trans == 2){
					for (unsigned int i=0; i<dcrm_dt1_pm.size(); i++)
					{
						dc[i] = dcrm_dt1_pm[i][_y];
						//sum += dcrm_dt1_pm[i][_y];
						if (dcrm_dt1_pm[i][_y] < min)
						{
							min = dcrm_dt1_pm[i][_y];
							x = i;
						}
					}
				}
			}
			else{
				std::cout << "This transportation type is not supported!" << std::endl;
			}
			
			/**
			double cumulative = 0.0;
			for (int i=0; i<16; i++){
				dc[i] = (double) dc[i] / (double) sum;
				cumulative += dc[i];
				dc[i] = cumulative;
			}*/
			
			int mini; 
			double temp; int tempi;
			for (int index = 0; index < 15; index++)
			{
				mini = index;
				for (int scan = index + 1; scan < 16; scan++)
				{
					if (dc[scan] - dc[mini] < 0)
						mini = scan;
				}
				
				temp = dc[mini]; tempi = indices[mini];
				dc[mini] = dc[index]; indices[mini] = indices[index];
				dc[index] = temp; indices[index] = tempi;
			}
			
			
			double rnd = rng.uniform(1.0);
			if (rnd < 0.75)
				x = indices[0];
			else if (rnd < 0.9)
				x = indices[1];
			//else if (rnd < 0.9)
				//x = indices[2];
			else
				x = indices[2];
			
			return x;
			
		}
		
		int findXepdc(int _y, int _type)
		{
			int x;
			
			//if (opt_obj == -1)
			//	trans = (int) rng.uniform(2.9999999);
			//else
			//	trans = opt_obj;
			
			double min = 9999999;
			if (_type < 4){
				if (trans == 0){
					for (unsigned int i=0; i<epdc_hd1_cost.size(); i++)
					{
						if ((bep[0][i] + epdc_hd1_cost[i][_y]) < min)
						{
							min = (bep[0][i] + epdc_hd1_cost[i][_y]);
							x = i;
						}
					}
				}
				else if (trans == 1){
					for (unsigned int i=0; i<epdc_hd_co2.size(); i++)
					{
						if ((bep[0][i] + epdc_hd_co2[i][_y]) < min)
						{
							min = (bep[0][i] + epdc_hd_co2[i][_y]);
							x = i;
						}
					}
				}
				else if (trans == 2){
					for (unsigned int i=0; i<epdc_hd1_pm.size(); i++)
					{
						if ((bep[0][i] + epdc_hd1_pm[i][_y]) < min)
						{
							min = (bep[0][i] + epdc_hd1_pm[i][_y]);
							x = i;
						}
					}
				}
			}
			else if (_type == 4){
				if (trans == 0){
					for (unsigned int i=0; i<epdc_train_cost.size(); i++)
					{
						if ((bep[0][i] + epdc_train_cost[i][_y]) < min)
						{
							min = (bep[0][i] + epdc_train_cost[i][_y]);
							x = i;
						}
					}
				}
				else if (trans == 1){
					for (unsigned int i=0; i<epdc_train_co2.size(); i++)
					{
						if ((bep[0][i] + epdc_train_co2[i][_y]) < min)
						{
							min = (bep[0][i] + epdc_train_co2[i][_y]);
							x = i;
						}
					}
				}
				else if (trans == 2){
					for (unsigned int i=0; i<epdc_train_pm.size(); i++)
					{
						if ((bep[0][i] + epdc_train_pm[i][_y]) < min)
						{
							min = (bep[0][i] + epdc_train_pm[i][_y]);
							x = i;
						}
					}
				}
			}
			else{
				std::cout << "This transportation type is not supported!" << std::endl;
			}
			
			return x;
		}
		/**
		 * Calculate costs
		 * @param _scn Chromosome to calculate costs for
		 * @return costs for this chromosome/route
		 */
		double f_costs(SCN & _scn)
		{
			double costs = 0.0; // return variable
			//double subCosts = 0.0;
			
			//for (int i=0; i<3; i++)
			//	std::cout << demep[i] << " ";
			//std::cout << std::endl;
			
			//for (int i=0; i<3; i++) {
			//	for (int j=0; j<16; j++)
			//		std::cout << epdcv[i][j] << " ";
			//	std::cout << std::endl;
			//}
			
			// base to EP
			for (int i=0; i<3; i++)
				costs += bep[0][i] * demep[i];
				
			//std::cout << "BASE TO EP: " << costs << std::endl;
			//subCosts = costs;
			
			// EP to DC
			for (int i=0; i<3; i++)
			{
				for (int j=0; j<16; j++)
				{
					if (epdcc[i][j] == 0)
						costs += (epdc_hd1_cost[i][j] + wrc[0][j]) * epdcv[i][j];
					else if (epdcc[i][j] == 1)
						costs += (epdc_hd2_cost[i][j] + wrc[0][j]) * epdcv[i][j];
					else if (epdcc[i][j] == 2)
						costs += (epdc_hd3_cost[i][j] + wrc[0][j]) * epdcv[i][j];
					else if (epdcc[i][j] == 3)
						costs += (epdc_hd4_cost[i][j] + wrc[0][j]) * epdcv[i][j];
					else if (epdcc[i][j] == 4)
					{
						if (epdcv[i][j] >= 15)
							costs += (epdc_blocktrain_cost[i][j] + wrc[0][j]) * epdcv[i][j];
						else
							costs += (epdc_train_cost[i][j] + wrc[0][j]) * epdcv[i][j];
					}
					else
					{
						if ( epdcv[i][j] > 0 )
							std::cout << "Error calculating costs!" << std::endl;
					}
				}
			}
			//std::cout << "EP TO DC: " << (costs - subCosts) << std::endl;
			//subCosts = costs;
			
			// calculate transportation costs from DC to RM
			for (unsigned int i=0; i<16; i++)
			{
				for (unsigned int j=0; j<15; j++)
				{
					if (dcrmc[i][j] == 0)
						costs += dcrm_dt1_cost[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 1)
						costs += dcrm_dt2_cost[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 2)
						costs += dcrm_dt3_cost[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 3)
						costs += dcrm_dt4_cost[i][j] * dcrmv[i][j];
					else
					{
						if ( dcrmv[i][j] > 0 )
							std::cout << "Error calculating costs!" << std::endl;
					}
				}
			}
			
			//for (int i=0; i<16; i++)
			//{
			//	for (int j=0; j<15; j++)
			//		std::cout << dcrmv[i][j] << " ";
			//	std::cout << std::endl;
			//}
			
			//std::cout << "DC TO RM: " << (costs - subCosts) << std::endl;
			
			// return value
			return costs;
			
		} // end f_costs
		
		/**
		 * Calculate CO2
		 * @param _scn Chromosome to calculate CO2 for
		 * @return CO2 value
		 */
		double f_co2(SCN & _scn)
		{
			double co2 = 0.0; // return variable
			
			// base to EP
			for (int i=0; i<3; i++)
				co2 += bep[1][i] * demep[i];
			
			// EP to DC
			for (int i=0; i<3; i++)
			{
				for (int j=0; j<16; j++)
				{
					if (epdcc[i][j] <= 3)
						co2 += epdc_hd_co2[i][j] * epdcv[i][j];
					else if (epdcc[i][j] == 4)
					{
						if (epdcv[i][j] >= 15)
							co2 += epdc_blocktrain_co2[i][j] * epdcv[i][j];
						else
							co2 += epdc_train_co2[i][j] * epdcv[i][j];
					}
					else
					{
						if ( epdcv[i][j] > 0 )
							std::cout << "Error calculating co2!" << std::endl;
					}
				}
			}
			
			// calculate transportation costs from DC to RM
			for (unsigned int i=0; i<16; i++)
			{
				for (unsigned int j=0; j<15; j++)
				{
					if (dcrmc[i][j] == 0)
						co2 += dcrm_dt_co2[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 1)
						co2 += dcrm_dt_co2[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 2)
						co2 += dcrm_dt_co2[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 3)
						co2 += dcrm_dt_co2[i][j] * dcrmv[i][j];
					else
					{
						if ( dcrmv[i][j] > 0 )
							std::cout << "Error calculating co2!" << std::endl;
					}
				}
			}
			
			// return value
			return co2;
			
		} // end f_costs
		
		/**
		 * Calculate PM
		 * @param _scn chromosome to calc PM for
		 * @return PM value
		 */
		double f_pm(SCN & _scn)
		{
			double pm = 0.0; // return variable
			
			// base to EP
			for (int i=0; i<3; i++){
				pm += bep[2][i] * demep[i];
				
			}
			
			// EP to DC
			double old = pm;
			for (int i=0; i<3; i++)
			{
				for (int j=0; j<16; j++)
				{
					if (epdcc[i][j] == 0){
						pm += epdc_hd1_pm[i][j] * epdcv[i][j];
						//std::cout << epdc_hd1_pm[i][j] << " - " << epdcv[i][j] << std::endl;
					}
					else if (epdcc[i][j] == 1){
						pm += epdc_hd2_pm[i][j] * epdcv[i][j];
						//std::cout << epdc_hd2_pm[i][j] << " - " << epdcv[i][j] << std::endl;
					}
					else if (epdcc[i][j] == 2){
						pm += epdc_hd3_pm[i][j] * epdcv[i][j];
						//std::cout << epdc_hd3_pm[i][j] << " - " << epdcv[i][j] << std::endl;
					}
					else if (epdcc[i][j] == 3){
						pm += epdc_hd4_pm[i][j] * epdcv[i][j];
						//std::cout << epdc_hd4_pm[i][j] << " - " << epdcv[i][j] << std::endl;
					}
					else if (epdcc[i][j] == 4)
					{
						if (epdcv[i][j] >= 15){
							pm += epdc_blocktrain_pm[i][j] * epdcv[i][j];
							//std::cout << epdc_blocktrain_pm[i][j] << " - " << epdcv[i][j] << std::endl;
						}
						else{
							pm += epdc_train_pm[i][j] * epdcv[i][j];
							//std::cout << epdc_train_pm[i][j] << " - " << epdcv[i][j] << std::endl;
						}
					}
					else
					{
						if ( epdcv[i][j] > 0 )
							std::cout << "Error calculating PM!" << std::endl;
					}
				}
			}
			
			
			// calculate transportation costs from DC to RM
			for (unsigned int i=0; i<16; i++)
			{
				for (unsigned int j=0; j<15; j++)
				{
					if (dcrmc[i][j] == 0)
						pm += dcrm_dt1_pm[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 1)
						pm += dcrm_dt2_pm[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 2)
						pm += dcrm_dt3_pm[i][j] * dcrmv[i][j];
					else if (dcrmc[i][j] == 3)
						pm += dcrm_dt4_pm[i][j] * dcrmv[i][j];
					else
					{
						if ( dcrmv[i][j] > 0 )
							std::cout << "Error calculating PM!" << std::endl;
					}
				}
			}
			
			// return value
			return pm;
			
		} // end f_costs
		
		
		/* declare variables */
		// variables
		std::string folder;
		int trans;
		unsigned int EPs;
		unsigned int DCs;
		unsigned int RMs;
		unsigned int point1;
		unsigned int point2;
		unsigned int point3;
		unsigned int point4;
		unsigned int point5;
		unsigned int bepv[3];
		double epdcv[3][16];
		int epdcc[3][16];
		double dcrmv[16][15];
		int dcrmc[16][15];
		double demdc[16];
		double demep[3];
		
		// base to EP
		std::vector< std::vector<double> > bep;
		
		// EP to DC
		std::vector< std::vector<double> > epdc_hd1_cost;
		std::vector< std::vector<double> > epdc_hd2_cost;
		std::vector< std::vector<double> > epdc_hd3_cost;
		std::vector< std::vector<double> > epdc_hd4_cost;
		std::vector< std::vector<double> > epdc_hd_co2;
		std::vector< std::vector<double> > epdc_hd1_pm;
		std::vector< std::vector<double> > epdc_hd2_pm;
		std::vector< std::vector<double> > epdc_hd3_pm;
		std::vector< std::vector<double> > epdc_hd4_pm;
		std::vector< std::vector<double> > epdc_train_cost;
		std::vector< std::vector<double> > epdc_train_co2;
		std::vector< std::vector<double> > epdc_train_pm;
		std::vector< std::vector<double> > epdc_blocktrain_cost;
		std::vector< std::vector<double> > epdc_blocktrain_co2;
		std::vector< std::vector<double> > epdc_blocktrain_pm;
		// DC to RM
		std::vector< std::vector<double> > dcrm_dt1_cost;
		std::vector< std::vector<double> > dcrm_dt2_cost;
		std::vector< std::vector<double> > dcrm_dt3_cost;
		std::vector< std::vector<double> > dcrm_dt4_cost;
		std::vector< std::vector<double> > dcrm_dt_co2;
		std::vector< std::vector<double> > dcrm_dt1_pm;
		std::vector< std::vector<double> > dcrm_dt2_pm;
		std::vector< std::vector<double> > dcrm_dt3_pm;
		std::vector< std::vector<double> > dcrm_dt4_pm;
		// demand + warehouse costs
		std::vector< std::vector<double> > demand;
		std::vector< std::vector<double> > wrc;
		
}; // end -class

#endif
