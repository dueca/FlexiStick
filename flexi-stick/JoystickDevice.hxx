/* ------------------------------------------------------------------   */
/*      item            : JoystickDevice.hxx
        made by         : Rene van Paassen
        date            : 170722
	category        : header file 
        description     : 
	changes         : 170722 first version
        language        : C++
*/

#ifndef JoystickDevice_hxx
#define JoystickDevice_hxx

#include <SDL.h>
#include <SDL_joystick.h>
#include "HIDStick.hxx"
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
      joystick device. */
  struct JoystickDevice: public HIDStick
  {
    /** joystick device */
    SDL_Joystick* joystick;

  public:
    /** Constructor */
    JoystickDevice(SDL_JoystickID dev, const std::string& name);

    /** Destructor */
    ~JoystickDevice();
  };

}
#endif
