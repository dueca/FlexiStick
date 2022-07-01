/* ------------------------------------------------------------------   */
/*      item            : HIDTouch.hxx
        made by         : Rene van Paassen
        date            : 191111
	category        : header file 
        description     : 
	changes         : 170722 first version
        language        : C++
*/

#ifndef HIDTouch_hxx
#define HIDTouch_hxx

#include <SDL.h>
#include <SDL_joystick.h>
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
  struct HIDTouch: public HIDBase
  {
    /** Define type for storing axis reading results */
    typedef HIDInputValue<float>   HIDInputTouch;
    typedef HIDInputValue<bool>   HIDTouchActive;

    /** relative or absolute device */
    bool relative;
    
    struct FingerSet {
      boost::intrusive_ptr<HIDTouchActive> state;
      boost::intrusive_ptr<HIDInputTouch> x;
      boost::intrusive_ptr<HIDInputTouch> y;
      boost::intrusive_ptr<HIDInputTouch> pressure;
      // boost::intrusive_ptr<HIDInputTouchID> id;
      unsigned id;
      
      FingerSet(unsigned id) :
	state(new HIDTouchActive()),
	x(new HIDInputTouch()),
	y(new HIDInputTouch()),
	pressure(new HIDInputTouch()),
	id(id)
      { }
    };
    
    /** List type for finger inputs (2d, so pairwise) */
    typedef std::vector<FingerSet>    touch_t;

    /** finger devices */
    touch_t fingers;
    
  public:
    /** Constructor */
    HIDTouch(const std::string& name);

    /** Destructor */
    ~HIDTouch();

    /** Data from the finger */
    void newData(const SDL_TouchFingerEvent& ev);
    
    /** Run any collected changes through the conversion system */
    void propagate();

    /** find the X value of a finger */
    boost::intrusive_ptr<FlexiLink> getFingerX(unsigned idx);

    /** find the X value of a finger */
    boost::intrusive_ptr<FlexiLink> getFingerY(unsigned idx);

    /** find the Pressure value of a finger */
    boost::intrusive_ptr<FlexiLink> getFingerPressure(unsigned idx);
    
    /** find the Pressure value of a finger */
    boost::intrusive_ptr<FlexiLink> getFingerStatus(unsigned idx);
  }; 
}
#endif
