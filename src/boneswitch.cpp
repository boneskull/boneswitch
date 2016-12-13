#include <Arduino.h>
#include <Homie.h>
#include <Boards.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#define DEFAULT_DATA_PIN D1
#define DEFAULT_ON_AT_START true

using namespace std;

HomieNode switchNode("switch", "switch");

HomieSetting<long> dataPinSetting("data_pin", "Data pin for relay");
HomieSetting<bool> initSetting("on_at_start", "If true, relay will be \"on\" at start");

void setup () {
  Serial.begin(115200);
  Homie_setFirmware("boneswitch", "0.1.0");

  // these two handlers start "doing actual stuff" if Homie boots in "normal"
  // mode, which is why the stuff they do is not in this function.
  Homie.setSetupFunction(function<void()>([] () {
    uint8_t dataPin = dataPinSetting.get();
    pinMode(dataPin, OUTPUT);
    digitalWrite(dataPin, (uint8_t)(initSetting.get() ? HIGH : LOW));
    switchNode.setProperty("on").send("false");
  }));

  // the settings' defaults should be present for config mode.
  dataPinSetting.setDefaultValue(DEFAULT_DATA_PIN).setValidator(
    function<bool (long)>([] (long value) {
      return IS_PIN_DIGITAL(value);
    }));

  initSetting.setDefaultValue(DEFAULT_ON_AT_START);

  switchNode.advertise("on").settable(function<bool (const HomieRange &, const String &)>([] (const HomieRange &range, const String &value) {
    uint8_t dataPin = dataPinSetting.get();
    if (value.equalsIgnoreCase("true") || value.equals("1")) {
      digitalWrite(dataPin, HIGH);
      switchNode.setProperty("on").send("true");
    } else {
      digitalWrite(dataPin, LOW);
      switchNode.setProperty("on").send("false");
    }
    return true;
  }));

  Homie.setup();
}

void loop () {
  Homie.loop();
}

#pragma clang diagnostic pop
