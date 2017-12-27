// File StarNearEarth.h
// Interface for classes
//   StarNearEarth - Single star object from the RECONS database
//   StarNearEarthReader - Used to read and return StarNearEarth objects
// Author: Michael Loceff c 2009

#ifndef StarNearEarth_H
#define StarNearEarth_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

class StarNearEarth
{
public:
   static const unsigned int MIN_STRING = 0;
   static const unsigned int MAX_STRING = 100;
   static const int MIN_DUB = -1000;
   static const int MAX_DUB = 1000;
private:
   string nameCns, spectralType, notes,  nameCommon;
   int rank, nameLhs, numComponents;
   double rAsc, decl,  propMotionMag, propMotionDir, parallaxMean, 
      parallaxVariance, magApparent, magAbsolute, mass;
   bool whiteDwarfFlag;

public:
   StarNearEarth();

   //mutators
   bool setNameCns(string strArg);
   bool setSpectralType(string strArg);
   bool setNotes(string strArg);
   bool setNameCommon(string strArg);

   bool setRank(int intArg);
   bool setNameLhs(int intArg);
   bool setNumComponents(int intArg);

   bool setRAsc(double dblArg);
   bool setDec(double dblArg);
   bool setPropMotionMag(double dblArg);
   bool setPropMotionDir(double dblArg);
   bool setParallaxMean(double dblArg);
   bool setParallaxVariance(double dblArg);
   bool setMagApparent(double dblArg);
   bool setMagAbsolute(double dblArg);
   bool setMass(double dblArg);

   void setWhiteDwarfFlag(bool boolArg);

   // accessors
   string getNameCns() const { return nameCns; }
   string getSpectralType() const { return spectralType; }
   string getNotes() const { return notes; }
   string getNameCommon() const { return nameCommon; }

   int getRank() const { return rank; }
   int getNameLhs() const { return nameLhs; }
   int getNumComponents() const { return numComponents; }

   double getRAsc() const { return rAsc; }
   double getDec() const { return decl; }
   double getPropMotionMag() const { return propMotionMag; }
   double getPropMotionDir() const { return propMotionDir; }
   double getParallaxMean() const { return parallaxMean; }
   double getParallaxVariance() const { return parallaxVariance; }
   double getMagApparent() const { return magApparent; }
   double getMagAbsolute() const { return magAbsolute; }
   double getMass() const { return mass; }
   bool getWhiteDwarfFlag() const { return whiteDwarfFlag; }

   // comparator tools
   // could use static const int, instead:
private:
   static int sortKey;

public:
   enum 
   {
      SORT_BY_NAME_CNS, SORT_BY_SPECTRAL_TYPE, SORT_BY_NAME_COMMON, SORT_BY_RANK,
      SORT_BY_NAME_LHS, SORT_BY_NUM_COMPONENTS, SORT_BY_RA, SORT_BY_DEC, SORT_BY_PROP_MOTION_MAG,
      SORT_BY_PROP_MOTION_DIR, SORT_BY_PARALLAX_MEAN, SORT_BY_PARALLAX_VARIANCE, 
      SORT_BY_MAG_APPARENT, SORT_BY_MAG_ABSOLUTE, SORT_BY_MASS
   };
   static bool setSortType( int whichType );
   bool operator<(const StarNearEarth &other) const;
   bool operator>(const StarNearEarth &other) const;
   bool operator==(const StarNearEarth &other) const;
   bool operator!=(const StarNearEarth &other) const;};

class StarNearEarthReader
{
private:
   vector <StarNearEarth> stars;
   int numStars;
   bool fileOpenError;
   string starFile;
   StarNearEarth readOneStar(string line);
   bool isDataLine(int line_num, string line);


public:
   StarNearEarthReader(string fileName);
   StarNearEarth &operator[](int k);
   string getFileName() { return starFile; }
   bool readError() { return fileOpenError; }
   int getNumStars() { return numStars; }

   // static utilities for converting strings and RA/DEC to single floats
   static double convertRA(string strRA);   // "HH MM SS.S" -> double
   static double convertDEC(string strDMS); // "DDD MM SS.S" -> double
   static double convertToDouble(string strToCnvrt);

private:
   static double dmsToFloatDegree(int deg, int min, double sec);
   static double hmsToFloatDegree(int hr, int min, double sec);
};
#endif