/* ------------------------------------------------------------------   */
/*      item            : HIDTouch.hxx
        made by         : Rene van Paassen
        date            : 191111
	category        : header file 
        description     : 
	changes         : 170722 first version
        language        : C++
*/

#ifndef WacomTouch_hxx
#define WacomTouch_hxx

#ifdef WACOMTOUCH
#include <WacomMultiTouch/WacomMultiTouch.h>

#include "HIDBase.hxx"
#include <list>
#include <vector>
#include <string>

#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

namespace flexistick
{
  
  /** struct that gives the necessary data to read one SDL
      touch device. */
  struct WacomTouch: public HIDBase
  {
    /** Define type for storing axis reading results */
    typedef HIDInputValue<float>   WacomInputTouch;
    typedef HIDInputValue<bool>    WacomTouchActive;

    /** Offset */
    float offsetX, offsetY;

    /** Scaling */
    float scaleX, scaleY;

    struct FingerSet {
      boost::intrusive_ptr<WacomTouchActive> state;
      boost::intrusive_ptr<WacomInputTouch> x;
      boost::intrusive_ptr<WacomInputTouch> y;
      //unsigned id;
      
      FingerSet() :
	state(new WacomTouchActive()),
	x(new WacomInputTouch()),
	y(new WacomInputTouch())//,
	//id(id)
      { }
    };
    
    /** List type for finger inputs (2d, so pairwise) */
    typedef std::vector<FingerSet>    wacomtouch_t;

    /** finger devices */
    wacomtouch_t fingers;
    
  public:
    /** Constructor */
    WacomTouch(const std::string& name, unsigned nfingers);

    /** Destructor */
    ~WacomTouch();

    /** Data from the finger */
    int newData(const WacomMTFingerCollection* ev);
    
    /** Run any collected changes through the conversion system */
    void propagate();

    /** find the X value of a finger */
    boost::intrusive_ptr<FlexiLink> getFingerX(unsigned idx);

    /** find the X value of a finger */
    boost::intrusive_ptr<FlexiLink> getFingerY(unsigned idx);

    /** find the Pressure value of a finger */
    boost::intrusive_ptr<FlexiLink> getFingerStatus(unsigned idx);
  }; 
}

#endif
#endif
