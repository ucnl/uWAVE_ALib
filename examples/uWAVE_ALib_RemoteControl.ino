#include <SoftwareSerial.h>
#include "uWAVE_ALib.h"

/*
 * For compiling this sketch uses uWAVE_ALib library, that can be found on github:
 * https://github.com/ucnl/uWAVE_ALib
 * 
 * This sketch is intended to connect the user control commands (RC_USR_CMD_000 ... RC_USR_CMD_008) received 
 * via the uWave modem and the pins of the Arduino board. When a user command is received, the corresponding 
 * pin on the Arduino board for the time USR_CMD_PIN_ACTIVE_DUR milliseconds goes from USR_CMD_PIN_INACTIVE 
 * to USR_CMD_PIN_ACTIVE. After that, the pin automatically goes back to the USR_CMD_PIN_INACTIVE state.
 * 
 * Every user control command corresponds to a pin via defines uWAVE_USR_CMD_0_PIN ... uWAVE_USR_CMD_8_PIN
 * Default inactive state defines via USR_CMD_PIN_INACTIVE and active state defines via USR_CMD_PIN_ACTIVE
 * 
 * For more information on uWave user command, please refer: 
 * https://docs.unavlab.com/documentation/EN/uWAVE/uWAVE_Protocol_Specification_en.html#42-remote-commands 
 * 
 * 
 * Hardware list:
 *  
 *    1. Arduino UNO/Nano compatible board - 1 pcs
 *    2. uWAVE modem - at least 1 pcs
 *      
 *  Both Arduino and uWave modem are powered from PC's usb port.
 *  
 *  uWAVE's cable cores should be soldered to the Arduino as follows:
 *  
 *  uWAVE Core     | Arduino Nano Pin
 *  *********************************           
 *  GND            | GND
 *  VCC            | VIN
 *  TX             | D2
 *  RX             | D3
 *  CMD            | D4
 *  
 *  Arduino pins uWAVE_USR_CMD_0_PIN ... uWAVE_USR_CMD_8_PIN can be used to drive LEDs for example. 
 *  In that case you will need to use current liniting 200-300 Ohm resistor per each LED. 
 *  
 *  Instead of LEDs you can drive a relays etc.
 */



// Configuration of this sketch ------------------------------------------------------------------------------------

#define USE_HW_SERIAL_DBG

#define LOCAL_MODEM_TX_ID  (0)
#define LOCAL_MODEM_RX_ID  (1)

#define SALINITY_PSU       (0)
#define GRAVITY_ACC        (9.81)

#define USR_CMD_PIN_INACTIVE           (LOW)
#define USR_CMD_PIN_ACTIVE             (HIGH)
#define USR_CMD_PIN_ACTIVE_DURATION_MS (500)

// Configuration of modem's connection ----------------------------------------------------------------------------
#define uWAVE_RX_PIN  (3)
#define uWAVE_TX_PIN  (2)
#define uWAVE_CMD_PIN (4)

#define uWAVE_USR_CMD_0_PIN (5)
#define uWAVE_USR_CMD_1_PIN (6)
#define uWAVE_USR_CMD_2_PIN (7)
#define uWAVE_USR_CMD_3_PIN (8)
#define uWAVE_USR_CMD_4_PIN (9)
#define uWAVE_USR_CMD_5_PIN (10)
#define uWAVE_USR_CMD_6_PIN (11)
#define uWAVE_USR_CMD_7_PIN (12)
#define uWAVE_USR_CMD_8_PIN (13)

// Variables ------------------------------------------------------------------------------------------------------
#define USR_CMD_PINS_NUMBER (9)
byte usr_cmd_pins[USR_CMD_PINS_NUMBER] { uWAVE_USR_CMD_0_PIN, uWAVE_USR_CMD_1_PIN, 
                                         uWAVE_USR_CMD_2_PIN, uWAVE_USR_CMD_3_PIN, 
                                         uWAVE_USR_CMD_4_PIN, uWAVE_USR_CMD_5_PIN, 
                                         uWAVE_USR_CMD_6_PIN, uWAVE_USR_CMD_7_PIN,
                                         uWAVE_USR_CMD_8_PIN };
                                         
byte usr_cmd_pin_states[USR_CMD_PINS_NUMBER];
long usr_cmd_pin_ts[USR_CMD_PINS_NUMBER];
long ts;


SoftwareSerial uWavePort(uWAVE_TX_PIN, uWAVE_RX_PIN);

uWAVE uWrapper(&uWavePort, uWAVE_CMD_PIN);
uWAVE_EVENT_Enum result;


