/* ------------------------------------------------------------------   */
/*      item            : TouchDevice.cxx
        made by         : Rene' van Paassen
        date            : 170920
	category        : body file 
        description     : 
	changes         : 170920 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
*/

#define TouchDevice_cxx
#include "TouchDevice.hxx"
#include <dueca/debug.h>

namespace flexistick {
  
  class devicenotvalid: public std::exception
  {
  public:
    const char* what() const throw()
    { return "Device not a valid touche device"; }
  };
  
  TouchDevice::TouchDevice(int dev, const std::string& name) :
    HIDTouch(name),
    touchid(0)
  {
    touchid = SDL_GetTouchDevice(dev);

#if 0
    SDL_TouchDeviceType dtype = SDL_GetTouchDeviceType(touchid);
    switch (dtype) {
    case SDL_TOUCH_DEVICE_INVALID:
      { throw(devicenotvalid()); }
      break;
    case SDL_TOUCH_DEVICE_DIRECT:
      I_MOD("Touch device number " << dev << " direct screen");
      break;
    case SDL_TOUCH_DEVICE_INDIRECT_ABSOLUTE:
      I_MOD("Touch device number " << dev << " absolute pad");
      break;
    case SDL_TOUCH_DEVICE_INDIRECT_RELATIVE:
      I_MOD("Touch device number " << dev << " relative pad");
    }
#endif
    //SDL_EventState(SDL_ENABLE);
    unsigned nfinger = SDL_GetNumTouchFingers(touchid);
    I_MOD("Detection of " << nfinger << " fingers");

    for (unsigned ii = 0; ii < nfinger; ii++) {
      fingers.emplace_back(ii);
    }
  }

  TouchDevice::~TouchDevice()
  {
    // 
  }
}
 
  
