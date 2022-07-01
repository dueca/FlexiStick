/* ------------------------------------------------------------------   */
/*      item            : WeightedSum.hxx
        made by         : Rene van Paassen
        date            : 170721
	category        : header file 
        description     : 
	changes         : 170721 first version
        language        : C++
*/

#ifndef WeightedSum_hxx
#define WeightedSum_hxx

#include "FlexiLink.hxx"
#include <vector>

namespace flexistick
{
  /** WeightedSum, creates an integer value from logicals */	
  struct WeightedSum: public FlexiLink
  {
    /** Vector of weights */
    std::vector<float> weight;

    /** Vector of current values */
    std::vector<float> value;
    
  public:
    /** Constructor */
    WeightedSum();

    /** set values */
    void addWeight(const float& w);
    
    /** Destructor */
    ~WeightedSum();

    /** Accept input & produce result */
    void propagate(bool& v, unsigned idx);
    
    /** Accept input & produce result */
    void propagate(int& v, unsigned idx);
    
    /** Accept input & produce result */
    void propagate(float& v, unsigned idx);
  };

}
#endif
