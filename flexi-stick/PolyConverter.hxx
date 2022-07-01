/* ------------------------------------------------------------------   */
/*      item            : PolyConverter.hxx
        made by         : Rene van Paassen
        date            : 170721
        category        : header file
        description     :
        changes         : 170721 first version
        language        : C++
*/

#ifndef PolyConverter_hxx
#define PolyConverter_hxx

#include "FlexiLink.hxx"
#include <extra/PolynomialN.hxx>
#include <vector>

namespace flexistick
{
  /** PolyConverter, uses a polynomial to convert an input value */
  struct PolyConverter: public FlexiLink
  {
    /** Converter */
    dueca::PolynomialN converter;

    /** Current value */
    float value;

  public:
    /** Constructor */
    PolyConverter();

    /** Destructor */
    ~PolyConverter();

    /** Set the final polynomial */
    void definePoly(const std::vector<double>& coef);

    /** Accept input & produce result */
    void propagate(float& v, unsigned idx);

    /** Accept input & produce result */
    void propagate(int& v, unsigned idx);
  };

}
#endif
