/* ------------------------------------------------------------------   */
/*      item            : FlexiStick.cxx
        made by         : repa
        from template   : DuecaModuleTemplate.cxx
        template made by: Rene van Paassen
        date            : Mon Jul 17 18:34:36 2017
        category        : body file
        description     :
        changes         : Mon Jul 17 18:34:36 2017 first version
        template changes: 030401 RvP Added template creation comment
                          060512 RvP Modified token checking code
                          160511 RvP Some comments updated
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
*/


#define FlexiStick_cxx

// include the definition of the module class
#include "FlexiStick.hxx"

// include the debug writing header, by default, write warning and
// error messages
#define I_MOD
#define W_MOD
#define E_MOD
#include <debug.h>

#ifndef DEB
#define DEB(A) std::cerr << A << std::endl;
#endif

// include additional files needed for your calculation here
#include <dueca/Ticker.hxx>
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <dueca/DataClassRegistry.hxx>
#include <dueca/CommObjectMemberAccess.hxx>
#include <iomanip>
#include "WeightedSum.hxx"

// the standard package for DUSIME, including template source
#define DO_INSTANTIATE
#include <dusime.h>

#ifdef WACOMTOUCH
#include <Carbon/Carbon.h>
#endif

using namespace flexistick;

// class/module name
const char* const FlexiStick::classname = "flexi-stick";

// Parameters to be inserted
const ParameterTable* FlexiStick::getMyParameterTable()
{
  static const ParameterTable parameter_table[] = {
    { "set-timing",
      new MemberCall<_ThisModule_,TimeSpec>
        (&_ThisModule_::setTimeSpec), set_timing_description },

    { "check-timing",
      new MemberCall<_ThisModule_,vector<int> >
      (&_ThisModule_::checkTiming), check_timing_description },

    { "add-device", new MemberCall<_ThisModule_,std::string>
      (&_ThisModule_::addDevice),
      "Supply a symbolic name and number for an SDL joystick device. Example\n"
      "\"mydev:0\". Multiple devices may be added, and referenced by their\n"
      "name later when writing the data into channels or converting data.\n"
      "This creates access to the axes (a), buttons (b), hats (h) or hat\n"
      "values (u, d, l, r, for up, down, left, right respectively). For\n"
      "example \"mydev.a[0]\" will link to the device's first axis" },

#if defined(DataRecorder_hxx)
    { "enable-record-replay", new VarProbe<_ThisModule_,bool>
      (&_ThisModule_::enable_record_replay),
      "When true, enable recording in Advance mode, and replay of the given\n"
      "recording. Note that replay will only work on the exact same set of\n"
      "channels being written\n" },
#endif

    { "add-touch", new MemberCall<_ThisModule_,std::string>
      (&_ThisModule_::addTouch),
      "Supply a symbolic name and number for an SDL touch device. Example\n"
      "\"mydev:0\". Multiple devices may be added, and referenced by their\n"
      "name later when writing the data into channels or converting data.\n"
      "This create access to the x, y and pressure values (x, y, p) and touch\n"
      "state (s) for all fingers processed by the touch device." },

#ifdef WACOMTOUCH
    { "add-wacom-touch", new MemberCall<_ThisModule_,std::string>
      (&_ThisModule_::addWacom),
      "Supply a symbolic name and number of fingers for a Wacom touch device.\n"
      "Example \"mydev:2\". Multiple devices may be added, and are processed\n"
      "in the order given by the WACOM driver. These devices are known by\n"
      "name later when writing the data into channels or converting data.\n"
      "The x, y and s (state) values may be accessed" },
#endif

    { "add-channel", new MemberCall<_ThisModule_,std::vector<std::string> >
      (&_ThisModule_::addChannel),
      "Channel (entry) to write; specify short name, channel name, data\n"
      "class and entry label. Optionally specify event writing by adding\n"
      "\"event\" as last argument" },

    { "add-link", new MemberCall<_ThisModule_,std::vector<std::string> >
      (&_ThisModule_::addLink),
      "Create a link between a joystick button (b), axis (a) or hat (h), or a\n"
      "scaled or converted value and a written channel variable or a touch.\n"
      "input. Give the written variable by means of a channel name, dot (.)\n"
      "and the variable name, optionally with index. Example \"chan1.val[2]\"\n"
      "As second argument specify the input from a joystick directly, e.g.,\n"
      "\"jsname.b[1]\"or use the name of a virtual input device like a\n"
      "counter, or scaler/converter." },

    { "create-counter",
      new MemberCall<_ThisModule_, std::vector<std::string> >
      (&_ThisModule_::createCounter),
      "create a counting device. Supply 3 or 4 strings; symbolic name\n"
      "for the counter, down counting input (see above), up counting input,\n"
      "and optionally the centering input." },

    { "counter-params",
      new MemberCall<_ThisModule_, std::vector<int> >
      (&_ThisModule_::defineCounter),
      "Specification of counter, [minimum count], maximum count,\n"
      "[center value]" },

    { "create-poly",
      new MemberCall<_ThisModule_, std::vector<std::string> >
      (&_ThisModule_::createPoly),
      "create a scaling polynomial. Supply symbolic name and input name" },

    { "poly-params",
      new MemberCall<_ThisModule_, std::vector<double> >
      (&_ThisModule_::definePoly),
      "set the parameters for the scaling polynomial, coefficient as\n"
      "a0 + a1 x + a2 x^2 etc." },

    { "create-steps",
      new MemberCall<_ThisModule_, std::vector<std::string> >
      (&_ThisModule_::createSteps),
      "create a stepping converter. Supply symbolic name and input name" },

    { "steps-params",
      new MemberCall<_ThisModule_, std::vector<double> >
      (&_ThisModule_::defineSteps),
      "provide 'choice' steps for calibration, give input, output pairs\n"
      "u0, y0, u1, y1 etc. Finds u closest to input value, returns\n"
      "corresponding output value" },

    { "create-weighted",
      new MemberCall<_ThisModule_, std::vector<std::string> >
      (&_ThisModule_::createWeightedSum),
      "create a weighted sum converter. Supply symbolic name, and then\n"
      "specify one or more inputs. Optionally supply a gain factor with\n"
      "the input, like \"1.2*val1\"" },

    { "add-virtual",
      new MemberCall<_ThisModule_, std::vector<std::string> >
      (&_ThisModule_::addVirtual),
      "create a virtual stick device. Supply the name of the device\n"
      "and optionally a file name for the gui template" },

    { "add-virtual-slider",
      new MemberCall<_ThisModule_, std::vector<int> >
      (&_ThisModule_::addVirtualSlider),
      "add a virtual slider. Supply x0, y0 for slider start coordinates,\n"
      "x1, y1 for slider end coordinates, and r for the slider radius\n"
      "optionally add '1' to obtain sticky (throttle?) behaviour" },

    { "add-virtual-slider-2d",
      new MemberCall<_ThisModule_, std::vector<int> >
      (&_ThisModule_::addVirtualSlider2D),
      "add a virtual slider. Supply x0, y0 for slider start coordinates,\n"
      "x1, y1 for slider end coordinates, and r for the slider radius\n"
      "optionally add '1' to obtain sticky behaviour" },

    { "add-virtual-button",
      new MemberCall<_ThisModule_, std::vector<int> >
      (&_ThisModule_::addVirtualButton),
      "add a virtual button. Supply x0, y0 for button center, and r for the\n"
      "button radius, optionally add '1' to obtain sticky behaviour" },

    { "add-virtual-hat",
      new MemberCall<_ThisModule_, std::vector<int> >
      (&_ThisModule_::addVirtualHat),
      "add a virtual button. Supply x0, y0 for hat center, and r for the\n"
      "hat radius, optionally add '1' to obtain sticky behaviour" },

    /* You can extend this table with labels and MemberCall or
       VarProbe pointers to perform calls or insert values into your
       class objects. Please also add a description (c-style string).

       Note that for efficiency, set_timing_description and
       check_timing_description are pointers to pre-defined strings,
       you can simply enter the descriptive strings in the table. */

    /* The table is closed off with NULL pointers for the variable
       name and MemberCall/VarProbe object. The description is used to
       give an overall description of the module. */
    { NULL, NULL,
      "Generic flexible joystick device reading module."} };

  return parameter_table;
}

