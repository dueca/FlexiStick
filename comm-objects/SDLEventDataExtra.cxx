void SDLEventData::getEvent(SDL_Event& ev)
{
  ev.type = type;
  ev.common.timestamp = timestamp;
  dueca::AmorphReStore s(data.data(), data.size());

  switch(ev.common.type) {
  case SDL_QUIT:
    return;

  case SDL_JOYAXISMOTION:
    ::unPackData(s, ev.jaxis.which);  // uint32
    ::unPackData(s, ev.jaxis.axis);   // uint8
    ::unPackData(s, ev.jaxis.value);  // int16
    return;

  case SDL_JOYHATMOTION:
    ::unPackData(s, ev.jhat.which); // uint32
    ::unPackData(s, ev.jhat.hat);   // uint8
    ::unPackData(s, ev.jhat.value); // uint8
    return;

  case SDL_JOYBUTTONDOWN:
  case SDL_JOYBUTTONUP:
    ::unPackData(s, ev.jbutton.which); // uint32
    ::unPackData(s, ev.jbutton.button);// uint8
    ::unPackData(s, ev.jbutton.state); // uint8
    return;

  case SDL_FINGERDOWN:
  case SDL_FINGERMOTION:
  case SDL_FINGERUP:
    ::unPackData(s, ev.tfinger.touchId);  // int64
    ::unPackData(s, ev.tfinger.fingerId); // int64
    ::unPackData(s, ev.tfinger.x);        // float
    ::unPackData(s, ev.tfinger.y);        // float
    ::unPackData(s, ev.tfinger.dx);       // float
    ::unPackData(s, ev.tfinger.dy);       // float
    ::unPackData(s, ev.tfinger.pressure); // float
    ::unPackData(s, ev.tfinger.windowID); // uint32
    return;
  }
}

SDLEventData::SDLEventData(const SDL_Event& ev) :
  type(ev.common.type),
  timestamp(ev.common.timestamp)
{
  dueca::AmorphStore s(data.data(), data.size());

  switch(ev.common.type) {
  case SDL_QUIT:
    return;

  case SDL_JOYAXISMOTION:
    ::packData(s, ev.jaxis.which);
    ::packData(s, ev.jaxis.axis);
    ::packData(s, ev.jaxis.value);
    return;

  case SDL_JOYHATMOTION:
    ::packData(s, ev.jhat.which);
    ::packData(s, ev.jhat.hat);
    ::packData(s, ev.jhat.value);
    return;

  case SDL_JOYBUTTONDOWN:
  case SDL_JOYBUTTONUP:
    ::packData(s, ev.jbutton.which);
    ::packData(s, ev.jbutton.button);
    ::packData(s, ev.jbutton.state);
    return;

  case SDL_FINGERDOWN:
  case SDL_FINGERMOTION:
  case SDL_FINGERUP:
    ::packData(s, ev.tfinger.touchId);
    ::packData(s, ev.tfinger.fingerId);
    ::packData(s, ev.tfinger.x);
    ::packData(s, ev.tfinger.y);
    ::packData(s, ev.tfinger.dx);
    ::packData(s, ev.tfinger.dy);
    ::packData(s, ev.tfinger.pressure);
    ::packData(s, ev.tfinger.windowID);
    return;
  }
}
