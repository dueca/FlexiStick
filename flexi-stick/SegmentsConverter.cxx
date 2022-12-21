/* ------------------------------------------------------------------   */
/*      item            : SegmentsConverter.cxx
        made by         : Rene' van Paassen
        date            : 170721
	category        : body file 
        description     : 
	changes         : 170721 first version
        language        : C++
*/

#define SegmentsConverter_cxx
#include "SegmentsConverter.hxx"
#include <algorithm>

#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

DUECA_NS_START

class segments_exception: public std::exception
{
public:
  /** Re-implementation of std:exception what. */
  const char* what() const throw() {return "segments need 2 or more values"; }
};


static void Segments_sort_and_copy(double *ui, double *yi,
				   double *a, double *y, int n)
{
  // creating this with n < 1 is not possible
  if (n < 2) {
    throw(segments_exception());
  }

  // for efficient look-up, values must be sorted in increasing
  // order. Start with a simple bubble sort
  int order[n];
  for (int ii = n; ii--; ) order[ii] = ii;

  bool sorted = false;
  while (!sorted) {
    sorted = true;
    for (int ii = n; --ii; ) {
      if (ui[order[ii]] < ui[order[ii-1]]) {
        sorted = false;        int tmp = order[ii];
        order[ii] = order[ii-1]; order[ii-1] = tmp;
      }
    }
  }

  // copy the result, calculate the slopes
  for (int ii = n; --ii; ) {
    y[ii] = yi[order[ii]];
    a[ii-1] = (yi[order[ii]] - yi[order[ii-1]]) /
      (ui[order[ii]] + ui[order[ii-1]]);
  }
  y[0] = yi[order[0]];
}

Segments::Segments(unsigned int n, double yi[], double ui[]) :
  n(n),
  y(new double[n > 1 ? n : 1]),
  ui(new double[n > 1 ? n : 1]),
  a(new double[n > 2 ? n - 1 : 1])
{
  Segments_sort_and_copy(ui, yi, a, y, n);
  for (unsigned ii=n; ii--; ) {
    this->ui[ii] = ui[ii];
  }
}

Segments::Segments(const Segments& o) :
  n(o.n),
  y(new double[o.n > 1 ? o.n : 1]),
  ui(new double[o.n > 1 ? o.n : 1]),
  a(new double[o.n > 2 ? o.n - 1 : 1])
{
  this->operator = (o);
}

Segments& Segments::operator = (const Segments& o)
{
  if (this != &o) {
    if (o.n != this->n) {
      delete [] y; y = new double[o.n > 1 ? o.n : 1];
      delete [] ui; ui = new double[o.n > 1 ? o.n : 1];
      delete [] a; a = new double[o.n > 2 ? o.n - 1 : 1];
      this->n = o.n;
    }
    for (unsigned int ii = n; ii--; ) {
      this->y[ii] = o.y[ii];
      this->ui[ii] = o.ui[ii];
    }
    for (unsigned int ii = n-1; ii--; ) {
      this->a[ii] = o.a[ii];
    }
  }
  return *this;
}


Segments::~Segments()
{
  delete [] y;
  delete [] ui;
  delete [] a;
}

double Segments::operator () (const double x) const
{
  if (x > ui[n-1]) return y[n-1];
  for (int ii = n - 1; ii--; ) {
    if (x > ui[ii]) return y[ii] + a[ii]*(x - ui[ii]);
  }
  return y[0];
}

std::ostream& Segments::print(std::ostream& os) const
{
  os << "Segments(n=" << n << "y:" << y[0];
  for (unsigned int ii = 1; ii < n; ii++) {
    os << "+" << a[ii-1] << "(x - " << ui[ii] << ") " << y[ii];
  }
  return os << ")";
}

DUECA_NS_END

namespace flexistick {
  static double dum[] = {0.0, 1.0};
  
  SegmentsConverter::SegmentsConverter() :
    FlexiLink(),
    converter(2, dum, dum)
  {
    //
  }

  void SegmentsConverter::defineSegments(const std::vector<double>& coef)
  {
    double yi[coef.size()/2];
    double ui[coef.size()/2];
    for (unsigned ii = 0; ii < coef.size()/2; ii++) {
      ui[ii] = coef[2*ii];
      yi[ii] = coef[2*ii+1];
    }
    converter = dueca::Segments(coef.size()/2, yi, ui);
  }

  SegmentsConverter::~SegmentsConverter()
  {
    //
  }

  void SegmentsConverter::propagate(int& v, unsigned idx)
  {
    float value = v;
    PDEB("Segments int conversion, " << v << " to " << value);    
    this->propagate(value, idx);
  }  

  void SegmentsConverter::propagate(float& v, unsigned idx)
  {
    float value = converter(v);
    PDEB("Segments conversion, " << v << " to " << value);
    for (t_flexilink::iterator ll = receivers.begin();
	 ll != receivers.end(); ll++) {
      ll->second->propagate(value, ll->first);
    }
  }  
}
