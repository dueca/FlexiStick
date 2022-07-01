/* ------------------------------------------------------------------   */
/*      item            : StickView.cxx
        made by         : repa
        from template   : DuecaModuleTemplate.cxx
        template made by: Rene van Paassen
        date            : Tue Jul 25 14:21:40 2017
        category        : body file
        description     :
        changes         : Tue Jul 25 14:21:40 2017 first version
        template changes: 030401 RvP Added template creation comment
                          060512 RvP Modified token checking code
                          160511 RvP Some comments updated
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
*/


#define StickView_cxx

// include the definition of the module class
#include "StickView.hxx"

// include the debug writing header, by default, write warning and
// error messages
#define W_MOD
#define E_MOD
#include <debug.h>

// include additional files needed for your calculation here

// the standard package for DUSIME, including template source
#define DO_INSTANTIATE
#include <dueca.h>

#ifdef DUECA_CONFIG_GTK3

// class/module name
const char* const StickView::classname = "stick-view";

// Parameters to be inserted
const ParameterTable* StickView::getMyParameterTable()
{
  static const ParameterTable parameter_table[] = {
    { "set-timing",
      new MemberCall<_ThisModule_,TimeSpec>
        (&_ThisModule_::setTimeSpec), set_timing_description },

    { "check-timing",
      new MemberCall<_ThisModule_,vector<int> >
      (&_ThisModule_::checkTiming), check_timing_description },

    /* You can extend this table with labels and MemberCall or
       VarProbe pointers to perform calls or insert values into your
       class objects. Please also add a description (c-style string).

       Note that for efficiency, set_timing_description and
       check_timing_description are pointers to pre-defined strings,
       you can simply enter the descriptive strings in the table. */

    /* The table is closed off with NULL pointers for the variable
       name and MemberCall/VarProbe object. The description is used to
       give an overall description of the module. */
    { NULL, NULL, "please give a description of this module"} };

  return parameter_table;
}

// constructor
StickView::StickView(Entity* e, const char* part, const
                   PrioritySpec& ps) :
  /* The following line initialises the SimulationModule base class.
     You always pass the pointer to the entity, give the classname and the
     part arguments. */
  Module(e, classname, part),

  // initialize the data you need in your simulation or process

  // initialize the channel access tokens, check the documentation for the
  // various parameters. Some examples:
  // r_mytoken(getId(), NameSet(getEntity(), MyData::classname, part),
  //           MyData::classname, 0, Channel::Events),
  // w_mytoken(getId(), NameSet(getEntity(), MyData2::classname, part),
  //           MyData2::classname, "label", Channel::Continuous),
  r_hiddata(getId(), NameSet(getEntity(), "FlexiStickTest", part),
            FlexiStickTest::classname, 0),
  // create a clock, if you need time based triggering
  // instead of triggering on the incoming channels
  // myclock(),

  // a callback object, pointing to the main calculation function
  cb1(this, &_ThisModule_::doCalculation),
  // the module's main activity
  do_calc(getId(), "show stick parameters", &cb1, ps)
{
  // connect the triggers for simulation
  do_calc.setTrigger(r_hiddata);
}

bool StickView::complete()
{
  static GladeCallbackTable table[] = {
    { "drawing", "draw", gtk_callback(&_ThisModule_::draw) },
    { NULL } };

  /* All your parameters have been set. You may do extended
     initialisation here. Return false if something is wrong. */
  fbwin.readGladeFile("../../../../FlexiStick/stick-view/stickviewgui.glade",
                      "stick-draw", reinterpret_cast<gpointer>(this), table);
  fbwin.show();

  return true;
}

// destructor
StickView::~StickView()
{
  //
}

// as an example, the setTimeSpec function
bool StickView::setTimeSpec(const TimeSpec& ts)
{
  // a time span of 0 is not acceptable
  if (ts.getValiditySpan() == 0) return false;

  // specify the timespec to the activity
  do_calc.setTimeSpec(ts);
  // or do this with the clock if you have it (don't do both!)
  // myclock.changePeriodAndOffset(ts);

  // do whatever else you need to process this in your model
  // hint: ts.getDtInSeconds()

  // return true if everything is acceptable
  return true;
}