void set_usr_cmd_pin(int idx)
{
  usr_cmd_pin_states[idx] = USR_CMD_PIN_ACTIVE;
  digitalWrite(usr_cmd_pins[idx], USR_CMD_PIN_ACTIVE);
  usr_cmd_pin_ts[idx] = millis();

#ifdef USE_HW_SERIAL_DBG

  Serial.print(F("Switching pin "));
  Serial.print(usr_cmd_pins[idx]);
  Serial.print(F(" to "));
  Serial.println(USR_CMD_PIN_ACTIVE);

#endif
}

void unset_all_usr_cmd_pins()
{
  for (int i = 0; i < USR_CMD_PINS_NUMBER; i++)
  {
    usr_cmd_pin_states[i] = USR_CMD_PIN_INACTIVE;
    digitalWrite(usr_cmd_pins[i], USR_CMD_PIN_INACTIVE);
  }
}

void process_usr_cmd_pins()
{
  ts = millis();
  
  for (int i = 0; i < USR_CMD_PINS_NUMBER; i++)
  {
    if (usr_cmd_pin_states[i] != USR_CMD_PIN_INACTIVE)
    {
      if (ts - usr_cmd_pin_ts[i] >= USR_CMD_PIN_ACTIVE_DURATION_MS)
      {
        usr_cmd_pin_states[i] = USR_CMD_PIN_INACTIVE;
        digitalWrite(usr_cmd_pins[i], USR_CMD_PIN_INACTIVE);

        
#ifdef USE_HW_SERIAL_DBG

        Serial.print(F("Switching pin "));
        Serial.print(usr_cmd_pins[i]);
        Serial.print(F(" to "));
        Serial.println(USR_CMD_PIN_INACTIVE);

#endif        
      }
    }
  }
}

void setup() 
{
  pinMode(uWAVE_USR_CMD_0_PIN, OUTPUT);
  pinMode(uWAVE_USR_CMD_1_PIN, OUTPUT);
  pinMode(uWAVE_USR_CMD_2_PIN, OUTPUT);
  pinMode(uWAVE_USR_CMD_3_PIN, OUTPUT);
  pinMode(uWAVE_USR_CMD_4_PIN, OUTPUT);
  pinMode(uWAVE_USR_CMD_5_PIN, OUTPUT);
  pinMode(uWAVE_USR_CMD_6_PIN, OUTPUT);
  pinMode(uWAVE_USR_CMD_7_PIN, OUTPUT);
  pinMode(uWAVE_USR_CMD_8_PIN, OUTPUT);

  unset_all_usr_cmd_pins();

#ifdef USE_HW_SERIAL_DBG
    
  Serial.begin(9600);    // Start hardware serial

#endif

  uWavePort.begin(9600); // Start software serial 
  uWrapper.enable();     // set the CMD pin to a High level (Command mode on)

  delay(1000);
}

void loop() 
{
  result = uWrapper.process(); // This function should be called as frequent as possible

  if (result != uWAVE_NONE)    // If something is happend in the uWrapper...
  {
    if (result & uWAVE_ASYNC_IN_RECEIVED) // If we have received a asynchronous incoming remote command (a USR_CMD_000 ... 008)
    {
      uWAVE_RC_CODES_Enum async_rc = uWrapper.getRem_async_in_rc();     // read the command ID

#ifdef USE_HW_SERIAL_DBG

      Serial.print(F("RC_ASYNC_IN, code="));
      Serial.println(async_rc);

#endif
      
      if ((async_rc >= RC_USR_CMD_000) && (async_rc <= RC_USR_CMD_008)) // quite a redundant check, but, anyway
        set_usr_cmd_pin(async_rc - RC_USR_CMD_000);                     // set the correspondent pin to 'ACTIVE' state            
    }
    
    if (result & uWAVE_LOCAL_TIMEOUT)
    {

#ifdef USE_HW_SERIAL_DBG

      Serial.println(F("Local timeout! Check connection"));

#endif
    }
  }
  else
  {
    if (!uWrapper.isWaitingLocal())     // make sure that we do not waiting an answer from our local modem
    {
      if (!uWrapper.deviceInfoUpdated())
      {
         uWrapper.queryForDeviceInfo();      
      }
      else if ((uWrapper.getTxChID() != LOCAL_MODEM_TX_ID) || // if current modem settings were not checked or they are not correct
               (uWrapper.getRxChID() != LOCAL_MODEM_RX_ID))
      {
        uWrapper.queryForSettingsUpdate(LOCAL_MODEM_TX_ID, LOCAL_MODEM_RX_ID, SALINITY_PSU, false, false, GRAVITY_ACC); // apply new settings (Tx and Rx IDs)        
      }
    }
  }

  process_usr_cmd_pins(); // if any pin has been set to 'ACTIVE' state for more than USR_CMD_PIN_ACTIVE_DURATION_MS, set it to 'INACTIVE' state

}
