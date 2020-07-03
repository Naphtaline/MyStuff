/****************************************************************************************************************************
 * ESP8266FastTimerInterrupt.h
 * For ESP8266 boards
 * Written by Khoi Hoang
 * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP8266_ISR_Servo
 * Licensed under MIT license
 * Version: 1.0.2
 *
 * The ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
 * The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
 * The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
 * Using 256 prescaler, maximum timer1 interval is only 26.843542 seconds !!!
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      04/12/2019 Initial coding
 *  1.0.1   K Hoang      05/12/2019 Add more features getPosition and getPulseWidth. Optimize.
 *  1.0.2   K Hoang      20/12/2019 Add more Blynk examples.Change example names to avoid duplication.   
 *****************************************************************************************************************************/

#ifndef ESP8266FastTimerInterrupt_h
#define ESP8266FastTimerInterrupt_h

#ifndef TIMER_INTERRUPT_DEBUG
#define TIMER_INTERRUPT_DEBUG      1
#endif

#ifndef ESP8266
#error This code is designed to run on ESP8266 platform, not Arduino nor ESP32! Please check your Tools->Board setting.
#endif

/* From /arduino-1.8.10/hardware/esp8266com/esp8266/cores/esp8266/esp8266_peri.h

#define ESP8266_REG(addr) *((volatile uint32_t *)(0x60000000+(addr)))
#define ESP8266_DREG(addr) *((volatile uint32_t *)(0x3FF00000+(addr)))
#define ESP8266_CLOCK 80000000UL

//CPU Register
#define CPU2X     ESP8266_DREG(0x14) //when bit 0 is set, F_CPU = 160MHz
*/

/* From /arduino-1.8.10/hardware/esp8266com/esp8266/cores/esp8266/Arduino.h

//timer dividers
enum TIM_DIV_ENUM {
  TIM_DIV1 = 0,   // 80 / 160 MHz (80 / 160 ticks/us - 104857.588 us max)
  TIM_DIV16 = 1,  // 5  / 10  MHz (5  /  10 ticks/us - 1677721.4 us max)
  TIM_DIV256 = 3  // 312.5 / 625 Khz (1 tick = 3.2 / 1.6 us - 26843542.4 us max)
};

//timer int_types
#define TIM_EDGE	0
#define TIM_LEVEL	1
//timer reload values
#define TIM_SINGLE	0 //on interrupt routine you need to write a new value to start the timer again
#define TIM_LOOP	1 //on interrupt the counter will start with the same value again

*/

// ESP8266 only has one usable timer1, max count is only 8,388,607. So to get longer time, we use max available 256 divider
class ESP8266TimerInterrupt;

typedef ESP8266TimerInterrupt ESP8266Timer;

#define MAX_ESP8266_NUM_TIMERS      1
#define MAX_ESP8266_COUNT           8388607

typedef void (*timer_callback)  (void);


class ESP8266TimerInterrupt
{
  private:   
    timer_callback  _callback;        // pointer to the callback function
    float           _frequency;       // Timer frequency
    uint32_t        _timerCount;      // count to activate timer
              
  public:

  ESP8266TimerInterrupt() 
  { 
    _frequency  = 0;
    _timerCount = 0;
    _callback   = NULL;    
  };
  
  // frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely 
  // No params and duration now. To be addes in the future by adding similar functions here or to esp32-hal-timer.c
  bool setFrequency(float frequency, timer_callback callback)
  {
    bool isOKFlag = true;
    
    // ESP8266 only has one usable timer1, max count is only 8,388,607. So to get longer time, we use 16 divider to get 10us
    //_frequency  = 80000000 / 256;
    _frequency  = 80000000 / 16;
    _timerCount = (uint32_t) _frequency / frequency;
    _callback   = callback;
    
    if( _timerCount > MAX_ESP8266_COUNT)
    {
      _timerCount = MAX_ESP8266_COUNT;
      // Flag error
      isOKFlag = false;
    }
    
    // count up
    #if (TIMER_INTERRUPT_DEBUG > 0)
    Serial.println("ESP8266TimerInterrupt: _fre = " + String(_frequency) + ", _count = " + String(_timerCount));
    #endif

    // Clock to timer (prescaler) is always 80MHz, even F_CPU is 160 MHz
    
    timer1_attachInterrupt(callback);
    
    timer1_write(_timerCount);
    
    // Interrupt on EGDE, autoloop
    //timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
       
    return isOKFlag;
  }
  
  // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
  // No params and duration now. To be addes in the future by adding similar functions here or to esp32-hal-timer.c
  bool attachInterruptInterval(unsigned long interval, timer_callback callback)
  {
     return setFrequency( (float) ( 1000000.0f / interval), callback);
  }

  void detachInterrupt()
  {
    timer1_disable();
  }

  // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
  void reattachInterrupt()
  {
    if ( (_frequency != 0) && (_timerCount != 0) && (_callback != NULL) )
      setFrequency(_frequency, _callback);
  }
}; // class ESP8266TimerInterrupt

#endif      //#ifndef ESP8266TimerInterrupt_h
