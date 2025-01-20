/* ------------------------------------------------------------------   */
/*      item            : GuiDevice.hxx
        made by         : Rene van Paassen
        date            : 170915
        category        : header file
        description     :
        changes         : 170915 first version
        language        : C++
*/

#ifndef GuiDevice_hxx
#define GuiDevice_hxx
#include "FlexiLink.hxx"
#include "HIDStick.hxx"
#include <Eigen/Dense>
#include <GtkGladeWindow.hxx>
#include <boost/intrusive_ptr.hpp>
#include <gtk/gtk.h>
#include <list>
#include <string>
#include <vector>
#define DO_INSTANTIATE
#include <SDL.h>
#include <dueca/AsyncList.hxx>

namespace flexistick {

/** Simulation of joystick input */
class GuiDevice : public HIDStick
{
  /** canvas for control */
  dueca::GtkGladeWindow fbwin;

  /** Window opening file */
  std::string gfile;

  /** List of generated events */
  dueca::AsyncList<SDL_Event> events;

  /** Cairo pointer, only for gtk2 */
  cairo_t *cr;

  /** Widget width and height */
  int width, height;

public:
  /** control equivalent to 1 or 2 axis joystick part, hat or button */
  struct GuiValueGroup
  {

    /** refcounter */
    unsigned intrusive_refcount;

    /** point to master */
    GuiDevice *master;

    /** redraw the control and value */
    virtual void draw(cairo_t *cr, int width, int height) = 0;

    virtual bool keyevent(guint keyval, bool press);

    virtual bool motionevent(gdouble x, gdouble y) = 0;

    virtual bool leaveevent() = 0;

    virtual bool buttonevent(guint button, bool press, gdouble x, gdouble y) = 0;

    /** Default constructor, initialize refcount */
    GuiValueGroup(GuiDevice *master);

    /** Destructor */
    virtual ~GuiValueGroup();
  };

  /** momentary press button

     left click does momentary on, is removed on
        (1) left release <buttonevent>
        (2) mouse move out of region <motionevent>
        (3) leave window <leaveevent>

     middle click toggles on, is removed on
        (1) middle click in region
        (2) keypress

     when 'sticky', left click behaves as middle click
  */
  struct GuiButton : public GuiValueGroup
  {

    /** propagation of value */
    SDL_JoyButtonEvent event;

    /** Input state */
    enum InputState {
      NoInput,   /**< No input currently active */
      Momentary, /**< Switched on with left mouse button */
      ToggleOn   /**< Toggled on with middle button or key */
    };

    /** State of control */
    InputState state;

    /** value */
    bool pressed;

    /** center of the button on the gui canvas */
    Eigen::Vector2d center;

    /** radius of area */
    double radius;

    /** sticky property, left button toggles */
    bool sticky;

    /** Constructor */
    GuiButton(double x, double y, double r, GuiDevice *master, bool sticky);

    /** Check for changed value and pass if so */
    bool passChange();

    /** draw function */
    void draw(cairo_t *cr, int width, int height) final;

    /** react to button press or release */
    bool buttonevent(guint button, bool press, gdouble x, gdouble y) final;

#if 0
      /** switch on with button press */
      bool keyevent(GdkEventKey* key);
#endif

    /** react to a mouse motion event */
    bool motionevent(gdouble x, gdouble y) final;

    /** react to a window leave, default is to do nothing */
    bool leaveevent() final;
  };

  /** single dimension slider

     left makes the slider value follow the mouse, input back to zero on
        (1) left release <buttonevent>
        (2) mouse move out of region <motionevent>
        (3) leave window <leaveevent>

     middle click toggles sticky input, is removed on
        (1) middle or left click in region

     when 'sticky', left click behaves as middle click
*/
  struct GuiSlider : public GuiValueGroup
  {

    /** propagation of value */
    SDL_JoyAxisEvent event;
    // boost::intrusive_ptr<HIDBase::HIDInputAxis> out;

    /** has been grabbed? */
    bool grabbed;

    /** state, -1 to 1 */
    double value;

    /** bottom/left of the slider */
    Eigen::Vector2d xstart;

    /** top/right of the slider */
    Eigen::Vector2d xend;

    /** normal vector for distance calculation */
    Eigen::Vector3d xnorm;

    /** along vector for scaling */
    Eigen::Vector3d xunit;

    /** cap for sensitive region */
    double ratio;

    /** width/size of button */
    double radius;

    /** sticky property, left button toggles */
    bool sticky;

    /** slider creation */
    GuiSlider(double x0, double y0, double x1, double y1, double r,
              GuiDevice *master, bool sticky);

    /** Check for changed value and pass if so */
    bool passChange();

    /** draw function */
    void draw(cairo_t *cr, int width, int height) final;

    /** react to button press or release */
    bool buttonevent(guint button, bool press, gdouble x, gdouble y) final;

    /** react to a mouse motion event */
    bool motionevent(gdouble x, gdouble y) final;

    /** react to a window leave, default is to do nothing */
    bool leaveevent() final;
  };

  /** double dimension slider

     left makes the slider value follow the mouse, input back to zero on
        (1) left release <buttonevent>
        (2) mouse move out of region <motionevent>
        (3) leave window <leaveevent>

     middle click toggles sticky input, is removed on
        (1) middle or left click in region

     when 'sticky', left click behaves as middle click
  */
  struct GuiSlider2D : public GuiValueGroup
  {

    /** propagation of value 1 */
    SDL_JoyAxisEvent eventx;
    // boost::intrusive_ptr<HIDBase::HIDInputAxis> outx;

    /** propagation of value 2 */
    SDL_JoyAxisEvent eventy;
    // boost::intrusive_ptr<HIDBase::HIDInputAxis> outy;

