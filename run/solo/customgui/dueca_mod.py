## -*-python-*-
## dueca_mod.py: created with DUECA version 4.3.1
## Created on: 31-Jul-2026

## A simple example configuration that uses a custom ui (gtk3 or gtk4)
## with an image background, for a GUI "joystick".

## [nodes] node set-up
ecs_node = 0    # experiment control station, node 0 with gui

## priority set-up
# normal nodes: 0 administration
#               1 logging
#               2 simulation, unpackers
#               3 communication
#               4 ticker

# administration priority. Run the interface and logging here
admin_priority = dueca.PrioritySpec(0, 0)

# logging prio. Keep this free from time-critical other processes
log_priority = dueca.PrioritySpec(1, 0)

# priority of simulation, just above log
sim_priority = dueca.PrioritySpec(2, 0)

## timing set-up

# simulation process, this is normally 100, giving 100 Hz timing
sim_timing = dueca.TimeSpec(0, 1000)

## for now, display on 50 Hz
display_timing = dueca.TimeSpec(0, 200)

## log a bit more economical, 25 Hz
log_timing = dueca.TimeSpec(0, 400)

## [timing] nodes with different timing set-up

# timing of the stick calculations. Assuming 100 usec ticks, this gives 2500 Hz
# stick_timing = dueca.TimeSpec(0, 4)

## the name for the main entity to create. Note that you can create as
## many entities as you want, usually one is enough. Adjust as appropriate
entity = "CUSTOM"

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
    # uncomment for web-based graph, see DUECA documentation
    # DUECA_mods.append(dueca.Module("config-storage", "", admin_priority))

    if no_of_nodes > 1 and not classic_ip:
        DUECA_mods.append(dueca.Module("net-view", "", admin_priority))

    # create the DUECA entity with that list
    DUECA_entity = dueca.Entity("dueca", DUECA_mods)

## ---------------------------------------------------------------------
# modules for your project
mymods = []

## [modules] modules to be created in node 0 ("ecs")
if this_node_id == ecs_node:

    # demo configuration of a gui stick interface with a custom background image,
    # available for gtk3 and gtk4 variants
    jsdevice = (
            ("add-virtual", ("joy", f"{graphic_interface}/stickgui-custom.ui")),
            # roll and pitch axes
            ("add-virtual-slider-2d",
             (15, 15, 185, 185, 3, 0, 0, -20)),
            # axis 2, yaw
            ('add-virtual-slider',
             (10, 195, 190, 195, 3)),
            # axis 3, throttle
            ('add-virtual-slider',
             (5, 10, 5, 190, 3, 1, -50)),
            # buttons 0, 1 not used, out of screen?
            ('add-virtual-button', (0, 210, 0)),
            ('add-virtual-button', (10, 210, 0)),
            # buttons 2, 4 for trim, 3, 5 for flaps extend, retract
            ('add-virtual-button', (195, 180, 3)),  # 2
            ('add-virtual-button', (195,  60, 3)),  # 3
            ('add-virtual-button', (195, 160, 3)),  # 4
            ('add-virtual-button', (195, 90, 3)),
            )

    mymods.append(
        dueca.Module("flexi-stick", "", admin_priority).param(
            ("set-timing", sim_timing),
            # device definition from above
            *jsdevice,
            # take the values of the axes; label and convert
            ("create-poly", ("stickx", "joy.a[0]")),
            ("poly-params", (0.0, -0.04)),
            ("create-poly", ("sticky", "joy.a[1]")),
            ("poly-params", (0.0, -0.04)),
            ("create-poly", ("stickz", "joy.a[2]")),
            ("poly-params", (0.0, -0.08 * 3)),
            ("create-poly", ("throttle", "joy.a[3]")),
            ("poly-params", (0.5, -0.5)),
            ("create-counter", ("flapcount", "joy.b[3]", "joy.b[5]")),
            ("counter-params", (0, 3, 0)),
            ("create-steps", ("flaps", "flapcount")),
            ("steps-params", (0, 0, 1, 5, 2, 15, 3, 35)),
            # write PrimaryControls
            (
                "add-channel",
                ("prim", f"MediumJoystick://{entity}/prim", "MediumJoystick", ""),
            ),
            # link the values
            ("add-link", ("prim.axis[0]", "stickx")),
            ("add-link", ("prim.axis[1]", "sticky")),
            ("add-link", ("prim.axis[2]", "stickz")),
            # write SecondaryControls
            (
                "add-channel",
                ("sec", f"MediumJoysitick://{entity}/sec", "MediumJoystick", ""),
            ),
            ("add-link", ("sec.axis[0]", "throttle")),
            ("add-link", ("sec.axis[1]", "throttle")),
            ("add-link", ("sec.axis[2]", "flaps")),
            # write SecondarySwitches
            (
                "add-channel",
                ("sw1", f"MediumJoysitick://{entity}/sw2", "MediumJoystick", ""),
            ),
            ("add-link", ("sw1.button[0]", "joy.b[2]")),
            ("add-link", ("sw1.button[1]", "joy.b[4]"))
        )

    )

# then combine in an entity
if mymods:
    myentity = dueca.Entity(entity, mymods)
