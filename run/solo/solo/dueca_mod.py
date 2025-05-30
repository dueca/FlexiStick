## -*-python-*-

## configure the simulation

# use the replay functionality (advanced)
use_replay = False

# show the inputs in a graphical way
show_inputs = False

# entity name for naming channels, modules, logfiles etc.
entity_name = "ph-simple"

## find the joystick hardware through the SDL library
try:
    import sdl2

    preferjs = "Logitech"    # string to select joystick
    use_js = None
    sdl2.SDL_Init(sdl2.SDL_INIT_JOYSTICK)
    for js in range(sdl2.SDL_NumJoysticks()):
        print(f"Found joystick {js}:",
              sdl2.SDL_JoystickNameForIndex(js).decode('ascii'))
        if use_js is None:
            use_js = js
        elif preferjs and \
             preferjs in sdl2.SDL_JoystickNameForIndex(js).decode('ascii'):
            use_js = js
    sdl2.SDL_Quit()

except ModuleNotFoundError:
    print("Cannot import python sdl2 module, assuming joystick 0")
    use_js = 0

print(f"Selected joystick {use_js}")

## node set-up
ecs_node = 0   # only node in solo

## priority set-up
# normal nodes: 0 administration + graphics
#               1 simulation, unpackers
#               2 communication
#               3 ticker (highest_manager)

# administration priority. Run the interface and logging here
admin_priority = dueca.PrioritySpec(0, 0)

# priority of simulation, just above adiminstration
sim_priority = dueca.PrioritySpec(1, 0)

## timing set-up

# regular modules run at 100 Hz timing (100 ticks of 0.1ms)
sim_timing = dueca.TimeSpec(0, 100)

# display should be lower than common (vsync-locked) refresh rate, so 50 Hz
display_timing = dueca.TimeSpec(0, 200)

# log a bit more economical, 25 Hz
log_timing = dueca.TimeSpec(0, 400)

## ---------------------------------------------------------------------
### the modules needed for dueca itself
if this_node_id == ecs_node:

    # create a list of modules:
    DUECA_mods = []
    DUECA_mods.append(dueca.Module("dusime", "", admin_priority))
    DUECA_mods.append(dueca.Module("dueca-view", "", admin_priority))
    DUECA_mods.append(dueca.Module("activity-view", "", admin_priority))
    DUECA_mods.append(dueca.Module("timing-view", "", admin_priority))
    DUECA_mods.append(dueca.Module("log-view", "", admin_priority))
    DUECA_mods.append(dueca.Module("channel-view", "", admin_priority))

    if use_replay:
        for e in (entity_name,):
            DUECA_mods.append(
                dueca.Module("initials-inventory", e, admin_priority).param(
                    # reference_file=f"initials-{e}.toml",
                    store_file=f"initials-{e}-%Y%m%d_%H%M.toml"))
            DUECA_mods.append(
                dueca.Module("replay-master", e, admin_priority).param(
                    #reference_files=f"recordings-{e}.ddff",
                    store_files=f"recordings-{e}-%Y%m%d_%H%M%S.ddff"))

    # create the entity with that list
    DUECA_entity = dueca.Entity("dueca", DUECA_mods)

## ---------------------------------------------------------------------
# modules for your project (example)
mymods = []

if this_node_id == ecs_node:
    mymods.append(dueca.Module(
        "flexi-stick", "", sim_priority).param(
            set_timing = sim_timing,
            check_timing = (10000, 20000),
            enable_record_replay = use_replay).param(
                ('add-device', f"tiger:{use_js}"),
                # values are already -1 .. 1, no conversion would look like this:
                #('create-poly', ("stickx", "tiger.a[0]")),
                #('poly-params', (0.0, 1.0)),
                # instead, let's make some segments
                ('create-segments', ("stickx", "tiger.a[0]")),
                ('segments-params',
                 (-1.0, -1.0, -0.2, 0.0, 0.2, 0.0, 1.0, 1.0)),
                # for the second axis, make it a percentage
                ('create-poly', ("sticky", "tiger.a[1]")),
                ('poly-params', (0.0, 100.0)),
                # for the throttle, let it run from 0 to 1
                ('create-poly', ("throttle", "tiger.a[2]")),
                ('poly-params', (0.5, 0.5)),
                # let the hat up & down control a counter
                # note that the hats have d(own), u(p), l(eft), and r(ight) switches
                ('create-counter', ("iflaps", "tiger.d[0]", "tiger.u[0]")),
                ('counter-params', (0, 3, 0)),
                # convert the iflap counter to flap values
                ('create-steps', ("flaps", "iflaps")),
                ('steps-params', (0, 0,  1, 5,  2, 15,  3, 35)),
                # for kicks, add a summation of
                # iflaps and the left hat
                ('create-weighted', ("wsum", "2.0*iflaps", "-1.3*tiger.l[0]")),

                # channel written
                # note that the data class (DCO) needs to be in some module's
                # comm-objects.lst for it to be included in the DCO registry
                ('add-channel',
                 ("main",                          # var name, for use in add-link
                  "FlexiStickTest://"+entity_name, # channel full name, convention is DCO://Entity
                  "FlexiStickTest",                # data class, from registry
                  "",                              # entry label
                  "event")),                       # [optional] event channel, leave out for stream

                # link calculated & stick values to channel
                ('add-link', ("main.valb", "tiger.b[0]")), # direct button value
                ('add-link', ("main.vecd[0]", "stickx")),  # calculated polynomial
                ('add-link', ("main.vecd[1]", "sticky")),  # calculated polynomial
                ('add-link', ("main.valf", "tiger.a[2]")), # direct axis value
                ('add-link', ("main.vals", "tiger.b[0]")), # direct button value
                # direct hat output is a bitmask (0x1: up, 0x2: right, 0x4: down, 0x8: left)
                ('add-link', ("main.vali", "tiger.h[0]")), # direct hat value 
                ('add-link', ("main.vald", "wsum")),       # calculated weighted sum

                ('add-channel',
                 ("second",                        # var name
                  "MediumJoystick://"+entity_name, # channel full name
                  "MediumJoystick",                # data class
                  "")),                            # entry label

                ('add-link', ("second.axis[0]", "stickx")),       # calculated polynomial
                ('add-link', ("second.axis[1]", "sticky")),       # calculated polynomial
                ('add-link', ("second.axis[2]", "tiger.a[2]")),   # direct axis value
                ('add-link', ("second.button[0]", "tiger.b[0]")), # direct button value
                )
        )

    # an optional graphical representation of the FlexiStickTest channel
    if show_inputs :
        mymods.append(dueca.Module(
            "stick-view", "", admin_priority).param(
                set_timing = sim_timing,
                check_timing = (10000, 20000))
        )

    # the recorder for the replay functionality, if enabled
    if use_replay:
        filer = dueca.ReplayFiler(entity_name, sim_priority)

# then combine in an entity (one "copy" per node)
if mymods:
    myentity = dueca.Entity(entity_name, mymods)