// constructor
FlexiStick::FlexiStick(Entity* e, const char* part, const
                   PrioritySpec& ps) :
  /* The following line initialises the SimulationModule base class.
     You always pass the pointer to the entity, give the classname and the
     part arguments. */
  SimulationModule(e, classname, part, NULL, 0),

  // initialize the data you need in your simulation or process
  deltat(Ticker::single()->getDT()),
  need_sdljoystick_init(true),
  need_sdlevent_init(true),
#ifdef WACOMTOUCH
  need_wacomtouch_init(true),
#endif
  // create a clock, if you need time based triggering
  // instead of triggering on the incoming channels
  myclock(),
#if defined(DataRecorder_hxx)
  enable_record_replay(false),
#endif

  // a callback object, pointing to the main calculation function
  cb1(this, &_ThisModule_::doCalculation),
  // the module's main activity
  do_calc(getId(), "read stick inputs", &cb1, ps)
{
  // connect the triggers for simulation
  do_calc.setTrigger(myclock);
}

bool FlexiStick::complete()
{
  bool res = true;
  /* All your parameters have been set. You may do extended
     initialisation here. Return false if something is wrong. */
  for (t_gui_device::iterator ii = gui_device.begin();
       ii != gui_device.end(); ii++) {
    res  = res && (*ii)->init();
  }

#ifdef WACOMTOUCH
  if (wcandidates.size()) {
    res = res && wacomTouchInit();
  }
#endif

  return res;
}

// destructor
FlexiStick::~FlexiStick()
{
  if (!need_sdljoystick_init) {
    SDL_Quit();
  }
}

// as an example, the setTimeSpec function
bool FlexiStick::setTimeSpec(const TimeSpec& ts)
{
  // a time span of 0 is not acceptable
  if (ts.getValiditySpan() == 0) return false;

  // or do this with the clock if you have it (don't do both!)
  myclock.changePeriodAndOffset(ts);

  // do whatever else you need to process this in your model
  deltat = ts.getDtInSeconds();

  // return true if everything is acceptable
  return true;
}

// the checkTiming function installs a check on the activity/activities
// of the module
bool FlexiStick::checkTiming(const vector<int>& i)
{
  if (i.size() == 3) {
    new TimingCheck(do_calc, i[0], i[1], i[2]);
  }
  else if (i.size() == 2) {
    new TimingCheck(do_calc, i[0], i[1]);
  }
  else {
    return false;
  }
  return true;
}

// tell DUECA you are prepared
bool FlexiStick::isPrepared()
{
  bool res = true;

  for (t_channelaccess::iterator cc = w_tokens.begin();
       cc != w_tokens.end(); cc++) {
    if (!cc->second->isPrepared()) {
      W_MOD("Channel " << cc->first << " not prepared");
      res = false;
    }
  }

  // return result of checks
  return res;
}

// start the module
void FlexiStick::startModule(const TimeSpec &time)
{
  do_calc.switchOn(time);
}

// stop the module
void FlexiStick::stopModule(const TimeSpec &time)
{
  do_calc.switchOff(time);
}

