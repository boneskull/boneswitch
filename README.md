# boneswitch

> [homie-esp8266](https://github.com/marvinroger/homie-esp8266) + relay

## Overview

This code allows for operation of a relay over MQTT via the [Homie](https://github.com/marvinroger/homie) convention.

### Settings

- `{long}` `data_pin`: Your relay control pin.  Defaults to `5`, which is pin "D1" in ESP8266-speak.
- `{bool}` `on_at_start`: Whether or not to open the switch at "boot".  Defaults to `true`.

### Nodes

Exposes a Homie node `switch` with property `on` which can be set to `"true"` or `"false"`.

To open the relay, publish on MQTT with topic `<base-topic>/<device-id>/switch/set/on` and payload `true`.  Likewise, to close the relay, send payload `false` instead.

## Hardware

**DISCLAIMER: This is not intended to be a tutorial.  Follow proper safety precautions when working with mains power.**

WEMOS D1 Mini:

![wemos d1 mini](https://cldup.com/FfiN4aoDEL.png)

WEMOS Relay Shield:

![wemos relay shield](https://cldup.com/7PM3u17Fz3.png)

And put 'em together like so:

![combined](https://cldup.com/fMvdKWfRuj.png)

For the relay, I took a garden-variety ungrounded NEMA 1 extension cord, then cut and stripped it.  I "broke" the live wire by inserting it (from the male end) into the "NC" screw terminal. I then inserted the other live wire (from the female end) into the middle screw terminal.  I re-soldered the ends of the neutral wires (this is the wire corresponding to the "larger" of the two male blades) back together and protected the connection with heat shrink.

## License

:copyright: 2016 [Christopher Hiller](https://github.com/boneskull).  Licensed MIT.
