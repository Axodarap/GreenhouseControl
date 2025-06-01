// HaGreenhouseImpl.cpp
#include "HomeAssistantGreenhouse.h"

namespace GreenhouseHA {

HADevice _device("Greenhouse");
HAMqtt* _mqtt = nullptr;

HASwitch _pumpSwitch("greenhouse_pump");
HABinarySensor _pumpStatus("greenhouse_pump_status");
HASwitch _fanSwitch("greenhouse_fan");
HASwitch _valveSwitches[8] = {
    HASwitch("greenhouse_valve_1"), HASwitch("greenhouse_valve_2"), HASwitch("greenhouse_valve_3"), HASwitch("greenhouse_valve_4"),
    HASwitch("greenhouse_valve_5"), HASwitch("greenhouse_valve_6"), HASwitch("greenhouse_valve_7"), HASwitch("greenhouse_valve_8")
};
HASwitch _allValves("greenhouse_all_valves");
HASelect _durationSelect("watering_duration");
HASensorNumber _soilSensors[8] = {
    HASensorNumber("greenhouse_soil_1", HASensorNumber::PrecisionP0), HASensorNumber("greenhouse_soil_2", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_3", HASensorNumber::PrecisionP0), HASensorNumber("greenhouse_soil_4", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_5", HASensorNumber::PrecisionP0), HASensorNumber("greenhouse_soil_6", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_7", HASensorNumber::PrecisionP0), HASensorNumber("greenhouse_soil_8", HASensorNumber::PrecisionP0)
};

constexpr int wateringOptions[] = {30, 60, 90, 120, 180};
constexpr size_t numWateringOptions = sizeof(wateringOptions) / sizeof(wateringOptions[0]);

std::function<void(bool)> _pumpCb;
std::function<void(bool)> _fanCb;
std::function<void(uint8_t, bool)> _valveCb;
std::function<void(bool)> _allValvesCb;
std::function<void(int)> _durationCb;

String getWateringOptionsString() {
    String result;
    for (size_t i = 0; i < numWateringOptions; ++i) {
        result += String(wateringOptions[i]);
        if (i < numWateringOptions - 1) result += ";";
    }
    return result;
}

void valveCallback(bool state, HASwitch* sender) {
    for (uint8_t i = 0; i < 8; ++i) {
        if (&_valveSwitches[i] == sender && _valveCb) {
            _valveCb(i, state);
            break;
        }
    }
    sender->setState(state);
}

void begin(IPAddress broker, const char* user, const char* pass,
           const char* deviceName, const char* swVersion, WiFiClient& client) {
    _device.setName(deviceName);
    _device.setSoftwareVersion(swVersion);
    static HAMqtt mqttInstance(client, _device);
    _mqtt = &mqttInstance;

    _pumpSwitch.setName("Pumpe");
    _pumpStatus.setName("Pumpenstatus");
    _pumpStatus.setDeviceClass("running");
    _fanSwitch.setName("Luefter");
    _allValves.setName("Alle Ventile");

    _durationSelect.setName("Dauer");
    _durationSelect.setOptions(getWateringOptionsString().c_str());

    for (int i = 0; i < 8; ++i) {
        char label[32];
        snprintf(label, sizeof(label), "Bodenfeuchte_%d", i + 1);
        _soilSensors[i].setName(label);
        _soilSensors[i].setUnitOfMeasurement("%");
        _soilSensors[i].setDeviceClass("humidity");

        snprintf(label, sizeof(label), "Ventil_%d", i + 1);
        _valveSwitches[i].setName(label);
    }

    _pumpSwitch.onCommand([](bool state, HASwitch* s) {
        if (_pumpCb) _pumpCb(state);
        s->setState(state);
    });

    _fanSwitch.onCommand([](bool state, HASwitch* s) {
        if (_fanCb) _fanCb(state);
        s->setState(state);
    });

    _allValves.onCommand([](bool state, HASwitch* s) {
        if (_allValvesCb) _allValvesCb(state);
        s->setState(state);
    });

    _durationSelect.onCommand([](int8_t idx, HASelect* s) {
        if (idx >= 0 && static_cast<size_t>(idx) < numWateringOptions && _durationCb) {
            _durationCb(wateringOptions[idx]);
        }
    });

    for (int i = 0; i < 8; ++i) {
        _valveSwitches[i].onCommand(valveCallback);
    }

    _mqtt->begin(broker, user, pass);
}

void loop() {
    if (_mqtt) _mqtt->loop();
}

void publishSensors(float soilValues[], size_t count) {
    for (size_t i = 0; i < count && i < 8; ++i) {
        _soilSensors[i].setValue(soilValues[i]);
    }
}

void setPumpState(bool state) {
    _pumpSwitch.setState(state);
    _pumpStatus.setState(state);
}

void setFanState(bool state) {
    _fanSwitch.setState(state);
}

void setValveState(uint8_t index, bool state) {
    if (index < 8) _valveSwitches[index].setState(state);
}

void setAllValvesState(bool state) {
    _allValves.setState(state);
}

void onPumpCommand(std::function<void(bool)> cb) {
    _pumpCb = cb;
}

void onFanCommand(std::function<void(bool)> cb) {
    _fanCb = cb;
}

void onValveCommand(std::function<void(uint8_t, bool)> cb) {
    _valveCb = cb;
}

void onAllValvesCommand(std::function<void(bool)> cb) {
    _allValvesCb = cb;
}

void onDurationSelected(std::function<void(int)> cb) {
    _durationCb = cb;
}

} // namespace GreenhouseHA
