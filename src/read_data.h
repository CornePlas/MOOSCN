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

#ifndef READDATA_H_
#define READDATA_H_

// include needed c++ libraries
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string.h>

using namespace std;

/**
 * This function needs readCSV, it read the CSV file and store it in a vector of double vectors
 * @param _file string of the filename
 * @return vector of double vectors filled with data from file
 */
std::vector< std::vector<double> > readData(string _file)
{
	char *buf;
	buf = new char[_file.size()];
	strcpy(buf, _file.c_str());


	// open file to read, check also whether file exists
	fstream file(buf, ios::in);
	if (!file.is_open())
		std::cout << "File " << _file << "not found" << endl;
		
	// create empty array to put values in, and read/parse file	
	std::vector< std::vector<std::string> > csvData;
	std::string csvLine;
	
	while ( std::getline(file, csvLine) )
	{
		std::istringstream csvStream(csvLine);
		std::vector<std::string> csvColumn;
		std::string csvElement;
		//std::cout << csvLine << std::endl;
		
		while ( std::getline(csvStream, csvElement, ';') )
		{
			//std::cout << csvElement;
			csvColumn.push_back(csvElement);
		}
		csvData.push_back(csvColumn);
	}
	
	// create empty double array for the data
	std::vector< std::vector<double> > data;
	for (unsigned int i=0; i<csvData.size(); i++)
	{
		// row to put in vector
		std::vector<double> dRule;
		for (unsigned int j=0; j<csvData[i].size(); j++)
		{
			// convert string to double
			double value = atof(csvData[i][j].c_str());
			dRule.push_back( value );
		}
		// add vector to `matrix'
		data.push_back(dRule);
	}
	
	delete []buf;
	// return data 
	return data;
}

#endif
