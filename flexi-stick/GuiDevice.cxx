/* ------------------------------------------------------------------   */
/*      item            : GuiDevice.cxx
        made by         : Rene' van Paassen
        date            : 170915
	category        : body file
        description     :
	changes         : 170915 first version
        language        : C++
*/

#define GuiDevice_cxx
#include "GuiDevice.hxx"

#include <iomanip>
#include <algorithm>

#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

using namespace dueca;

#if defined(DUECA_CONFIG_GTK2) && defined(DUECA_CONFIG_GTK3)
#error "Cannot have both gtk3 and gtk3, check DCOMPONENTS in main Makefile"
#endif

namespace flexistick {
  GuiDevice::GuiValueGroup::GuiValueGroup(GuiDevice* master) :
    intrusive_refcount(0),
    master(master)
  { }

   GuiDevice::GuiValueGroup::~GuiValueGroup()
  { }

  GuiDevice::GuiDevice(const std::string& name, SDL_JoystickID which,
                       const std::string& gfile) :
    HIDStick(name),
    gfile(gfile),
    cr(NULL),
    width(1),
    height(1),
    which(which)
  {
    //
  }

  bool GuiDevice::init() {

    static GladeCallbackTable table[] = {
#if defined(DUECA_CONFIG_GTK2)
      { "drawing", "expose-event", gtk_callback(&GuiDevice::drawgtk2) },
      { "drawing", "size_allocate", gtk_callback(&GuiDevice::updatesize) },
#else
      { "drawing", "draw", gtk_callback(&GuiDevice::draw) },
#endif
      // { "drawing", "realize", gtk_callback(&GuiDevice::realize) },
      { "drawing", "button-press-event", gtk_callback(&GuiDevice::buttonevent)},
      { "drawing", "button-release-event", gtk_callback(&GuiDevice::buttonevent)},
      { "drawing", "key-release-event", gtk_callback(&GuiDevice::keyevent) },
      { "drawing", "motion-notify-event", gtk_callback(&GuiDevice::motionevent) },
      { NULL } };

    fbwin.readGladeFile(gfile.c_str(),
                        "stick-draw", reinterpret_cast<gpointer>(this), table);
    gtk_widget_add_events(fbwin["drawing"],
                          GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON_RELEASE_MASK |
                          GDK_POINTER_MOTION_MASK |
                          GDK_LEAVE_NOTIFY_MASK);
    fbwin.show();
    return true;
  }

  GuiDevice::~GuiDevice()
  {
    //
  }

  void GuiDevice::pushEvent(const SDL_Event* ev)
  {
    events.push_back(*ev);
  }

  bool GuiDevice::pollEvent(SDL_Event* ev)
  {
    if (events.notEmpty()) {
      *ev = events.front();
      events.pop();
      return true;
    }
    return false;
  }

#ifdef DUECA_CONFIG_GTK2
  void GuiDevice::updatesize(GtkWidget *widget,
                             GtkAllocation *allocation, void *data)
  {
    width = allocation->width;
    height = allocation->height;
  }
#endif

#ifdef DUECA_CONFIG_GTK2
gboolean GuiDevice::drawgtk2(GtkWidget* widget, GdkEventExpose *event,
                             gpointer data)
#else
gboolean GuiDevice::draw(GtkWidget* widget, cairo_t *cr, gpointer data)
#endif
  {

#ifdef DUECA_CONFIG_GTK2
    cr = gdk_cairo_create(widget->window);
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.7);
#else
    guint width, height;
    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    // GdkRGBA color;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context (widget);

    gtk_render_background (context, cr, 0, 0, width, height);
    /* gtk_style_context_get_color (context,
                                 gtk_style_context_get_state (context),
                                 &color); */
#endif

    for (gvalue_list_t::iterator gg = gvalue.begin();
         gg != gvalue.end(); gg++) {
      (*gg)->draw(widget, cr, data);
    }

#ifdef DUECA_CONFIG_GTK2
    cairo_destroy(cr);
#endif