inline bool FlexiStick::_SDL_PollEvent(SDL_Event* ev, const DataTimeSpec& ts)
{
#ifdef FSTEST

  // only for injecting test inputs, cycles through all these
  // simulated events
  static SDL_JoyAxisEvent aev[] = {
    { SDL_JOYAXISMOTION, 4, 0, 0, 0, 0, 0,   000 },
    { SDL_JOYAXISMOTION, 5, 0, 1, 0, 0, 0,   000 },
    { SDL_JOYAXISMOTION, 2, 0, 2, 0, 0, 0,   000 },
    { SDL_JOYAXISMOTION, 1, 0, 0, 0, 0, 0,  2000 },
    { SDL_JOYAXISMOTION, 2, 0, 1, 0, 0, 0, -2000 },
    { SDL_JOYAXISMOTION, 3, 0, 2, 0, 0, 0, -2000 }
  };
  static SDL_JoyHatEvent hev[] = {
    { SDL_JOYHATMOTION, 1, 0, 0, SDL_HAT_CENTERED },
    { SDL_JOYHATMOTION, 2, 0, 0, SDL_HAT_UP },
    { SDL_JOYHATMOTION, 3, 0, 0, SDL_HAT_RIGHTUP },
    { SDL_JOYHATMOTION, 4, 0, 0, SDL_HAT_RIGHT },
    { SDL_JOYHATMOTION, 5, 0, 0, SDL_HAT_RIGHTDOWN },
    { SDL_JOYHATMOTION, 6, 0, 0, SDL_HAT_DOWN },
    { SDL_JOYHATMOTION, 7, 0, 0, SDL_HAT_LEFTDOWN },
    { SDL_JOYHATMOTION, 8, 0, 0, SDL_HAT_LEFT },
    { SDL_JOYHATMOTION, 9, 0, 0, SDL_HAT_LEFTUP }
  };
  static SDL_JoyButtonEvent bev[] = {
    { SDL_JOYBUTTONUP, 1, 0, 0, SDL_RELEASED },
    { SDL_JOYBUTTONDOWN, 2, 0, 0, SDL_PRESSED }
  };
  static unsigned idx = 0;
  static unsigned tosend = 4;

  switch(--tosend) {
  case 0:
    tosend = 4;
    idx++;
    return false;
  case 1:
    ev->jaxis = aev[idx % (sizeof(aev)/sizeof(SDL_JoyAxisEvent))];
    return true;
  case 2:
    ev->jhat = hev[idx % (sizeof(hev)/sizeof(SDL_JoyHatEvent))];
    return true;
  case 3:
    ev->jbutton = bev[idx % (sizeof(bev)/sizeof(SDL_JoyButtonEvent))];
    return true;
  default:
    assert(0);
    return false;
  }
#else
  // first get events from all gui's emulating joystick buttons, hats and axes
  while (gui_current != gui_device.end()) {
    if ( (*gui_current)->pollEvent(ev) ) {
      return true;
    }
    else {
      gui_current++;
    }
  }

  return SDL_PollEvent(ev);
#endif
}
#ifdef WACOMTOUCH

// from https://vgable.com/blog/2008/04/23/printing-a-fourcharcode/
char *fourByteCodeStr(uint32_t code, char* str)
{
  sprintf(str, "%c%c%c%c",
          (code >> 24), (code>>16)&0xff, (code>>8)&0xff, code&0xff);
  return str;
}

static void processWacomTouchEvents(EventTimeout timeout)
{
  // reference to event and target
  EventRef ev;
  EventTargetRef target = GetEventDispatcherTarget();

#ifdef KNOWEVENTTYPES
  static const unsigned ntypes = 1;
  static EventTypeSpec evlist[ntypes] = {
    { FOUR_CHAR_CODE("????"), FOUR_CHAR_CODE("xxxx")  }
  };
#else
  static const unsigned ntypes = 0;
  static EventTypeSpec *evlist = NULL;
  char str[5];
#endif

  // pump the event loop,
  while (ReceiveNextEvent(ntypes, evlist, timeout, true, &ev) == 0) {
    SendEventToEventTarget(ev, target);
#ifndef KNOWEVENTTYPES
    std::cout << "Got an event of class '"
              << fourByteCodeStr(GetEventClass(ev), str)
              << "' kind '"
              << fourByteCodeStr(GetEventKind(ev), str) << "'" << std::endl;
#endif
    ReleaseEvent(ev);
  }
  std::cout << "Event loop exit" << std::endl;
}
#endif


