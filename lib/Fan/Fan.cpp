#include <Fan.h>
#include <Arduino.h>


/**
 * @brief Constructor for the Fan class.
 * 
 * @param pin The PWM pin connected to the fan.
 * @param speedPin The pin used to read the fan speed.
 * @param freq The frequency of the PWM signal.
 * @param fanChannel The channel used for PWM control.
 * @param resolution The resolution of the PWM signal.
 */
Fan::Fan(int pin, int speedPin, int freq, int fanChannel, int resolution)
{
  this->_pwmPin = pin;
  this->_speedPin = speedPin;
  this->_freq = freq;
  this->_fanChannel = fanChannel;
  this->_resolution = resolution;
  
  ledcSetup(_fanChannel, _freq, _resolution);
  ledcAttachPin(_pwmPin, _fanChannel);
  pinMode(_speedPin, INPUT);
}


/**
 * Parses the incoming message and updates the fan settings accordingly.
 * 
 * @param message The JSON message to parse.
 */
void Fan::parseMessage(JsonDocument message)
{
  // if message is "roomTemp" then set the room temp. else if the message is fan control the set the fan control
  try {
    if (strcmp(message["message"], "roomTemp") == 0)
    {
      rTemp = message["temperature"];
      
    }
    else if (strcmp(message["message"], "fanControl") == 0)
    {
      manual = strcmp(message["manual"], "on") == 0;
      manSpeed = atoi(message["manualSpeed"]);
      tempOffset = atoi(message["tempOffset"]);
      autoScaleMax = atoi(message["autoScaleMax"]);
    }
  }
  catch(const std::exception& e)
  {
    pubMessage["error"] = true;
    //  error message = invalid message + message name
    pubMessage["errorMessage"] = "Invalid message: ";
    serializeJsonPretty(pubMessage, Serial);
    Serial.println();
  }
}


/**
 * @brief Builds the public message for the fan.
 * 
 * This function populates the `pubMessage` object with various fan parameters such as fan speed, duty cycle, manual mode, manual speed, room temperature, temperature offset, PWM pin, speed pin, and auto scale maximum.
 */
void Fan::buildPubMessage()
{
  pubMessage["fanSpeed"] = _fanSpeed;
  pubMessage["Duty Cycle"] = _dutyCycle;
  pubMessage["Manual"] = manual;
  pubMessage["Manual Speed"] = manSpeed;
  pubMessage["Room Temp"] = rTemp;
  pubMessage["Temp Offset"] = tempOffset;
  pubMessage["Thermostat Temp"] = tTemp;  
  pubMessage["thermostat + offset"] = tempOffset + tTemp;
  pubMessage["PWM Pin"] = _pwmPin;
  pubMessage["Speed Pin"] = _speedPin;
  pubMessage["autoScaleMax"] = autoScaleMax;

}

/**
 * 
 * @brief Checks for errors in the fan speed and duty cycle.
 * 
 * If the fan speed is 0 and the duty cycle is not 0, an error is set and an error message is generated.
 * Otherwise, the error flag is cleared and the error message is set to an empty string.
 */
void Fan::errorCheck()
{
  // check for errors if fan speed is 0 and _dutyCycle is not 0
  if (_fanSpeed == 0 && _dutyCycle != 0)
  {
    _error = true;
    pubMessage["error"] = _error;
    pubMessage["errorMessage"] = "Fan speed is 0 but duty cycle is not 0";
  }
  else
  {
    _error = false;
    pubMessage["error"] = _error;
    pubMessage["errorMessage"] = "";
  }
}

/**
 * @brief Calculates the fan speed in RPM.
 * 
 * This function counts the number of pulses generated by the fan within a 1-second interval,
 * and calculates the fan speed in revolutions per minute (RPM) based on the pulse count.
 * 
 * @note This function assumes that the fan speed is measured using a digital input pin.
 * 
 * @return void
 */
void Fan::getFanSpeed()
{
  long now = millis();
  // count the number of pulses by looking at the transition from low to high and counting the number of transitions
  if (digitalRead(_speedPin) == HIGH && !_lastPulseHigh)
  {
    _speedInPulses++;
    _lastPulseHigh = true;
  } else if (digitalRead(_speedPin) == LOW && _lastPulseHigh)
  {
    _lastPulseHigh = false;
  }
  
  // count the pulses for 1 second then calculate the RPM of the fan and reset the count
  if (now - _lastPulse >= 1000)
  {
    _lastPulse = now;
    _fanSpeed = (_speedInPulses * 60) / 2;
    _speedInPulses = 0;
  }
  
  errorCheck();
}

/**
 * @brief Executes the main loop of the Fan class.
 * 
 * This function is responsible for controlling the fan speed based on the thermostat temperature and room temperature.
 * If the fan is in manual mode, it sets the fan speed based on the manual speed value.
 * If the fan is in automatic mode, it adjusts the fan speed based on the difference between the room temperature and the target temperature.
 * 
 * @param thermostatTemp The current temperature reading from the thermostat.
 */
void Fan::loop(float thermostatTemp)
{
  tTemp = thermostatTemp;

  getFanSpeed();
  if (manual)
  {
    _dutyCycle = map(manSpeed, 0, 100, 0, 255);
    ledcWrite(_fanChannel, _dutyCycle);
  }
  else
  {
    // if roomTemp is greater than targetTemp + tempOffset, map the fan speed to the roomTemp
  
    if (rTemp > tempOffset + tTemp)
    { 
      float diff = rTemp - tTemp;
      
      _dutyCycle = map(diff, 0, autoScaleMax, 0, 255);

    }
    else
    { 
      _dutyCycle = 0;
    }
    _dutyCycle = constrain(_dutyCycle, 0, 255);
    ledcWrite(_fanChannel, _dutyCycle);
  }
  buildPubMessage();
}