    return FALSE;
  }


  // ---------------------- GuiButton --------------------------------
  GuiDevice::GuiButton::GuiButton(double x, double y, double r,
                                  GuiDevice* master, bool sticky) :
    GuiValueGroup(master),
    state(NoInput),
    pressed(false),
    radius(r),
    sticky(sticky)
  {
    center[0] = x; center[1] = y;
    event.type = SDL_JOYBUTTONUP;
    event.timestamp = 0;
    event.which = master->getWhich();
    event.button = master->declareButton
      (boost::intrusive_ptr<GuiValueGroup>(this));
    event.state = SDL_RELEASED;
    event.padding1 = 0; event.padding2 = 0;
    master->pushEvent(reinterpret_cast<const SDL_Event*>(&event));
  }

  gboolean GuiDevice::GuiButton::
  draw(GtkWidget* widget, cairo_t *cr, gpointer data)
  {
    cairo_arc(cr, center[0], center[1], radius, -M_PI, M_PI);
    if (pressed) cairo_fill(cr);
    else cairo_stroke(cr);
    return FALSE;
  }

  bool GuiDevice::GuiButton::passChange()
  {
    if (pressed != (event.state == SDL_PRESSED)) {
      event.type = pressed ? SDL_JOYBUTTONDOWN : SDL_JOYBUTTONUP;
      event.state = pressed ? SDL_PRESSED : SDL_RELEASED;
      master->pushEvent(reinterpret_cast<const SDL_Event*>(&event));
      return true;
    }
    return false;
  }

#if 0
  bool GuiDevice::GuiButton::
  keyevent(GdkEventKey* evt)
  {
    if (!keyval || (keyval != evt->keyval)) return false;
    if (evt->type == GDK_KEY_PRESS) {
      pressed = true;
      state = ToggleOn;
      return true;
    }
    else if (evt->type == GDK_KEY_PRESS && state == ToggleOn) {
      state = NoInput;
      pressed = false;
      return true;
    }
    return false;
  }
