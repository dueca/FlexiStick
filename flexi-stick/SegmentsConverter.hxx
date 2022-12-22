/* ------------------------------------------------------------------   */
/*      item            : SegmentsConverter.hxx
        made by         : Rene van Paassen
        date            : 170721
	category        : header file 
        description     : 
	changes         : 170721 first version
        language        : C++
*/

#ifndef SegmentsConverter_hxx
#define SegmentsConverter_hxx
#include "FlexiLink.hxx"
#include <vector>
#include <extra/SimpleFunction.hxx>

namespace dueca {

  /** Implementation of a scaling/converting device, that produces a
      piecewise continuous function. 

      To create the object, call the constructor with a list of desired
      output values and a list of corresponding raw input values. When
      called to convert an input value, the output is calculated on the
      basis of the linear interpolation */
  
  class Segments: public SimpleFunction
  {
    /** Dimension of segments */
    unsigned int n;

    /** Array of output values. */
    double* y;

    /** Array of input values. */
    double* ui;
    
    /** Array of slopes */
    double *a;
    
  public:
    
    /** Constructor.
	\param n     Number of input-output value pairs.
	\param yi    Set of possible output values points.
	\param ui    Input values corresponding to the output
               	     values. */
    Segments(unsigned int n, double yi[], double ui[]);
    
    /** Copy constructor */
    Segments(const Segments& o);

    /** Assignment */
    Segments& operator = (const Segments& o);

    /** Destructor. */
    ~Segments();

    /** The operator.
	@param x     Raw value
	@returns     Converted output. */
    double operator () (const double x) const;

    /** Print the object */
    std::ostream& print (std::ostream& os) const;
  };
}

namespace flexistick
{
  /** SegmentsConverter, converts an continuous input to piecewise
      output segments. */	
  struct SegmentsConverter: public FlexiLink
  {
    /** Converter */
    dueca::Segments converter;

    /** Current value */
    float value;
    
  public:
    /** Constructor */
    SegmentsConverter();
    
    /** Destructor */
    ~SegmentsConverter();

    /** Set converter */
    void defineSegments(const std::vector<double>& coef);
    
    /** Accept input & produce result */
    void propagate(float& v, unsigned idx);

    /** Accept input & produce result */    
    void propagate(int& v, unsigned idx);
  };

}
#endif
