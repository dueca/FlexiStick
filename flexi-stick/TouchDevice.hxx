/* ------------------------------------------------------------------   */
/*      item            : TouchDevice.hxx
        made by         : Rene van Paassen
        date            : 170722
	category        : header file 
        description     : 
	changes         : 170722 first version
        language        : C++
*/

#ifndef TouchDevice_hxx
#define TouchDevice_hxx

#include <SDL.h>
#include <SDL_joystick.h>
#include "HIDTouch.hxx"
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
  struct TouchDevice: public HIDTouch
  {
    /** joystick device */
    SDL_TouchID touchid;

  public:
    /** Constructor */
    TouchDevice(int dev, const std::string& name);

    /** Destructor */
    ~TouchDevice();
  };

}
#endif
