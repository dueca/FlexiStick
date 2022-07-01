/* ------------------------------------------------------------------   */
/*      item            : Counter.hxx
        made by         : Rene van Paassen
        date            : 170721
	category        : header file
        description     :
	changes         : 170721 first version
        language        : C++
*/

#ifndef Counter_hxx
#define Counter_hxx

#include "FlexiLink.hxx"

namespace flexistick
{
  /** Counter, creates an integer value from logicals */
  struct Counter: public FlexiLink
  {
    /** Minimum value */
    int minval;

    /** Default / reset value */
    int defval;

    /** Maximum value */
    int maxval;

    /** Current value (state) */
    int value;

    /** Current upcount (state) */
    bool cup;

    /** Current downcount (state) */
    bool cdown;

  public:
    /** Constructor */
    Counter();

    /** set values */
    void defineCounter(int cmin, int cmax, int ccenter);

    /** Destructor */
    ~Counter();

    /** Accept input & produce result */
    void propagate(bool& v, unsigned idx);

    /** Have state */
    bool haveState() const { return true; }

    /** Code state into json */
    void takeSnapshot(dueca::smartstring::json_string_writer& writer);

    /** Load state from json */
    void loadSnapshot(JValue& doc);
  };

}
#endif
