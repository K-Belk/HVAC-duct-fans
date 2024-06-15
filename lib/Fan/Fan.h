#ifndef Fan_h
#define Fan_h
#include <ArduinoJson.h>

class Fan
/**
 * @class Fan
 * @brief Represents a fan with various properties and functionalities.
 */
{
public:
  bool manual = false;     /**< Indicates whether the fan is in manual mode. */
  int manSpeed = 0;        /**< The manual speed of the fan. */
  float rTemp = 0;         /**< The room temperature. */
  float tTemp = 0;         /**< The thermostat temperature. */
  float tempOffset = 0;    /**< The temperature offset. */
  int autoScaleMax = 10;   /**< The maximum auto scale value. */
  bool debug = false;      /**< Indicates whether debug mode is enabled. */
  JsonDocument pubMessage; /**< The MQTT message in JSON format. */

  /**
   * @brief Constructs a Fan object with the specified parameters.
   * @param pin The pin number for the fan.
   * @param speedPin The pin number for the fan speed control.
   * @param freq The frequency for the PWM signal.
   * @param fanChannel The channel for the fan.
   * @param resolution The resolution for the PWM signal.
   */
  Fan(int pin, int speedPin, int freq, int fanChannel, int resolution);

  /**
   * @brief Parses the MQTT message.
   * @param message The MQTT message to be parsed.
   */
  void parseMessage(JsonDocument message);

  /**
   * @brief Builds the MQTT message.
   */
  void buildPubMessage();

  /**
   * @brief Gets the fan speed.
   */
  void getFanSpeed();

  /**
   * @brief Checks for errors.
   */
  void errorCheck();

  /**
   * @brief The main loop for the fan.
   * @param thermostatTemp The temperature from the thermostat.
   */
  void loop(float thermostatTemp);

private:
  int _pwmPin;                 /**< The pin number for the PWM signal. */
  int _speedPin;               /**< The pin number for the fan speed control. */
  bool _error = false;         /**< Indicates whether an error occurred. */
  float _fanSpeed = 0;         /**< The current fan speed. */
  int _dutyCycle = 0;          /**< The duty cycle for the PWM signal. */
  int _freq;                   /**< The frequency for the PWM signal. */
  int _fanChannel;             /**< The channel for the fan. */
  int _resolution;             /**< The resolution for the PWM signal. */
  int _speedInPulses = 0;      /**< The speed of the fan in pulses. */
  long _lastPulse = 0;         /**< The timestamp of the last pulse. */
  bool _lastPulseHigh = false; /**< Indicates whether the last pulse was high. */
};

#endif