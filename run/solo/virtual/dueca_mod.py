## -*-python-*-
## this is an example dueca_mod.py file, for you to start out with and adapt
## according to your needs. Note that you need a dueca_mod.py file only for the
## node with number 0

## in general, it is a good idea to clearly document your set up
## this is an excellent place.

## node set-up
ecs_node = 0   # dutmms1, send order 3
#aux_node = 1   # dutmms3, send order 1
#pfd_node = 2   # dutmms5, send order 2
#cl_node = 3    # dutmms4, send order 0

## priority set-up
# normal nodes: 0 administration
#               1 simulation, unpackers
#               2 communication
#               3 ticker

# administration priority. Run the interface and logging here
admin_priority = dueca.PrioritySpec(0, 0)

# priority of simulation, just above adiminstration
sim_priority = dueca.PrioritySpec(1, 0)

# nodes with a different priority scheme
# control loading node has 0, 1 and 2 as above and furthermore
#               3 stick priority
#               4 ticker priority
# priority of the stick. Higher than prio of communication
# stick_priority = dueca.PrioritySpec(3, 0)

# timing set-up
# timing of the stick calculations. Assuming 100 usec ticks, this gives 2500 Hz
# stick_timing = dueca.TimeSpec(0, 4)

# this is normally 100, giving 100 Hz timing
sim_timing = dueca.TimeSpec(0, 100)

## for now, display on 50 Hz
display_timing = dueca.TimeSpec(0, 200)

## log a bit more economical, 25 Hz
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

    for e in ("ph-simple",):
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
            enable_record_replay = True).param(
                ('add-virtual', ("tiger", )),
                ('add-virtual-slider-2d',
                 (15, 15, 185, 185, 3)),
                # axis 2, yaw
                ('add-virtual-slider',
                 (10, 195, 190, 195, 3)),
                # axis 3, throttle
                ('add-virtual-slider',
                 (5, 190, 5, 10, 3, 1)),
                # place the window somewhere
                ('virtual-position-size', (450, 450)),
                 
                 # values are already coverted to -1 .. 1
                #('create-poly', ("stickx", "tiger.a[0]")),
                #('poly-params', (0.0, 1.0)),
                ('create-segments', ("stickx", "tiger.a[0]")),
                ('segments-params',
                 (-1.0, -1.0, -0.2, 0.0, 0.2, 0.0, 1.0, 1.0)),
                ('create-poly', ("sticky", "tiger.a[1]")),
                ('poly-params', (0.0, 1.0)),
                ('create-poly', ("throttle", "tiger.a[2]")),
                ('poly-params', (0.5, 0.5)),
 
                # channel written
                ('add-channel',
                 ("main",                       # var name
                  "FlexiStickTest://ph-simple", # channel full name
                  "FlexiStickTest",             # data class
                  "first try",                  # entry label
                  "event")),

                # link calculated & stick values to channel
                 ('add-link', ("main.vecd[0]", "stickx")),
                ('add-link', ("main.vecd[1]", "sticky")),
                ('add-link', ("main.valf", "tiger.a[2]")),
 
                ('add-channel',
                 ("second",
                  "MediumJoystick://ph-simple",
                  "MediumJoystick",
                  "verify",
                  "event")),

                ('add-link', ("second.axis[0]", "stickx")),
                ('add-link', ("second.axis[1]", "sticky")),
                ('add-link', ("second.axis[2]", "tiger.a[2]")),
                 )
        )
    mymods.append(dueca.Module(
        "stick-view", "", admin_priority).param(
            set_timing = sim_timing,
            check_timing = (10000, 20000))
            )
    filer = dueca.ReplayFiler("ph-simple", sim_priority)

# etc, each node can have modules in its mymods list

# then combine in an entity (one "copy" per node)
if mymods:
    myentity = dueca.Entity("ph-simple", mymods)
