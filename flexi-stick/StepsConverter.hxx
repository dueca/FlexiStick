/* ------------------------------------------------------------------   */
/*      item            : StepsConverter.hxx
        made by         : Rene van Paassen
        date            : 170721
	category        : header file 
        description     : 
	changes         : 170721 first version
        language        : C++
*/

#ifndef StepsConverter_hxx
#define StepsConverter_hxx

#include "FlexiLink.hxx"
#include <extra/StepsN.hxx>
#include <vector>

namespace flexistick
{
  /** StepsConverter, converts an continuous input to distinct output steps. */	
  struct StepsConverter: public FlexiLink
  {
    /** Converter */
    dueca::StepsN converter;

    /** Current value */
    float value;
    
  public:
    /** Constructor */
    StepsConverter();
    
    /** Destructor */
    ~StepsConverter();

    /** Set converter */
    void defineSteps(const std::vector<double>& coef);
    
    /** Accept input & produce result */
    void propagate(float& v, unsigned idx);

    /** Accept input & produce result */    
    void propagate(int& v, unsigned idx);
  };

}
#endif