#endif

  bool GuiDevice::GuiButton::
  buttonevent(GdkEventButton *button)
  {
    double rx = button->x-center[0];
    double ry = button->y-center[1];
    double d2 = rx*rx + ry*ry;

    bool button1 = !sticky && button->button == 1;
    bool button2 = button->button == 2 || (sticky && button->button == 1);

    if (button1) {
      if (button->type == GDK_BUTTON_RELEASE && state == Momentary) {
        state = NoInput;
        pressed = false;
      }
      else if (button->type == GDK_BUTTON_PRESS && d2 < radius*radius) {
        state = Momentary;
        pressed = true;
      }
    }
    else if (button2 && d2 < radius*radius &&
             button->type == GDK_BUTTON_PRESS) {
      if (state == NoInput) {
        state = ToggleOn;
        pressed = true;
      }
      else {
        state = NoInput;
        pressed = false;
      }
    }
    return passChange();
  }

  bool GuiDevice::GuiButton::motionevent(GdkEventMotion* mot)
  {
    if (state == Momentary) {
      double rx = mot->x-center[0];
      double ry = mot->y-center[1];
      double d2 = rx*rx + ry*ry;
      if (d2 > radius*radius) {
        state = NoInput;
        pressed = false;
      }
    }
    return passChange();
  }

  bool GuiDevice::GuiButton::leaveevent(GdkEventCrossing* key)
  {
    if (state == Momentary) {
      state = NoInput;
      pressed = false;
    }
    return passChange();
  }

  // ------------------- slider ---------------------------------------
  GuiDevice::GuiSlider::GuiSlider(double x0, double y0,
                                  double x1, double y1, double r,
                                  GuiDevice* master, bool sticky) :
    GuiValueGroup(master),
    grabbed(false),
    value(0.0),
    radius(r),
    sticky(sticky)
  {
    xstart[0] = x0; xstart[1] = y0;
    double dx = x1 - x0; double dy = y1 - y0;
    xend[0] = dx; xend[1] = dy;
    double xn = sqrt(dx*dx + dy*dy);
    double r3 = dy/xn*x0 - dx/xn*y0;
    xnorm[0] = -dy/xn/r;
    xnorm[1] = dx/xn/r;
    xnorm[2] = r3/r;
    xunit[0] = 2.0*dx/(xn*xn);
    xunit[1] = 2.0*dy/(xn*xn);
    xunit[2] = -xunit[0]*0.5*(x0+x1)-xunit[1]*0.5*(y0+y1);
    ratio = 1.0 + r/xn;
    event.type = SDL_JOYAXISMOTION;
    event.timestamp = 0;
    event.which = master->getWhich();
    event.axis = master->declareAxis(boost::intrusive_ptr<GuiValueGroup>(this));
    event.padding1 = 0;
    event.padding2 = 0;
    event.padding3 = 0;
    event.value = 0;
    event.padding4 = 0;
    master->pushEvent(reinterpret_cast<const SDL_Event*>(&event));
  }

  bool GuiDevice::GuiSlider::passChange()
  {
    int nval = lrint(value*0x7fff);
    if (event.value != nval) {
      event.value = nval;
      master->pushEvent(reinterpret_cast<const SDL_Event*>(&event));
      return true;
    }
    return false;
  }

  gboolean GuiDevice::GuiSlider::
  draw(GtkWidget* widget, cairo_t *cr, gpointer data)
  {
    PDEB("slider redraw, val " << value << ' ' << xstart.transpose() << "->" <<
         xend.transpose());
    cairo_save(cr);
    cairo_translate(cr, xstart[0], xstart[1]);
    cairo_line_to(cr, 0.0, 0.0);
    cairo_line_to(cr, xend[0], xend[1]);
    cairo_stroke(cr);
    cairo_arc(cr, 0.5*(value+1.0)*xend[0], 0.5*(value+1.0)*xend[1],
              radius, -M_PI, M_PI);
    cairo_fill(cr);
    cairo_restore(cr);
    return FALSE;
  }

  bool GuiDevice::GuiSlider::buttonevent(GdkEventButton *event)
  {
    if (event->button == 1 || event->button == 2) {
      if (event->type == GDK_BUTTON_PRESS) {
        if (fabs(xnorm[0]*event->x + xnorm[1]*event->y + xnorm[2]) < 1.0) {
          double v = xunit[0]*event->x + xunit[1]*event->y + xunit[2];
          if (fabs(v) <= ratio) {
            grabbed = true;
            value = std::max(-1.0, std::min(v, 1.0));
          }
        }
      }
      else if (grabbed && event->type == GDK_BUTTON_RELEASE) {
        grabbed = false;
        if (!sticky && event->button == 1) {
          value = 0.0;
        }
      }
    }
    return passChange();
  }

  bool GuiDevice::GuiSlider::motionevent(GdkEventMotion *event)
  {
    if (grabbed) {
      double v = xunit[0]*event->x + xunit[1]*event->y + xunit[2];
      value = std::max(-1.0, std::min(v, 1.0));
    }
    return passChange();
  }

  bool GuiDevice::GuiSlider::leaveevent(GdkEventCrossing *event)
  {
    if (grabbed) {
      grabbed = false;
    }
    return passChange();
  }

  //-------------------------- slider 2D ----------------------------
  GuiDevice::GuiSlider2D::
  GuiSlider2D(double x0, double y0, double x1, double y1, double r,
              GuiDevice* master, bool sticky) :
    GuiValueGroup(master),
    grabbed(false),
    valuex(0.0),
    valuey(0.0),
    radius(r),
    sticky(sticky)
  {
    xstart[0] = x0; xstart[1] = y0;
    double dx = x1 - x0; double dy = y1 - y0;
    xend[0] = dx; xend[1] = dy;
    xunit[0] = 2.0/dx; xunit[1] = 0.0; xunit[2] = -xunit[0]*0.5*(x1+x0);
    yunit[0] = 0.0; yunit[1] = 2.0/dy; yunit[2] = -yunit[1]*0.5*(y1+y0);

    eventx.type = SDL_JOYAXISMOTION;
    eventx.timestamp = 0;
    eventx.which = master->getWhich();
    eventx.axis =
      master->declareAxis(boost::intrusive_ptr<GuiValueGroup>(this));
    eventx.padding1 = 0;
    eventx.padding2 = 0;
    eventx.padding3 = 0;
    eventx.value = 0;
    eventx.padding4 =0;
    master->pushEvent(reinterpret_cast<const SDL_Event*>(&eventx));

    eventy.type = SDL_JOYAXISMOTION;
    eventy.timestamp = 0;
    eventy.which = master->getWhich();
    eventy.axis =
      master->declareAxis(boost::intrusive_ptr<GuiValueGroup>());
    eventy.padding1 = 0;
    eventy.padding2 = 0;
    eventy.padding3 = 0;
    eventy.value = 0;
    eventy.padding4 = 0;
    master->pushEvent(reinterpret_cast<const SDL_Event*>(&eventy));
  }

  bool GuiDevice::GuiSlider2D::passChange()
  {
    int nvalx = lrint(valuex*0x7fff);
    int nvaly = lrint(valuey*0x7fff);
    if (eventx.value == nvalx && eventy.value == nvaly) return false;
    if (eventx.value != nvalx) {
      eventx.value = nvalx;
      master->pushEvent(reinterpret_cast<const SDL_Event*>(&eventx));
    }
    if (eventy.value != nvaly) {
      eventy.value = nvaly;
      master->pushEvent(reinterpret_cast<const SDL_Event*>(&eventy));
    }
    return true;
  }

  gboolean GuiDevice::GuiSlider2D::
  draw(GtkWidget* widget, cairo_t *cr, gpointer data)
  {
    PDEB("slider 2D redraw, val " << valuex << "," << valuey << " " <<
         xstart.transpose() << "->" << xend.transpose());

    cairo_save(cr);
    cairo_translate(cr, xstart[0], xstart[1]);
    cairo_line_to(cr, 0.0, 0.0);
    cairo_line_to(cr, xend[0], 0.0);
    cairo_line_to(cr, xend[0], xend[1]);
    cairo_line_to(cr, 0.0, xend[1]);
    cairo_line_to(cr, 0.0, 0.0);
    cairo_stroke(cr);
    cairo_arc(cr, 0.5*(valuex+1.0)*xend[0],
              0.5*(valuey+1.0)*xend[1], radius, -M_PI, M_PI);
    cairo_fill(cr);
    cairo_restore(cr);
    return FALSE;
  }

  bool GuiDevice::GuiSlider2D::buttonevent(GdkEventButton *event)
  {
    if (event->button == 1 || event->button == 2) {
      if (event->type == GDK_BUTTON_PRESS) {
        double vx = xunit[0]*event->x + xunit[1]*event->y + xunit[2];
        double vy = yunit[0]*event->x + yunit[1]*event->y + yunit[2];
        if (fabs(vx) <= 1.0 && fabs(vy) <= 1.0) {
          grabbed = true;
          valuex = vx;
          valuey = vy;
        }
      }
      else if (grabbed && event->type == GDK_BUTTON_RELEASE) {
        grabbed = false;
        if (!sticky && event->button == 1) {
          valuex = 0.0; valuey = 0.0;
        }
      }
    }
    return passChange();
  }

  bool GuiDevice::GuiSlider2D::motionevent(GdkEventMotion *event)
  {
    if (grabbed) {
      double vx = xunit[0]*event->x + xunit[1]*event->y + xunit[2];
      double vy = yunit[0]*event->x + yunit[1]*event->y + yunit[2];
      valuex = std::max(-1.0, std::min(vx, 1.0));
      valuey = std::max(-1.0, std::min(vy, 1.0));
    }
    return passChange();
  }

  bool GuiDevice::GuiSlider2D::leaveevent(GdkEventCrossing *event)
  {
    if (grabbed) {
      grabbed = false;
    }
    return passChange();
  }

  // ----------------- hat ---------------------------------------

  GuiDevice::GuiHat::GuiHat(double x0, double y0, double r,
                            GuiDevice* master, bool sticky) :
    GuiValueGroup(master),
    state(NoInput),
    value(SDL_HAT_CENTERED),
    radius(r),
    sticky(sticky)
  {
    xcenter[0] = x0; xcenter[1] = y0;

    event.type = SDL_JOYHATMOTION;
    event.timestamp = 0;
    event.which = master->getWhich();
    event.hat = master->declareHat(boost::intrusive_ptr<GuiValueGroup>(this));
    event.value = SDL_HAT_CENTERED;
    event.padding1 = 0; event.padding2 = 0;
    master->pushEvent(reinterpret_cast<const SDL_Event*>(&event));
  }

  uint8_t GuiDevice::GuiHat::idxvalue[8] = {
    SDL_HAT_DOWN,     // 0 deg
    SDL_HAT_LEFTDOWN, // 45 deg
    SDL_HAT_LEFT,
    SDL_HAT_LEFTUP,
    SDL_HAT_UP,
    SDL_HAT_RIGHTUP,
    SDL_HAT_RIGHT,
    SDL_HAT_RIGHTDOWN
  };

  gboolean GuiDevice::GuiHat::
  draw(GtkWidget* widget, cairo_t *cr, gpointer data)
  {
    cairo_save(cr);
    cairo_translate(cr, xcenter[0], xcenter[1]);
    for (unsigned i = 8; i--; ) {
      cairo_save(cr);
      cairo_rotate(cr, 0.25*M_PI*i);
      if (value == idxvalue[i]) {
        cairo_arc(cr, 0.0, 0.5*radius, 0.25*radius, -M_PI, M_PI);
        cairo_fill(cr);
        PDEB("filling arc " << i
             << std::hex << int(idxvalue[i]) << std::dec);
      }
      else {
        cairo_arc(cr, 0.0, 0.5*radius, 0.25*radius, 0.0, M_PI);
        cairo_stroke(cr);
      }
      cairo_restore(cr);
    }
    return FALSE;
  }

  void GuiDevice::GuiHat::adjust(double d2, double rx, double ry)
  {
    static double rs = sin(M_PI/8.0);
    if (d2 < 0.20*radius*radius) {
      PDEB("rx " << rx << " ry " << ry << " c");
      value = SDL_HAT_CENTERED;
    }
    else if (rs*abs(rx) > abs(ry)) {
      PDEB("rx " << rx << " ry " << ry << " lr");
      value = (rx > 0.0) ? SDL_HAT_RIGHT : SDL_HAT_LEFT;
    }
    else if (rs*abs(ry) > abs(rx)) {
      PDEB("rx " << rx << " ry " << ry << " ud");
      value = (ry < 0.0) ? SDL_HAT_UP : SDL_HAT_DOWN;
    }
    else if (rx > 0) {
      PDEB("rx " << rx << " ry " << ry << " lru");
      value = (ry < 0.0) ? SDL_HAT_RIGHTUP : SDL_HAT_RIGHTDOWN;
    }
    else {
      PDEB("rx " << rx << " ry " << ry << " lrd");
      value = (ry < 0.0) ? SDL_HAT_LEFTUP : SDL_HAT_LEFTDOWN;
    }
  }

  bool GuiDevice::GuiHat::passChange()
  {
    if (value != event.value) {
      event.value = value;
      master->pushEvent(reinterpret_cast<const SDL_Event*>(&event));
      return true;
    }
    return false;
  }

  bool GuiDevice::GuiHat::
  buttonevent(GdkEventButton *event)
  {
    double rx = event->x-xcenter[0];
    double ry = event->y-xcenter[0];
    double d2 = rx*rx + ry*ry;
    if (d2 > radius*radius) return false;

    bool button1 = !sticky && event->button == 1;
    bool button2 = event->button == 2 || (sticky && event->button == 1);

    if (button1 && event->type == GDK_BUTTON_PRESS) {
      state = Momentary;
      adjust(d2, rx, ry);
    }
    else if (button1 && event->type == GDK_BUTTON_RELEASE) {
      state = NoInput;
      value = SDL_HAT_CENTERED;
    }
    else if (button2 && event->type == GDK_BUTTON_RELEASE) {
      if (state == ToggleOn) {
        state = NoInput;
        value = SDL_HAT_CENTERED;
      }
      else {
        adjust(d2, rx, ry);
        state = ToggleOn;
      }
    }
    return passChange();
  }

  bool GuiDevice::GuiHat::motionevent(GdkEventMotion *event)
  {
    if (state == Momentary) {
      double rx = event->x-xcenter[0];
      double ry = event->y-xcenter[0];
      double d2 = rx*rx + ry*ry;
      if (d2 > radius*radius) {
        state = NoInput;
        value = SDL_HAT_CENTERED;
      }
    }
    return passChange();
  }

  bool GuiDevice::GuiHat::leaveevent(GdkEventCrossing *event)
  {
    if (state == Momentary) {
     state = NoInput;
     value = SDL_HAT_CENTERED;
    }
    return passChange();
  }

  // ------- gui device itself ----------------------------------------
  Uint8
  GuiDevice::declareHat(boost::intrusive_ptr<GuiValueGroup> g)
  {
    boost::intrusive_ptr<HIDStick::HIDInputHat> nh(new HIDInputHat());
    hats.push_back(nh);
    gvalue.push_back(g);
    return Uint8(hats.size()-1);
  }

  Uint8
  GuiDevice::declareAxis(boost::intrusive_ptr<GuiValueGroup> g)
  {
    boost::intrusive_ptr<HIDStick::HIDInputAxis> nh(new HIDInputAxis());
    axes.push_back(nh);
    if (g.get()) gvalue.push_back(g);
    return Uint8(axes.size()-1);
  }

  Uint8
  GuiDevice::declareButton(boost::intrusive_ptr<GuiValueGroup> g)
  {
    boost::intrusive_ptr<HIDStick::HIDInputButton>
      nh(new HIDInputButton());
    buttons.push_back(nh);
    gvalue.push_back(g);
    return Uint8(buttons.size()-1);
  }

  gboolean GuiDevice::buttonevent(GtkWidget* widget,
                                  GdkEventButton *event, gpointer data)
  {
    bool redraw = false;

    if ((event->type == GDK_BUTTON_RELEASE ||
         event->type == GDK_BUTTON_PRESS) &&
        (event->button == 1 || event->button == 2)) {

      // toggle if it is a middle button release in a button zone
      // temporary on if it is a left button press in a button zone
      // off again on release or leaving zone
      for (gvalue_list_t::iterator gg = gvalue.begin();
           gg != gvalue.end(); gg++) {
        redraw |= (*gg)->buttonevent(event);
      }
    }
    if (redraw) {
      gtk_widget_queue_draw(widget);
    }
    return FALSE;
  }

  gboolean GuiDevice::
  keyevent(GtkWidget* widget, GdkEventKey *event, gpointer data)
  {
    bool redraw = false;
    for (gvalue_list_t::iterator gg = gvalue.begin();
         gg != gvalue.end(); gg++) {
      redraw |= (*gg)->keyevent(event);
    }
    if (redraw) {
      gtk_widget_queue_draw(widget);
    }
    return FALSE;
  }

  gboolean GuiDevice::
  motionevent(GtkWidget* widget, GdkEventMotion *event, gpointer data)
  {
    bool redraw = false;
    for (gvalue_list_t::iterator gg = gvalue.begin();
         gg != gvalue.end(); gg++) {
      redraw |= (*gg)->motionevent(event);
    }
    if (redraw) {
      gtk_widget_queue_draw(widget);
    }
    return FALSE;
  }



}
