//
//  SNE_Analyzer.h
//  10 - Stars Near Earth
//
//  Created by Justin Chiu on 12/6/17.
//  Copyright © 2017 Justin Chiu. All rights reserved.
//

#ifndef SNE_Analyzer_h
#define SNE_Analyzer_h

#include "StarNearEarth.h"
#include <math.h>

class SNE_Analyzer : public StarNearEarth {
private:
   double x, y, z;
   double distance;
public:
   void calcCartCoords();
   void calcDistance();
   double getX() { return x; }
   double getY() { return y; }
   double getZ() { return z; }
   double getDist() { return distance; }
   string coordToString();
   SNE_Analyzer& operator=( const StarNearEarth &sne );
   bool operator<(const SNE_Analyzer& rhs) const { return distance < rhs.distance; }
};

#endif /* SNE_Analyzer_hpp */
