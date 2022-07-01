/* ------------------------------------------------------------------   */
/*      item            : WeightedSum.cxx
        made by         : Rene' van Paassen
        date            : 170721
	category        : body file 
        description     : 
	changes         : 170721 first version
        language        : C++
*/

#define WeightedSum_cxx
#include "WeightedSum.hxx"
#include <algorithm>

#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

namespace flexistick {

  WeightedSum::WeightedSum() :
    FlexiLink()
  {
    //
  }
  
  WeightedSum::~WeightedSum()
  {
    //
  }

  void WeightedSum::addWeight(const float& w)
  {
    weight.push_back(w);
    value.push_back(0.0f);
  }
  
  void WeightedSum::propagate(float& v, unsigned idx)
  {
    PDEB("WeightedSum conversion, idx " << idx << " val " << v
	 << " oldstate " << value[idx]);
    float res = 0.0f;
    if (value[idx] != v) {
      value[idx] = v;
      for (unsigned ii = weight.size(); ii--; ) {
        res += weight[ii] * value[ii];
      }

      PDEB("WeightedSum newstate " << res);
      for (t_flexilink::iterator ll = receivers.begin();
	   ll != receivers.end(); ll++) {
	ll->second->propagate(res, ll->first);
      }
      changed = false;
    }
  }
  
  void WeightedSum::propagate(int& v, unsigned idx)
  { float f(v); propagate(f, idx); }
  void WeightedSum::propagate(bool& v, unsigned idx)
  { float f(v ? 1.0f: 0.0f); propagate(f, idx); }
  
}
