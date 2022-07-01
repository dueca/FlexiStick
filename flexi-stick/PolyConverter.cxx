/* ------------------------------------------------------------------   */
/*      item            : PolyConverter.cxx
        made by         : Rene' van Paassen
        date            : 170721
	category        : body file 
        description     : 
	changes         : 170721 first version
        language        : C++
*/

#define PolyConverter_cxx
#include "PolyConverter.hxx"
#include <algorithm>

#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

namespace flexistick {

  static double dum = 0.0;
  
  PolyConverter::PolyConverter() :
    FlexiLink(),
    converter(0, &dum)
  {
    //
  }

  void PolyConverter::definePoly(const std::vector<double>& coef)
  {
    converter = dueca::PolynomialN(coef.size() - 1, coef.data()); 
  }

  PolyConverter::~PolyConverter()
  {
    //
  }

  void PolyConverter::propagate(int& v, unsigned idx)
  {
    float value = v;
    PDEB("Poly int conversion, " << v << " to " << value);    
    this->propagate(value, idx);
  }  

  void PolyConverter::propagate(float& v, unsigned idx)
  {
    float value = converter(v);
    PDEB("Poly conversion, " << v << " to " << value);    
    for (t_flexilink::iterator ll = receivers.begin();
	 ll != receivers.end(); ll++) {
      ll->second->propagate(value, ll->first);
    }
  }  
}
