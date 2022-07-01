/* ------------------------------------------------------------------   */
/*      item            : StepsConverter.cxx
        made by         : Rene' van Paassen
        date            : 170721
	category        : body file 
        description     : 
	changes         : 170721 first version
        language        : C++
*/

#define StepsConverter_cxx
#include "StepsConverter.hxx"
#include <algorithm>

#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

namespace flexistick {
  static double dum = 0.0;
  
  StepsConverter::StepsConverter() :
    FlexiLink(),
    converter(1, &dum, &dum)
  {
    //
  }

  void StepsConverter::defineSteps(const std::vector<double>& coef)
  {
    double yi[coef.size()/2];
    double ui[coef.size()/2];
    for (unsigned ii = 0; ii < coef.size()/2; ii++) {
      ui[ii] = coef[2*ii];
      yi[ii] = coef[2*ii+1];
    }
    converter = dueca::StepsN(coef.size()/2, yi, ui);
  }

  StepsConverter::~StepsConverter()
  {
    //
  }

  void StepsConverter::propagate(int& v, unsigned idx)
  {
    float value = v;
    PDEB("Steps int conversion, " << v << " to " << value);    
    this->propagate(value, idx);
  }  

  void StepsConverter::propagate(float& v, unsigned idx)
  {
    float value = converter(v);
    PDEB("Steps conversion, " << v << " to " << value);
    for (t_flexilink::iterator ll = receivers.begin();
	 ll != receivers.end(); ll++) {
      ll->second->propagate(value, ll->first);
    }
  }  
}