// the checkTiming function installs a check on the activity/activities
// of the module
bool StickView::checkTiming(const vector<int>& i)
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
bool StickView::isPrepared()
{
  bool res = true;

  // Example checking a token:
  CHECK_TOKEN(r_hiddata);

  // return result of checks
  return res;
}

// start the module
void StickView::startModule(const TimeSpec &time)
{
  do_calc.switchOn(time);
}

// stop the module
void StickView::stopModule(const TimeSpec &time)
{
  do_calc.switchOff(time);
}

// this routine contains the main simulation process of your module. You
// should read the input channels here, and calculate and write the
// appropriate output
void StickView::doCalculation(const TimeSpec& ts)
{
  try {
    DataReader<FlexiStickTest> rd(r_hiddata, ts);
    tval = rd.data();
    //cout << rd.data() << endl;

    gtk_widget_queue_draw(fbwin["drawing"]);
  }
  catch (const dueca::NoDataAvailable& e) {
    // cout << "No data " << ts << endl;
  }
}

gboolean StickView::draw(GtkWidget* widget, cairo_t *cr, gpointer data)
{
  guint width, height;
  GdkRGBA color;
  GtkStyleContext *context;

  context = gtk_widget_get_style_context (widget);

  width = gtk_widget_get_allocated_width (widget);
  height = gtk_widget_get_allocated_height (widget);

  gtk_render_background (context, cr, 0, 0, width, height);
  gtk_style_context_get_color (context,
                               gtk_style_context_get_state (context),
                               &color);

  // 2d movement
  cairo_set_line_width(cr, 2.0);
  cairo_save(cr);
  cairo_translate(cr, 10, 10);
  cairo_scale(cr, height-20, height-20);
  cairo_rectangle(cr, 0.0, 0.0, 1.0, 1.0);
  cairo_move_to(cr, 0.5, 0.5);
  cairo_rel_line_to(cr, 0.5*tval.vecd[0], 0.5*tval.vecd[1]);
  cairo_arc(cr, 0.5*tval.vecd[0], 0.5*tval.vecd[1], 0.05, -M_PI, M_PI);
  cairo_restore(cr);
  cairo_stroke(cr);

  // rudder?
  cairo_save(cr);
  cairo_translate(cr, height/2, 5);
  cairo_scale(cr, height/2-10, height/2-10);
  cairo_line_to(cr, tval.vald, 0.0);
  cairo_arc(cr, tval.vald, 0.0, 0.02, -M_PI, M_PI);
  cairo_restore(cr);
  cairo_stroke(cr);

  // throttle
  cairo_save(cr);
  cairo_translate(cr, 5, height/2);
  cairo_scale(cr, height/2-10, height/2-10);
  cairo_line_to(cr, 0.0, tval.valf);
  cairo_arc(cr, 0.0, tval.valf, 0.02, -M_PI, M_PI);
  cairo_restore(cr);
  cairo_stroke(cr);

  // buttons
  cairo_arc(cr, height + 10, 20, 5, -M_PI, M_PI);
  if (tval.valb) cairo_fill(cr); else cairo_stroke(cr);
  cairo_arc(cr, height + 10, 40, 5, -M_PI, M_PI);
  if (tval.vecb[0]) cairo_fill(cr); else cairo_stroke(cr);
  cairo_arc(cr, height + 10, 60, 5, -M_PI, M_PI);
  if (tval.vecb[0]) cairo_fill(cr); else cairo_stroke(cr);

  return FALSE;
}

// Make a TypeCreator object for this module, the TypeCreator
// will check in with the script code, and enable the
// creation of modules of this type
static TypeCreator<StickView> a(StickView::getMyParameterTable());

#endif
