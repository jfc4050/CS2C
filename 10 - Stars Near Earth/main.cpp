//
//  main.cpp
//  10 - Stars Near Earth
//
//  Created by Justin Chiu on 12/6/17.
//  Copyright © 2017 Justin Chiu. All rights reserved.
//

#include "StarNearEarth.h"
#include "SNE_Analyzer.h"
#include <iostream>
#include <vector>

#define NUM_TO_PLOT 9
#define PRINTWIDTH 1


void printPlot(const vector<vector<char>>& plot) {
   for (const vector<char>& row : plot) {
      for (const char& val : row) {
         cout << val << " ";
      } cout << endl;
   }
}


int main() {
    StarNearEarthReader starInput("/Users/justin/Downloads/CS_2C_Client_Support/StarNearEarth Folder/nearest_stars.txt");
    
    if (starInput.readError()) {
        cout << "couldn't open " << starInput.getFileName() << " for input.\n";
        exit(1);
    }
    
    cout << starInput.getFileName() << endl
         << starInput.getNumStars() << endl;
    
    // create an array of objects for our own use:
    int arraySize = starInput.getNumStars();
    SNE_Analyzer *starArray = new SNE_Analyzer[arraySize];
    for (int k = 0; k < arraySize; k++)
        starArray[k] = starInput[k];
   
   int zeroCoord = 0;
   // display cartesian coords
   for (int k = 0 ; k < NUM_TO_PLOT ; k++) {
      cout << starArray[k].getNameCommon() << " "
           << starArray[k].coordToString() << " "
           << starArray[k].getDist()       << endl;
      if ( zeroCoord < round(abs(starArray[k].getX())) )
         zeroCoord = round(abs(starArray[k].getX()));
      if ( zeroCoord < round(abs(starArray[k].getY())) )
         zeroCoord = round(abs(starArray[k].getY()));
      if ( zeroCoord < round(abs(starArray[k].getZ())) )
         zeroCoord = round(abs(starArray[k].getZ()));
   } cout << "\n";
   
   int matSize = (zeroCoord * 2) + 1;
   vector<vector<char>> XYplot(matSize, vector<char>(matSize, '*'));
   vector<vector<char>> XZplot(matSize, vector<char>(matSize, '*'));
   vector<vector<char>> YZplot(matSize, vector<char>(matSize, '*'));
   XYplot[zeroCoord][zeroCoord] = 'S';
   XZplot[zeroCoord][zeroCoord] = 'S';
   YZplot[zeroCoord][zeroCoord] = 'S';
   
   //Populate Plots w/ stars
   for (int k = 0 ; k < NUM_TO_PLOT ; ++k) {
      char starVal = '0' + (k + 1);
      XYplot[round(zeroCoord - starArray[k].getY())]
            [round(zeroCoord + starArray[k].getX())] = starVal;
      XZplot[round(zeroCoord - starArray[k].getZ())]
            [round(zeroCoord + starArray[k].getX())] = starVal;
      YZplot[round(zeroCoord - starArray[k].getZ())]
            [round(zeroCoord + starArray[k].getY())] = starVal;
   }

   cout << "XY PLOT" << endl;
   printPlot(XYplot);
   cout << "\n\n";
   
   cout << "XZ PLOT" << endl;
   printPlot(XZplot);
   cout << "\n\n";
   
   cout << "YZ PLOT" << endl;
   printPlot(YZplot);
   cout << "\n\n";
   
   
   delete[] starArray;
   return 0;
}
