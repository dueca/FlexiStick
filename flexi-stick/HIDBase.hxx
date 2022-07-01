/* ------------------------------------------------------------------   */
/*      item            : HIDBase.hxx
        made by         : Rene van Paassen
        date            : 170920
	category        : header file 
        description     : 
	changes         : 170920 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
*/

#ifndef HIDBase_hxx
#define HIDBase_hxx

#include <vector>
#include <string>
#include "FlexiLink.hxx"
#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

#include <SDL.h>
#include <SDL_joystick.h>

namespace flexistick {
  
  /** generic base class for devices that read data */
  struct HIDBase
  {
    /** refcounter */
    unsigned intrusive_refcount;

    /** name of this device */
    std::string name;

    /** Generic type of value keeper */
    template<typename D>
    struct HIDInputValue: public FlexiLink {
      /** latest value */
      D           value;
      /** update value */
      inline void update(D newval) {
        changed = true; value = newval;
        PDEB("HID input new value " << newval);
      }
      /** collect values */
      void propel() {
        if (changed) {
	  PDEB("Propel start, value " << value); 
          for (FlexiLink::t_flexilink::iterator ll = this->receivers.begin();
               ll != this->receivers.end(); ll++) {
            ll->second->propagate(value, ll->first);
          }
          changed = false;
        }
      }
      /** constructor */
      HIDInputValue() : FlexiLink(), value(0) {  }
    };    
    
    /** Constructor */
    HIDBase(const std::string& name);
    
    /** Destructor */
    ~HIDBase();

    /** Find name of device */
    const std::string& getName() const { return name; }    
  };

  class jsinputnotpresent: public std::exception
  {
  public:
    const char* what() const throw();
  };
}
#endif
