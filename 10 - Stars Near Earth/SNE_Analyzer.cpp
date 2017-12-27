//
//  SNE_Analyzer.cpp
//  10 - Stars Near Earth
//
//  Created by Justin Chiu on 12/6/17.
//  Copyright © 2017 Justin Chiu. All rights reserved.
//

#include "SNE_Analyzer.h"
using namespace std;


void SNE_Analyzer::calcCartCoords() {
   double degToRad = (M_PI / 180);
   
   double rAscRad  = getRAsc() * degToRad;
   double decRad   = getDec() * degToRad;
   double parallaxLY = 3.262 / getParallaxMean();
   
   x = parallaxLY * cos(decRad) * cos(rAscRad);
   y = parallaxLY * cos(decRad) * sin(rAscRad);
   z = parallaxLY * sin(decRad);
}


void SNE_Analyzer::calcDistance() {
   distance = pow(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0), 0.5);
}

string SNE_Analyzer::coordToString() {
   stringstream str;
   str << "(" << x << "," << y << "," << z << ")";
   return str.str();
}

SNE_Analyzer& SNE_Analyzer::operator=(const StarNearEarth &sne) {
   setRank(sne.getRank());
   setNameCns(sne.getNameCns());
   setNumComponents(sne.getNumComponents());
   setNameLhs(sne.getNameLhs());
   setRAsc(sne.getRAsc());
   setDec(sne.getDec());
   setPropMotionMag(sne.getPropMotionMag());
   setPropMotionDir(sne.getPropMotionDir());
   setParallaxMean(sne.getParallaxMean());
   setParallaxVariance(sne.getParallaxVariance());
   setWhiteDwarfFlag(sne.getWhiteDwarfFlag());
   setSpectralType(sne.getSpectralType());
   setMagApparent(sne.getMagApparent());
   setMagAbsolute(sne.getMagAbsolute());
   setMass(sne.getMass());
   setNotes(sne.getNotes());
   setNameCommon(sne.getNameCommon());
   
   calcCartCoords();
   calcDistance();
    
   return *this;
}


