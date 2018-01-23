#include <Arduino.h>
#include <Homie.h>
#include <Boards.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace std;

struct Pin {
  uint8_t pin;
  bool normallyOpen;
};

vector<Pin> pins;

HomieNode switchesNode("switches", "switches"); // NOLINT

HomieSetting<const char *>
    pinSettings("pins", "Data pin settings (JSON)"); // NOLINT

bool switchHandler(const HomieRange &range, const String &value) {
  if (!range.isRange || range.index < 0 || range.index > pins.size()) {
    Homie.getLogger() << "Range is a range? " << range.isRange << endl
                      << "Range index: " << range.index << endl << "Pins: "
                      << pins.size();
    return true;
  }
  if (!(value.equalsIgnoreCase("true") || value.equalsIgnoreCase("false"))) {
    Homie.getLogger() << "Invalid value: " << value << endl;
    return true;
  }
  bool on = value.equalsIgnoreCase("true") != 0;
  Pin p = pins[range.index];
  if (p.normallyOpen) {
    Homie.getLogger() << "On? " << on << endl << "Writing pin " << p.pin << " "
                      << (on ? "low" : "high") << endl;
    digitalWrite(p.pin, (uint8_t) (on ? LOW : HIGH));
  } else {
    Homie.getLogger() << "On? " << on << endl << "Writing pin " << p.pin << " "
                      << (on ? "high" : "low") << endl;
    digitalWrite(p.pin, (uint8_t) (on ? HIGH : LOW));
  }
  switchesNode.setProperty("on")
      .setRange(range)
      .send(value);
  return true;
}

void setup() {
  Serial.begin(115200);
  Homie_setFirmware("boneswitch", "0.1.0");

  // these two handlers start "doing actual stuff" if Homie boots in "normal"
  // mode, which is why the stuff they do is not in this function.
  Homie.setSetupFunction(function<void()>([]() {
    StaticJsonBuffer<255> jsonBuffer;
    JsonArray &root = jsonBuffer.parseArray(pinSettings.get());
    pins.reserve(root.size());

    for (int i = 0; i < root.size(); i++) {
      uint8_t pinNumber;
      bool normallyOpen;
      if (root[i].is<JsonObject &>()) {
        JsonObject &relay = root[i].as<JsonObject>();
        pinNumber = relay["pin"].as<uint8_t>();
        if (relay.containsKey("normally_open")) {
          normallyOpen = relay["normally_open"];
        } else {
          normallyOpen = false;
        }
      } else {
        pinNumber = (uint8_t) root[i];
        normallyOpen = false;
      }
      pinMode(pinNumber, OUTPUT);
      Homie.getLogger() << "Resetting pin " << i << endl;
      digitalWrite(pinNumber, (uint8_t) (normallyOpen ? HIGH : LOW));
      Pin pin = {pinNumber, normallyOpen};
      pins.push_back(pin);
    }
  }));

  pinSettings.setDefaultValue("[{\"pin\": 1, \"normally_open\": false}]")
      .setValidator(
          function<bool(const char *)>([](const char *value) {
            StaticJsonBuffer<255> jsonBuffer;
            JsonArray &root = jsonBuffer.parseArray(value);
            if (!root.success()) {
              Homie.getLogger() << "Failed to parse root array" << endl;
              return false;
            }
            if (root.size() == 0) {
              Homie.getLogger() << "Empty root array" << endl;
              return false;
            }
            for (int i = 0; i < root.size(); i++) {
              if (root[i].is<long>() && !IS_PIN_DIGITAL(root[i])) {
                Homie.getLogger() << "Raw pin '" << root[i].as<char *>()
                                  << "' not valid" << endl;
                return false;
              }
              if (root[i].is<JsonObject &>()) {
                JsonObject &o = root[i].as<JsonObject>();
                if (!o.containsKey("pin")) {
                  Homie.getLogger() << "Array item " << i
                                    << " has no 'pin' property" << endl;
                  return false;
                }
                if (o["pin"].is<long>() && !IS_PIN_DIGITAL(o["pin"])) {
                  Homie.getLogger() << "Array item " << i
                                    << " has invalid 'pin' property" << endl;
                  return false;
                }
              } else {
                Homie.getLogger() << "Invalid array item at index " << i
                                  << endl;
                return false;
              }
            }
            return true;
          }));

  switchesNode.advertiseRange("on", 0, 7)
      .settable(switchHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();
}

#pragma clang diagnostic pop
