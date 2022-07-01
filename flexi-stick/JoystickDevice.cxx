/* ------------------------------------------------------------------   */
/*      item            : JoystickDevice.cxx
        made by         : Rene' van Paassen
        date            : 170722
	category        : body file 
        description     : 
	changes         : 170722 first version
        language        : C++
*/

#define JoystickDevice_cxx

#include "JoystickDevice.hxx"
#include <utility>
#define I_MOD
#include <debug.h>

#ifndef DEB
#define DEB(A) std::cerr << A << std::endl;
#endif


namespace flexistick {
  
  JoystickDevice::JoystickDevice(SDL_JoystickID dev, const std::string& name) :
    HIDStick(name),
    joystick(NULL)
  {
#if FSTEST
    std::string tname = "Simulated SDL joystick";
    unsigned nax = 3;
    unsigned nhat = 1;
    unsigned nbutton = 1;
#else
    joystick = SDL_JoystickOpen(dev);
    std::string tname = SDL_JoystickName(joystick);
    SDL_JoystickEventState(SDL_ENABLE);
    unsigned nax = SDL_JoystickNumAxes(joystick);
    unsigned nhat = SDL_JoystickNumHats(joystick);
    unsigned nbutton = SDL_JoystickNumButtons(joystick);
#endif
    hats.resize(nhat);
    for(int ii = nhat; ii--; ) hats[ii].reset(new HIDInputHat());
    axes.resize(nax);
    for(int ii = nax; ii--; ) axes[ii].reset(new HIDInputAxis());
    buttons.resize(nbutton);
    for(int ii = nbutton; ii--; ) buttons[ii].reset(new HIDInputButton());
    I_MOD("Opened joystick " << name << " type " << tname);
  }

    JoystickDevice::~JoystickDevice()
  {
    // 
  }

  
}


