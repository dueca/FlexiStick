/* ------------------------------------------------------------------   */
/*      item            : FlexiStick.hxx
        made by         : repa
        from template   : DuecaModuleTemplate.hxx
        template made by: Rene van Paassen
        date            : Mon Jul 17 18:34:36 2017
        category        : header file
        description     :
        changes         : Mon Jul 17 18:34:36 2017 first version
        template changes: 030401 RvP Added template creation comment
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
*/

#ifndef FlexiStick_hxx
#define FlexiStick_hxx

// This includes headers for the objects that are sent over the channels
#include "comm-objects.h"

// include the dusime header
#include <dusime.h>
USING_DUECA_NS;

// include headers for functions/classes you need in the module
#include <list>
#include <map>
#include <SDL.h>
#include <SDL_joystick.h>
#include <SDL_touch.h>
#include <boost/shared_ptr.hpp>
#include "FlexiLink.hxx"
#include "Counter.hxx"
#include "PolyConverter.hxx"
#include "StepsConverter.hxx"
#include "JoystickDevice.hxx"
#include "TouchDevice.hxx"
#include "ChannelAccess.hxx"
#include "GuiDevice.hxx"
#include "HIDStick.hxx"
#include "HIDTouch.hxx"

//#define WACOMTOUCH
#ifdef WACOMTOUCH
// https://developer-docs.wacom.com/pages/viewpage.action?pageId=10422381
// http://mirror.informatimago.com/next/developer.apple.com/documentation/Carbon/Reference/CarbonFrameworkReference/CarbonFrameworkReference.pdf
#include "WacomTouch.hxx"
#endif

/** Multiple, joystick-based, versatile input module.
    This module accepts multiple joystick devices as input sources.

    Then the connection between the buttons and axes of the joysticks
    may be specified in the script. Optionally scaling and stepping
    can be applied.

    The instructions to create an module of this class from the Scheme
    script are:

    \verbinclude flexi-stick.scm

    The module is configured in the scheme script with the following
    steps:
    <ol>
    <li> Specify the usual, time specification, priority etc.
    <li> Specify which joystick devices you want to read. The module uses
         SDL2 read the joysticks, and in SDL joysticks are numbered from 0.
         Specify a joystick as "jsname:0", where jsname will be the name
         you use to refer to the joystick later. You can
         supply multiple devices, all axes hats and buttons from these devices
         will be accessible. Later, when accessing these inputs, strings like
         "jsname.a[0]" for example gives you the first axis on this stick. Hats
         can be selected with letter "h", hat up left right down values by
         "u" "l" "r" "d" respectively, and buttons by letter "b".
    <li> Specify which channel objects are to be written. A channel object
         is referred to with a short name, then the full (dueca) name of
         the channel, the data type and optionally an entry label.
    <li> Optionally specify counters that turn buttons or hat positions into
         counted values; supply a name for the counter, the input that counts
         down, the input that counts up, and optionally a centering input.
         For the up and down counting inputs specify buttons (as above,
         e.g., "jsname:b0") or hat directions, such as "jsname:u0" for the
         up direction for hat 0.
    <li> Optionally add polynomial conversions. Specify a new name and input,
         and the coefficients for the polynomial.
    <li> Optionally add stepping conversions, with name and input, and pairs of
         x,y conversion values.
    <li> Specify how axes and buttons are turned into values on the
         output channel or channels. Supply the written variable, referring to
         the name of the channel object, use a ".", and the member variable.
         If the member is of an array type, add the index in square brackets.
         Then supply the input for the value (either an axis, button or hat
         from the joystick, or one of the counter/converter results).
    </ol>

    A typical example is given here:

    \verbinclude flexi-stick-example.mod
*/
class FlexiStick: public SimulationModule
{
  /** self-define the module type, to ease writing the parameter table */
  typedef FlexiStick _ThisModule_;

private: // simulation data
  // declare the data you need in your simulation

  /** Time step of the updates */
  double deltat;

  // flag to remember init
  bool need_sdljoystick_init;

  // event layer init
  bool need_sdlevent_init;

  // number of joystick devices in SDL
  SDL_JoystickID sdl_njoy;

  // counter for virtual joystick devices
  SDL_JoystickID virtual_joyid;

  // number of touch devices in SDL
  int            sdl_ntouch;

  /** Initialisation before declaring any sdl */
  bool sdlJoyInit();

  /** Initialisation of only event interface for touch sdl */
  bool sdlTouchInit();

  /** Map with joystick devices */
  typedef std::map<SDL_JoystickID,
                   boost::intrusive_ptr<flexistick::HIDStick> > t_devices;

  /** Map with touch devices */
  typedef std::map<SDL_TouchID,
                   boost::intrusive_ptr<flexistick::HIDTouch> > t_touchdevices;

  /** Map with joystick devices */
  t_devices devices;

  /** Map with touch devices */
  t_touchdevices touches;

  /** Type of device in use */
  enum DeviceType {
    None,
#ifdef WACOMTOUCH
    WacomTouchDeviceType,
#endif
    JoystickDeviceType,
    HIDTouchDeviceType
  };

  /** Gui device */
  typedef std::list<boost::intrusive_ptr<flexistick::GuiDevice> > t_gui_device;

  /** Gui device */
  t_gui_device gui_device;

  /** Iterator for inserting virtual events */
  t_gui_device::iterator gui_current;

  /** Channel access and data collection type */
  typedef std::map<std::string,boost::intrusive_ptr<flexistick::ChannelAccess> >
  t_channelaccess;

  /** Keeps channel access and data collection together */
  t_channelaccess w_tokens;

  /** Conversion base type */
  typedef std::map<std::string,
                   boost::intrusive_ptr<flexistick::FlexiLink> > t_sources;

  /** All defined sources */
  t_sources sources;