// this routine contains the main simulation process of your module. You
// should read the input channels here, and calculate and write the
// appropriate output
void FlexiStick::doCalculation(const TimeSpec& ts)
{
  // make sure the current state is update
  SimulationState state = getAndCheckState(ts);
  DataTimeSpec tspoint(ts.getValidityStart(), ts.getValidityStart());

  // set gui device iterator to start
  gui_current = gui_device.begin();

  // process all incoming events, and push these into the list with
  SDL_Event event;
  while (_SDL_PollEvent(&event, tspoint)) {
    switch (event.type) {
    case SDL_QUIT:
      W_MOD("got SDL quit event");
      break;
    case SDL_JOYAXISMOTION: {
      t_devices::iterator dev = devices.find(event.jaxis.which);
      if (dev == devices.end() ||
          dev->second->axes.size() <= event.jaxis.axis) {
        auto key = std::make_pair(uint32_t(event.jaxis.which),
                                  uint32_t(event.jaxis.axis));
        if (!uc_axes.count(key)) {
          W_MOD("SDL joystick unconfigured device/axis " <<
                key.first << '/' << key.second);
          uc_axes.insert(key);
        }
      }
      else {
        dev->second->axes[event.jaxis.axis]->
          update(float(event.jaxis.value)/float(0x8000));
      }
    }
      break;
    case SDL_JOYHATMOTION: {
      t_devices::iterator dev = devices.find(event.jhat.which);
      if (dev == devices.end() ||
          dev->second->hats.size() <= event.jhat.hat) {
        auto key = std::make_pair(uint32_t(event.jhat.which),
                                  uint32_t(event.jhat.hat));
        if (!uc_hats.count(key)) {
          W_MOD("SDL joystick unconfigured device/hat " <<
                key.first << '/' << key.second);
          uc_hats.insert(key);
        }
      }
      else {
        dev->second->hats[event.jhat.hat]->update(event.jhat.value);
      }
    }
      break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP: {
      t_devices::iterator dev = devices.find(event.jbutton.which);
      if (dev == devices.end() ||
          dev->second->buttons.size() <= event.jbutton.button) {
        auto key = std::make_pair(uint32_t(event.jbutton.which),
                                  uint32_t(event.jbutton.button));
        if (!uc_buttons.count(key)) {
          W_MOD("SDL joystick unconfigured device/button " <<
                key.first << '/' << key.second);
          uc_buttons.insert(key);
        }
      }
      else {
        dev->second->buttons[event.jbutton.button]->update
          (event.jbutton.state == SDL_PRESSED);
      }
      break;
    }
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
    case SDL_FINGERUP: {
      t_touchdevices::iterator dev = touches.find(event.tfinger.touchId);
      if (dev == touches.end() ||
          int(dev->second->fingers.size()) <= event.tfinger.fingerId) {
        I_MOD("SDL touch motion unconfigured device/finger " <<
              event.tfinger.touchId << '/' << event.tfinger.fingerId);
      }
      else {
        dev->second->newData(event.tfinger);
      }
    }
      break;

    default:
      W_MOD("Unknown event of type " << hex << event.type << dec);
    }
  }

#ifdef WACOMTOUCH
  processWacomTouchEvents(0);
#endif

  // push all the data through scalers etc to output
  for (t_devices::iterator dev = devices.begin();
       dev != devices.end(); dev++) {
    dev->second->propagate();
  }

#if 0
  // run time-based updates (repeat keys) through the converters/scalers
  for (t_converters::iterator con = converters.begin();
       con != converters.end(); con++) {
    con->second->timeUpdate(ts);
  }
#endif

  // create write actions for all stream channels, and write actions
  // for all event channels with changed data
  for (t_channelaccess::iterator acc = w_tokens.begin();
       acc != w_tokens.end(); acc++) {
    acc->second->write(ts, state);
  }
}

bool FlexiStick::sdlJoyInit()
{
  sdlTouchInit();
  if (need_sdljoystick_init) {
    if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
      SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) {
      E_MOD("Unable to initialize SDL: " << SDL_GetError());
      return false;
    }
    sdl_njoy = SDL_NumJoysticks();
    virtual_joyid = sdl_njoy;
    need_sdljoystick_init = false;
  }
  return true;
}