    /** grabbed */
    bool grabbed;

    /** state, -1 to 1 */
    double valuex;

    /** state, -1 to 1 */
    double valuey;

    /** bottom/left of the slider */
    Eigen::Vector2d xstart;

    /** top/right of the slider */
    Eigen::Vector2d xend;

    /** convert to value vector x */
    Eigen::Vector3d xunit;

    /** convert to value vector x */
    Eigen::Vector3d yunit;

    /** width/size of button */
    double radius;

    /** sticky property, left button toggles */
    bool sticky;

    /** slider creation */
    GuiSlider2D(double x0, double y0, double x1, double y1, double r,
                GuiDevice *master, bool sticky);

    /** Check for changed value and pass if so */
    bool passChange();

    /** draw function */
    void draw(cairo_t *cr, int width, int height) final;

    /** react to button press or release */
    bool buttonevent(guint button, bool press, gdouble x, gdouble y) final;

    /** react to a mouse motion event */
    bool motionevent(gdouble x, gdouble y) final;

    /** react to a window leave, default is to do nothing */
    bool leaveevent() final;
  };

  /** momentary press hat

     left click does momentary on, is removed on
        (1) left release <buttonevent>
        (2) mouse move out of region <motionevent>
        (3) leave window <leaveevent>

     middle click toggles on, is removed on
        (1) middle click in region

     when 'sticky', left click behaves as middle click
  */
  struct GuiHat : public GuiValueGroup
  {

    /** Upwards propagation of value */
    SDL_JoyHatEvent event;

    /** Input state */
    enum InputState {
      NoInput,   /**< No input currently active */
      Momentary, /**< Switched on with left mouse button */
      ToggleOn   /**< Toggled on with middle button or key */
    };

    /** Input influence on the hat */
    InputState state;

    /** Hat position */
    uint8_t value;

    /** Give each position a value */
    static uint8_t idxvalue[8];

    /** center of the hat */
    Eigen::Vector2d xcenter;

    /** width/size of hat */
    double radius;

    /** sticky property, left button toggles */
    bool sticky;

    /** hat creation */
    GuiHat(double x0, double y0, double r, GuiDevice *master, bool sticky);

    /** Check for changed value and pass if so */
    bool passChange();

    /** draw function */
    void draw(cairo_t *cr, int width, int height) final;

    /** react to button press or release */
    bool buttonevent(guint button, bool press, gdouble x, gdouble y) final;

    /** react to a mouse motion event */
    bool motionevent(gdouble x, gdouble y) final;

    /** react to a window leave, default is to do nothing */
    bool leaveevent() final;

  private:
    /** calculate new state */
    void adjust(double d2, double rx, double ry);
  };

  /** List types */
  typedef std::list<boost::intrusive_ptr<GuiValueGroup>> gvalue_list_t;

  /** List of elements */
  gvalue_list_t gvalue;

  /** Joystick ID for this simulated JD */
  SDL_JoystickID which;

public:
  /** With a specific gui file */
  GuiDevice(const std::string &name, SDL_JoystickID which,
            const std::string &fgui =
#if GTK_CHECK_VERSION(4, 0, 0)
              "../../../../FlexiStick/flexi-stick/stickgui.ui"
#elif GTK_CHECK_VERSION(3, 0, 0)
              "../../../../FlexiStick/flexi-stick/stickgui.glade"
#elif GTK_CHECK_VERSION(2, 0, 0)
              "../../../../FlexiStick/flexi-stick/stickgui.glade2"
#else
#error "No suitable GTK version found"
#endif
  );

  /** Set window position and size */
  void setPositionAndSize(const std::vector<int> &ps);

  /** Open the window */
  bool init();

  /** Destructor */
  ~GuiDevice();

  /** Return simulated joystick number */
  SDL_JoystickID getWhich() const { return which; }

  /** push value into list */
  void pushEvent(const SDL_Event *);

  /** get a value from the list */
  bool pollEvent(SDL_Event *ev);

#if GTK_CHECK_VERSION(4, 0, 0)
  /** draw function */
  void draw(cairo_t *cr, int width, int height);

  /** react to button press or release */
  void buttonevent(guint button, bool press, gdouble x, gdouble y);

  /** react to a mouse motion event */
  void motionevent(gdouble x, gdouble y);

  /** react to a window leave, default is to do nothing */
  void leaveevent();

  /** Key press event */
  void keyevent(guint keyval, bool press);

#elif GTK_CHECK_VERSION(2, 0, 0)

#if GTK_CHECK_VERSION(3, 0, 0)

  /** Draw callback from gtk */
  gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer data);
#else

  /** Old style drawing */
  gboolean drawgtk2(GtkWidget *widget, GdkEventExpose *event, gpointer data);

  /** Get updated size */
  void updatesize(GtkWidget *widget, GtkAllocation *allocation, void *data);
#endif

  /** Button state change */
  gboolean buttonevent(GtkWidget *widget, GdkEventButton *event, gpointer data);

    /** Key release */
  gboolean keyevent(GtkWidget *widget, GdkEventKey *event, gpointer data);

    /** Pointer motion event */
  gboolean motionevent(GtkWidget *widget, GdkEventMotion *event, gpointer data);

    /** Leave event, reset any motion */
  gboolean leaveevent(GtkWidget *widget, GdkEventCrossing *event,
                      gpointer data);
#endif
  /** create new hat in gui device */
  Uint8 declareHat(boost::intrusive_ptr<GuiValueGroup> g);

  /** create new axis in gui device */
  Uint8 declareAxis(boost::intrusive_ptr<GuiValueGroup> g);

  /** create new button in gui device */
  Uint8 declareButton(boost::intrusive_ptr<GuiValueGroup> g);
};

} // namespace flexistick
#endif