  /** Currently being worked on counter. */
  boost::intrusive_ptr<flexistick::Counter> newcount;

  /** Currently being worked on polynomial */
  boost::intrusive_ptr<flexistick::PolyConverter> newpoly;

  /** Currently being worked on steps converter */
  boost::intrusive_ptr<flexistick::StepsConverter> newsteps;

#ifndef REPORT_ALWAYS_UNCONFIGURED
  /** Unreported hats/axes/buttons */
  typedef std::set<std::pair<uint32_t,uint32_t> > reported_t;

  /** Unconfigured axes */
  reported_t uc_axes;

  /** Unconfigured hats */
  reported_t uc_hats;

  /** Unconfigured buttons */
  reported_t uc_buttons;
#endif

#ifdef WACOMTOUCH
  /** flag for initialization */
  bool need_wacomtouch_init;

  /** List with configured wacom devices. */
  typedef std::vector<boost::intrusive_ptr<flexistick::WacomTouch> >
  t_wcandidates;

  t_wcandidates wcandidates;

  /** Map with detected and linked wacom devices */
  typedef std::map<int,
             boost::intrusive_ptr<flexistick::WacomTouch> > t_wdevices;

  t_wdevices wdevices;
#endif

#ifdef WACOMTOUCH
public:
  /** finger movement callback */
  int wacomFingerCallback(const WacomMTFingerCollection *fingerPacket);

  /** Initialization function */
  bool wacomTouchInit();

  /** Accept or deny stuff from device */
  bool processDeviceInfo(const WacomMTCapability& devinfo);

  /** detach device */
  bool processDeviceDetach(int DeviceID);
#endif

private:
#if defined(DataRecorder_hxx)
  /** Record event data for replay */
  bool                 enable_record_replay;
#endif

private: // activity allocation
  /** You might also need a clock. Don't mis-use this, because it is
      generally better to trigger on the incoming channels */
  PeriodicAlarm        myclock;

  /** Callback object for simulation calculation. */
  Callback<FlexiStick>  cb1;

  /** Activity for simulation calculation. */
  ActivityCallback      do_calc;

public: // class name and trim/parameter tables
  /** Name of the module. */
  static const char* const           classname;

  /** Return the parameter table. */
  static const ParameterTable*       getMyParameterTable();

public: // construction and further specification
  /** Constructor. Is normally called from scheme/the creation script. */
  FlexiStick(Entity* e, const char* part, const PrioritySpec& ts);

  /** Continued construction. This is called after all script
      parameters have been read and filled in, according to the
      parameter table. Your running environment, e.g. for OpenGL
      drawing, is also prepared. Any lengty initialisations (like
      reading the 4 GB of wind tables) should be done here.
      Return false if something in the parameters is wrong (by
      the way, it would help if you printed what!) May be deleted. */
  bool complete();

  /** Destructor. */
  ~FlexiStick();

  // add here the member functions you want to be called with further
  // parameters. These are then also added in the parameter table
  // The most common one (addition of time spec) is given here.
  // Delete if not needed!

  /** Specify a time specification for the simulation activity. */
  bool setTimeSpec(const TimeSpec& ts);

  /** Request check on the timing. */
  bool checkTiming(const vector<int>& i);

public: // member functions for cooperation with DUECA
  /** indicate that everything is ready. */
  bool isPrepared();

  /** start responsiveness to input data. */
  void startModule(const TimeSpec &time);

  /** stop responsiveness to input data. */
  void stopModule(const TimeSpec &time);

public: // the member functions that are called for activities
  /** the method that implements the main calculation. */
  void doCalculation(const TimeSpec& ts);

private:
  /** Read sdl event */
  bool _SDL_PollEvent(SDL_Event* ev, const DataTimeSpec& ts);

  /** Add a device to the list */
  bool addDevice(const std::string& dev);

  /** Add a touch device to the list */
  bool addTouch(const std::string& dev);

#ifdef WACOMTOUCH
  /** add a wacom-based device */
  bool addWacom(const std::string& dev);
#endif

  /** Add a channel */
  bool addChannel(const std::vector<std::string>& cpar);

  /** Link a device to a written variable */
  bool addLink(const std::vector<std::string>& lpar);

  /** Create a counter */
  bool createCounter(const std::vector<std::string>& cdef);

  /** Specify parameters for the counter */
  bool defineCounter(const std::vector<int>& cpar);

  /** Create a calibrating polynomial */
  bool createPoly(const std::vector<std::string>& cdef);

  /** Specify parameters for the calibration */
  bool definePoly(const std::vector<double>& cpar);

  /** Create a stepped conversion */
  bool createSteps(const std::vector<std::string>& cdef);

  /** Specify parameters for the counter */
  bool defineSteps(const std::vector<double>& cpar);

  /** Create a weighted sum of inputs */
  bool createWeightedSum(const std::vector<std::string>& cdef);

  /** Add a virtual stick based on gtk gui */
  bool addVirtual(const std::vector<std::string>& cdef);

  /** Add a slider to the latest virtual stick */
  bool addVirtualSlider(const std::vector<int>& cpar);

  /** Add a 2D slider to the latest virtual stick */
  bool addVirtualSlider2D(const std::vector<int>& cpar);

  /** Add a button to the latest virtual stick */
  bool addVirtualButton(const std::vector<int>& cpar);

  /** Add a hat to the latest virtual stick */
  bool addVirtualHat(const std::vector<int>& cpar);

private:
  /** Helper to find a source */
  bool linkToSource(const std::string& source,
                    boost::intrusive_ptr<flexistick::FlexiLink> target,
                    unsigned idx);

  /** Helper to store a converter */
  bool addToSources(const std::string name,
                    boost::intrusive_ptr<flexistick::FlexiLink> source);
};

#endif