bool FlexiStick::sdlTouchInit()
{
  if (need_sdlevent_init) {
    SDL_Init(0);
    if (SDL_WasInit(SDL_INIT_EVENTS) != 0) {
      SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
    if (SDL_InitSubSystem(SDL_INIT_EVENTS) != 0) {
      E_MOD("Unable to initialize SDL touch: " << SDL_GetError());
      return false;
    }
    sdl_ntouch = SDL_GetNumTouchDevices();
    need_sdlevent_init = false;
  }
  return true;
}

#ifdef WACOMTOUCH

bool FlexiStick::processDeviceInfo(const WacomMTCapability& devinfo)
{
  if (wcandidates.size() == 0) {
    W_MOD("Additional device info received, Version=" << devinfo.Version <<
          " DeviceID=" << devinfo.DeviceID << " Type=" <<
          (devinfo.Type == WMTDeviceTypeOpaque ? "Opaque" : "Integrated") <<
          " FingerMax=" << devinfo.FingerMax);
    return false;
  }
  if (devinfo.FingerMax == 0) {
    W_MOD("Fingerless device, Version=" << devinfo.Version <<
          " DeviceID=" << devinfo.DeviceID);
    return false;
  }
  if (wdevices.find(devinfo.DeviceID) != wdevices.end()) {
    W_MOD("Device ID already configured Version=" << devinfo.Version <<
          " DeviceID=" << devinfo.DeviceID);
    return false;
  }

  if (devinfo.FingerMax != wcandidates.front()->fingers.size()) {
    W_MOD("Number of fingers mismatch DeviceID=" << devinfo.DeviceID);
    return false;
  }
  wcandidates.front()->scaleX = 1.0f / devinfo.LogicalWidth;
  wcandidates.front()->scaleY = 1.0f / devinfo.LogicalHeight;
  wcandidates.front()->offsetX = devinfo.LogicalOriginX;
  wcandidates.front()->offsetY = devinfo.LogicalOriginY;
  wdevices[devinfo.DeviceID] = wcandidates.front();

  // create a callback
  auto f_callback =
    +[](WacomMTFingerCollection *fingerPacket, void *self) {
    return reinterpret_cast<WacomTouch*>(self)->newData(fingerPacket);
  };
  // register as consumer, take all data, use WacomTouch as target
  WacomMTError res = WacomMTRegisterFingerReadCallback
    (devinfo.DeviceID, NULL, WMTProcessingModeNone, f_callback,
     wcandidates.front().get());

  wcandidates.erase(wcandidates.begin());
  return wdevices.size() == 1;
}

bool FlexiStick::wacomTouchInit()
{
  if (need_wacomtouch_init) {
    I_MOD("Init wacom touch");
    WacomMTError res = WacomMTInitialize(WACOM_MULTI_TOUCH_API_VERSION);
    if (res != WMTErrorSuccess) {
      E_MOD("Could not initialize Wacom touch, error " << res);
      return false;
    }

    auto f_callback =
      +[](WacomMTCapability deviceInfo, void* self) {
      reinterpret_cast<FlexiStick*>(self)->processDeviceInfo(deviceInfo);
    };
    res = WacomMTRegisterAttachCallback(f_callback, NULL);
    if (res != WMTErrorSuccess) {
      E_MOD("Could not register 1st wacom callbac " << res);
      return false;
    }
    I_MOD("Wacom init called, processing events 1");
    processWacomTouchEvents(0);
    I_MOD("Wacom init called, processing events 1");
    processWacomTouchEvents(0);
  }
  return true;
}
#endif

bool FlexiStick::addDevice(const std::string& idev)
{
  // argument is name:no, with no the SDL device number
  size_t idxc = idev.find(':');
  SDL_JoystickID dev;
  std::string name;
  if (idxc != string::npos) {
    try {
      dev = boost::lexical_cast<SDL_JoystickID>(idev.substr(idxc+1));
      name = idev.substr(0, idxc);
    }
    catch(const boost::bad_lexical_cast& e) {
      E_MOD("Cannot parse joystick number from " << idev);
      return false;
    }
  }
  if (!name.size()) {
    E_MOD("Device argument incorrect");
  }

  // init sdl joystick system
  if (!sdlJoyInit()) return false;

#ifdef FSTEST
  if (dev == 0) {
    W_MOD("Simulated joystick device 0");
    boost::intrusive_ptr<HIDStick> jdev(new JoystickDevice(dev, name));
    devices[dev] = jdev;
    return true;
  }
#endif

  t_devices::const_iterator dd = devices.find(dev);
  if (dd != devices.end()) {
    E_MOD("SDL joystick device " << dev << " already configured");
    return false;
  }

  if (dev >= sdl_njoy) {
    E_MOD("SDL joystick device " << dev << " not present, have " <<
          sdl_njoy << " devices");
    return false;
  }

  boost::intrusive_ptr<HIDStick> jdev(new JoystickDevice(dev, name));

  PDEB("SDL joystick, name " << name << " dev " << dev);

  devices[dev] = jdev;
  return true;
}

class devicenotfound: public std::exception
{
public:
  const char* what() const throw()
  { return "Device name not matched"; }
};

static SDL_TouchID matchTouch(const std::string& n)
{
#if 0
  // not possible
  int sdl_ntouch = SDL_GetNumTouchDevices();
  for (SDL_TouchID it = 0; it < sdl_ntouch; it++) {
    if (n == std::string(SDL_TouchNameForIndex(it))) {
      return it;
    }
  }
#endif
  throw(devicenotfound());
}

bool FlexiStick::addTouch(const std::string& idev)
{
  // argument is name:no, with no the SDL device number
  size_t idxc = idev.find(':');
  SDL_TouchID dev;
  std::string label;
  std::string name;
  if (idxc != string::npos && idxc != 0) {
    try {
      dev = boost::lexical_cast<SDL_TouchID>(idev.substr(idxc+1));
      name = idev.substr(0, idxc);
    }
    catch(const boost::bad_lexical_cast& e) {
      try {
        dev = matchTouch(idev.substr(idxc+1));
      }
      catch (const std::exception& e) {
        D_MOD("Cannot parse touch device from " << idev << ' ' << e.what());
        return false;
      }
    }
  }
  if (!name.size()) {
    E_MOD("Device argument incorrect");
  }

  // init sdl event system
  if (!sdlTouchInit()) return false;

  t_touchdevices::const_iterator dd = touches.find(dev);
  if (dd != touches.end()) {
    E_MOD("SDL touch device " << dev << " already configured");
    return false;
  }

  if (dev >= sdl_ntouch) {
    E_MOD("SDL touch device " << dev << " not present, have " <<
          sdl_ntouch << " devices");
    return false;
  }

  boost::intrusive_ptr<HIDTouch> jdev(new TouchDevice(dev, name));

  PDEB("SDL touch, name " << name << " dev " << dev);

  touches[dev] = jdev;
  return true;
}

#ifdef WACOMTOUCH
bool FlexiStick::addWacom(const std::string& idev)
{
  // match on number of fingers?
  // argument is name
  size_t idxc = idev.find(':');
  unsigned nfingers = 0;
  std::string label;
  std::string name;
  if (idxc != string::npos && idxc != 0) {
    try {
      nfingers = boost::lexical_cast<unsigned>(idev.substr(idxc+1));
      name = idev.substr(0, idxc);
    }
    catch(const boost::bad_lexical_cast& e) {
      D_MOD("Cannot parse wacom device from " << idev << ' ' << e.what());
      return false;
    }
  }
  if (!name.size()) {
    E_MOD("Device argument incorrect");
  }

  boost::intrusive_ptr<WacomTouch> jdev(new WacomTouch(name, nfingers));

  PDEB("WACOM touch, name " << name << " nfinger " << nfinger);

  wcandidates.push_back(jdev);
  return true;
}
#endif

bool FlexiStick::addChannel(const std::vector<std::string>& cpar)
{
  static const std::string iseventtype("event");

  if (!(cpar.size() == 3 || cpar.size() == 4 ||
        (cpar.size() == 5 && cpar[4] == iseventtype))) {
    E_MOD("Incorrect number of parameter for addChannel");
    return false;
  }
  bool evtype = (cpar.size() > 2) && (cpar[cpar.size() - 1] == iseventtype);
  if (w_tokens.find(cpar[0]) != w_tokens.end()) {
    E_MOD("Channel " << cpar[0] << " already configured");
  }

  w_tokens[cpar[0]] = boost::intrusive_ptr<ChannelAccess>
    (new ChannelAccess(getId(), cpar[1], cpar[2],
                       (cpar.size() > 4 || (cpar.size() == 4 && !evtype)) ?
                       cpar[3] : std::string(), evtype,
                       enable_record_replay ? getEntity() : std::string()));
  return true;
}

bool FlexiStick::addLink(const std::vector<std::string>& lpar)
{
  if (lpar.size() != 2) {
    E_MOD("Need two arguments to link");
    return false;
  }

  // find the channel to link by short name
  size_t didx = lpar[0].find('.');
  if (didx == std::string::npos) {
    E_MOD("Channel variable " << lpar[0] << " incorrectly specified");
    return false;
  }

  std::string cname = lpar[0].substr(0,didx);
  std::string vfull = lpar[0].substr(didx+1);

  t_channelaccess::iterator tt = w_tokens.find(cname);
  if (tt == w_tokens.end()) {
    E_MOD("Channel " << cname << " for link " << lpar[1] <<
          " cannot be found");
    return false;
  }

  // get variable name and index if applicable
  size_t idxlb = vfull.find('[');
  size_t idxrb = vfull.find(']');
  std::string vname = vfull;
  int idxv = -1;
  if (idxlb != string::npos && idxrb != string::npos) {
    try {
      idxv = boost::lexical_cast<int>
        (vfull.substr(idxlb+1, idxrb-idxlb-1));
      vname = vfull.substr(0,idxlb);
    }
    catch (const boost::bad_lexical_cast& e) {
      E_MOD("Cannot read index from " << vfull);
    }
  }
  PDEB("Found channel " << cname << " member " << vname << " index " << idxv);

  // check that the variable/index has not been linked already
  ChannelAccess::t_writelink::iterator ll = tt->second->writelink.begin();
  while (ll != tt->second->writelink.end() &&
         !((*ll)->mname == vname && (*ll)->idx == idxv)) ll++;
  if (ll != tt->second->writelink.end()) {
    E_MOD("Link to channel " << lpar[0] << " already in use");
    return false;
  }

  // create the new write link to this variable and index
  boost::intrusive_ptr<ChannelAccess::WriteLinkBase> newlink
    (tt->second->createWriteLink(vname, idxv));
  tt->second->writelink.push_back(newlink);

  // connect the link to the source
  return linkToSource(lpar[1], newlink, 0);
}

bool FlexiStick::linkToSource(const std::string& isource,
                              boost::intrusive_ptr<FlexiLink> target,
                              unsigned idxt)
{
  static const std::string hats("hudlr");
  static const std::string jtypes("abhudlr");
  static const std::string ttypes("xyps");
  static const std::string wtypes("xys");
  std::string source = isource;

  // if applicable analyse as joystick name; then re-compose
  size_t idxc = source.find('.');
  size_t idxlb = source.find('[');
  size_t idxrb = source.find(']');
  SDL_JoystickID jid = -1; unsigned idx = 0;
  DeviceType devtype = None;
  char jtype = 'x';
  std::string jname;

  // analyze whether it matches joystick/touch definitions
  if (idxc != std::string::npos &&
      idxlb != std::string::npos && idxrb != std::string::npos) {

    // search for the joystick by name
    jname = source.substr(0, idxc);

    if (devtype == None) {
      for (t_devices::iterator dd = devices.begin();
           dd != devices.end(); dd++) {
        if (dd->second->getName() == jname) {
          jid = dd->first;
          devtype = JoystickDeviceType;
          break;
        }
      }
    }
    if (devtype == None) {
      for (t_touchdevices::iterator dd = touches.begin();
           dd != touches.end(); dd++) {
        if (dd->second->getName() == jname) {
          jid = dd->first;
          devtype = HIDTouchDeviceType;
          break;
        }
      }
    }
#ifdef WACOMTOUCH
    if (devtype == None) {
      jid = 0;
      for (t_wcandidates::iterator dd = wcandidates.begin();
           dd != wcandidates.end(); dd++) {
        if ((*dd)->getName() == jname) {
          devtype = WacomTouchDeviceType;
          break;
        }
        jid++;
      }
    }
#endif

    // interpret the device index and joy input type, then check
    if (devtype != None) {

      try {

        // index of device and type of measurement
        idx = boost::lexical_cast<unsigned>
          (source.substr(idxlb+1, idxrb-idxlb-1));
        jtype = source[idxc+1];
        PDEB("type " << jtype << " idx " << idx);

        switch(devtype) {
        case JoystickDeviceType: {
          PDEB("Found joystick " << jname);

          // check all ok
          if (jtypes.find(jtype) == std::string::npos ||
              (jtype == 'b' && devices[jid]->buttons.size() <= idx) ||
              (jtype == 'a' && devices[jid]->axes.size() <= idx) ||
              (hats.find(jtype) != std::string::npos &&
               devices[jid]->hats.size() <= idx)) {
            E_MOD("Cannot find joystick input " << source);
            return false;
          }
        }
          break;

        case HIDTouchDeviceType: {
          PDEB("Found touch " << jname);
          if (ttypes.find(jtype) == std::string::npos ||
              touches[jid]->fingers.size() <= idx) {
            E_MOD("Cannot find touch finger " << source);
            return false;
          }
        }
          break;

#ifdef WACOMTOUCH
        case WacomTouchDeviceType: {
          PDEB("Found wacom touch " << jname);
          if (wtypes.find(jtype) == std::string::npos ||
              wcandidates[jid]->fingers.size() <= idx) {
            E_MOD("Cannot find touch wacom finger " << source);
            return false;
          }
        }
          break;
#endif

        default:
          assert(0);
        }
      }
      catch(const boost::bad_lexical_cast& e) {
        E_MOD("Cannot parse joystick/touch definition " << source);
        return false;
      }

      // re-construct name, removes possible blanks in definition
      source = jname + std::string(".") + jtype + std::string("[") +
        boost::lexical_cast<std::string>(idx) + std::string("]");

      // now check for presence, and add if not yet there
      if (sources.find(source) == sources.end()) {

        PDEB("Source '" << source << "' not yet known");

        switch(devtype) {
        case JoystickDeviceType: {

          // JIT add joystick input as a source
          if (hats.find(jtype) != string::npos) {
            std::string hname = jname + string(".h[") +
              boost::lexical_cast<std::string>(idx) + std::string("]");
            sources[hname] = devices[jid]->getHat(idx);
            if (jtype != 'h') {
              // create and add a hatfilter
              boost::intrusive_ptr<FlexiLink> hatfilter
                (new JoystickDevice::HatFilter(jtype));
              sources[hname]->link(hatfilter, 0);
              sources[source] = hatfilter;
            }
          }
          else if (jtype == 'a') {
            sources[source] = devices[jid]->getAxis(idx);
          }
          else if (jtype == 'b') {
            sources[source] = devices[jid]->getButton(idx);
          }
        }
          break;

        case HIDTouchDeviceType: {
          if (jtype == 'x') {
            sources[source] = touches[jid]->getFingerX(idx);
          }
          else if (jtype == 'y') {
            sources[source] = touches[jid]->getFingerY(idx);
          }
          else if (jtype == 'p') {
            sources[source] = touches[jid]->getFingerPressure(idx);
          }
          else if (jtype == 's') {
            sources[source] = touches[jid]->getFingerStatus(idx);
          }
        }
          break;

#ifdef WACOMTOUCH
        case WacomTouchDeviceType: {
          if (jtype == 'x') {
            sources[source] = wcandidates[jid]->getFingerX(idx);
          }
          else if (jtype == 'y') {
            sources[source] = wcandidates[jid]->getFingerY(idx);
          }
          else if (jtype == 's') {
            sources[source] = wcandidates[jid]->getFingerStatus(idx);
          }
          break;
        }
#endif
        default:
          assert(0);
        }
      }
    }
  }

  // (re) find the source and link
  if (sources.find(source) != sources.end()) {
    PDEB("Found source '" << source << "'");
    sources[source]->link(target, idxt);
    return true;
  }

  E_MOD("Cannot find data source " << isource);
  return false;
}

bool FlexiStick::addToSources(const std::string name,
                                 boost::intrusive_ptr<FlexiLink> converter)
{
  if (sources.find(name) != sources.end()) {
    E_MOD("Converter " << name << " already defined");
    return false;
  }
  if (name.find(":") != std::string::npos) {
    E_MOD("Converter name " << name << " illegal, cannot contain :");
    return false;
  }
  sources[name] = converter;
  return true;
}

bool FlexiStick::createCounter(const std::vector<std::string>& cdef)
{
  if (cdef.size() != 3 && cdef.size() != 4) {
    E_MOD("Need name for counter, and at least 2 inputs");
    return false;
  }

  // create the new counter in the current counter spot
  newcount.reset(new Counter());

  // link to its up and down input, possibly to centering, and add
  // to the list of sources
  if (!linkToSource(cdef[1], newcount, 0) ||
      !linkToSource(cdef[2], newcount, 1) ||
      (cdef.size() == 4 && !linkToSource(cdef[3], newcount, 2)) ||
      !addToSources(cdef[0], newcount)) return false;

  return true;
}

bool FlexiStick::defineCounter(const std::vector<int>& cpar)
{
  if (cpar.size() > 3 || cpar.size() < 1) {
    E_MOD("Need 1 to 3 values for counter");
    return false;
  }
  int cmin = (cpar.size() > 1) ? cpar[0] : 0;
  int cmax = (cpar.size() == 1) ? cpar[0] : cpar[1];
  int ccenter = (cpar.size() == 3) ? cpar[2] : (cmin + cmax) / 2;

  if (cmin >= cmax || ccenter > cmax || ccenter < cmin) {
    E_MOD("Incorrect parameters for counter");
    return false;
  }

  // set params and clear newcount
  newcount->defineCounter(cmin, cmax, ccenter);
  newcount.reset();

  return true;
}

bool FlexiStick::createWeightedSum(const std::vector<std::string>& cdef)
{
  if (cdef.size() < 2) {
    E_MOD("Need at least 1 value for weighted sum");
    return false;
  }

  // running variables
  std::string source;
  bool linkfound = true;

  // decode all input, either in form of "1.2*source" or just "source"
  try {
    boost::intrusive_ptr<flexistick::WeightedSum> newsum(new WeightedSum());
    for (unsigned ii = 1; ii < cdef.size() && linkfound; ii++) {

      // initial assumption, no factor, just a name
      float factor = 1.0f;
      source = cdef[ii];

      // is there a multiplication factor?
      std::string::size_type idxm = source.find('*');
      if (idxm != std::string::npos) {
        factor = boost::lexical_cast<float> (source.substr(0, idxm));
        source = source.substr(idxm+1);
      }

      // connect the source, and extend the weights array
      linkfound = linkToSource(source, newsum, ii - 1);
      newsum->addWeight(factor);
    }

    if (!linkfound || !addToSources(cdef[0], newsum)) {
      E_MOD("Cannot create new weight");
      return false;
    }
  }
  catch(const boost::bad_lexical_cast& e) {
    E_MOD("Cannot parse gain and device from " << source);
    return false;
  }

  return linkfound;
}

bool FlexiStick::createPoly(const std::vector<std::string>& cdef)
{
  // 2 strings; name, input
  if (cdef.size() != 2) {
    E_MOD("Need name for poly, and the input parameter");
    return false;
  }
  if (newpoly.get()) {
    E_MOD("Previous poly not completed");
    return false;
  }
  newpoly.reset(new PolyConverter());
  PDEB("new poly, name " << cdef[0]);

  // this prints name taken or problem finding link
  if (!linkToSource(cdef[1], newpoly, 0) ||
      !addToSources(cdef[0], newpoly)) {
    E_MOD("Cannot create new poly");
    return false;
  }

  return true;
}

bool FlexiStick::definePoly(const std::vector<double>& cpar)
{
  if (cpar.size() < 1) {
    E_MOD("Need at least one coefficient for polynomial");
    return false;
  }
  if (!newpoly.get()) {
    E_MOD("Polynomial not defined");
    return false;
  }
  // set coefficients and clear newpoly
  newpoly->definePoly(cpar);
  newpoly.reset();

  return true;
}

bool FlexiStick::createSteps(const std::vector<std::string>& cdef)
{
  // 2 strings; name, input
  if (cdef.size() != 2) {
    E_MOD("Need name for steps, and the input parameter");
    return false;
  }
  if (newsteps.get()) {
    E_MOD("Previous steps not completed");
    return false;
  }
  newsteps.reset(new StepsConverter());

  // this prints name taken or problem finding link
  if (!linkToSource(cdef[1], newsteps, 0) ||
      !addToSources(cdef[0], newsteps)) {
    E_MOD("Cannot create new steps converter");
    return false;
  }

  return true;
}

bool FlexiStick::defineSteps(const std::vector<double>& cpar)
{
  if ((cpar.size() < 2) || (cpar.size() % 2 != 0)) {
    E_MOD("Need one or more pairs for steps");
    return false;
  }
  if (!newsteps.get()) {
    E_MOD("Steps converter not defined");
    return false;
  }
  // set coefficients and clear newsteps
  newsteps->defineSteps(cpar);
  newsteps.reset();

  return true;
}

bool FlexiStick::addVirtual(const std::vector<std::string>& cdef)
{
  if (cdef.size() != 1 && cdef.size() != 2) {
    E_MOD("Need one or two strings defining virtual device");
    return false;
  }
  if (sources.find(cdef[0]) != sources.end()) {
    E_MOD("Name for virtual stick \"" << cdef[0] << "\" already taken");
    return false;
  }
  if (!sdlJoyInit()) return false;

  boost::intrusive_ptr<flexistick::GuiDevice> newdev;
  if (cdef.size() == 1) {
    newdev.reset(new GuiDevice(cdef[0], virtual_joyid));
  }
  else {
    newdev.reset(new GuiDevice(cdef[0], virtual_joyid, cdef[1]));
  }
  devices[virtual_joyid] =
    boost::intrusive_ptr<flexistick::HIDStick>(newdev);
  gui_device.push_back(newdev);
  I_MOD("New virtual stick " << cdef[0] << " no " << virtual_joyid);
  return true;
}

bool FlexiStick::addVirtualSlider(const std::vector<int>& cpar)
{
  if (cpar.size() != 5 && cpar.size() != 6) {
    E_MOD("Need five or six arguments for virtual slider");
    return false;
  }
  if (!gui_device.size()) {
    E_MOD("First create gui device!");
    return false;
  }


  new GuiDevice::GuiSlider(cpar[0], cpar[1], cpar[2], cpar[3], cpar[4],
                           gui_device.back().get(),
                           (cpar.size() == 6 && cpar[5] != 0));

  return true;
}

bool FlexiStick::addVirtualSlider2D(const std::vector<int>& cpar)
{
  if (cpar.size() != 5 && cpar.size() != 6) {
    E_MOD("Need five or six arguments for virtual 2D slider");
    return false;
  }
  if (!gui_device.size()) {
    E_MOD("First create gui device!");
    return false;
  }

  new GuiDevice::GuiSlider2D(cpar[0], cpar[1], cpar[2], cpar[3], cpar[4],
                             gui_device.back().get(),
                             (cpar.size() == 6 && cpar[5] != 0));

  return true;
}

bool FlexiStick::addVirtualButton(const std::vector<int>& cpar)
{
  if (cpar.size() != 3 && cpar.size() != 4) {
    E_MOD("Need three or four arguments for virtual button");
    return false;
  }
  if (!gui_device.size()) {
    E_MOD("First create gui device!");
    return false;
  }

  new GuiDevice::GuiButton(cpar[0], cpar[1], cpar[2],
                           gui_device.back().get(),
                           (cpar.size() == 4 && cpar[3] != 0));

  return true;
}

bool FlexiStick::addVirtualHat(const std::vector<int>& cpar)
{
  if (cpar.size() != 3 && cpar.size() != 4) {
    E_MOD("Need three or four arguments for virtual hat");
    return false;
  }
  if (!gui_device.size()) {
    E_MOD("First create gui device!");
    return false;
  }

  new GuiDevice::GuiHat(cpar[0], cpar[1], cpar[2],
                        gui_device.back().get(),
                        (cpar.size() == 4 && cpar[3] != 0));

  return true;
}

// Make a TypeCreator object for this module, the TypeCreator
// will check in with the script code, and enable the
// creation of modules of this type
static TypeCreator<FlexiStick> a(FlexiStick::getMyParameterTable());
