#pragma once

#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include <vector>

#include <genericstate.h>

#define PIN 27      // Hier wird angegeben, an welchem digitalen Pin die WS2812 LEDs bzw. NeoPixel angeschlossen sind
#define NUMPIXELS 4 // Hier wird die Anzahl der angeschlossenen WS2812 LEDs bzw. NeoPixel angegeben


typedef struct GradientPalette_t {
    std::vector<std::tuple<int,int,int,int,int,int,const char*>> color_vec=
    {
        //                                              index, red, green, blue, white, brightness, name 
        std::tuple<int,int,int,int,int,int,const char*>{    0, 119,   110,  153,     0,          5, "Rhythm"        },
        std::tuple<int,int,int,int,int,int,const char*>{   35, 181,   114,  142,     5,         15, "Turkish Rose"  },
        std::tuple<int,int,int,int,int,int,const char*>{   70, 218,   127,  125,    10,         18, "New York Pink" },
        std::tuple<int,int,int,int,int,int,const char*>{  110, 235,   181,  138,    20,         22, "Crayola's Gold"},
        std::tuple<int,int,int,int,int,int,const char*>{  145, 244,   215,  151,    30,         27, "Caramel"       },
        std::tuple<int,int,int,int,int,int,const char*>{  210, 255,   242,  189,    50,         50, "Blond"         },
        std::tuple<int,int,int,int,int,int,const char*>{  255, 128,   128,  240,   100,        150, "light_blue"    },
    };
} GradientPalette_t;


class Service : public Adafruit_NeoPixel
{
    public:
        Service(const char* ServiceName, uint32_t TickRate);
        void ServiceInit();
        void Start();
        void Stop();
        static void vTimerCallback(TimerHandle_t TimerHandle);

        void lightOn();
        void lightOff();
        void startSunRise();

        TimerHandle_t gWorkTimer;

        uint32_t counter     = 0;
        uint32_t max_counter = 255;
        uint32_t last_index  = 0;
    private:
        uint32_t gTickRate;
        const char* gServiceName;
        GradientPalette_t GradientPalette;

/**
 * @brief Statemachine
 * 
 */
private:

    /**
     * @brief default unhandelt event function
     * 
     * @param str 
     */
    static void unhandledEvent(const String &str) 
    { 
        Serial.printf("unhandled event in %s\n", str.c_str()); 
    }

    /**
     * @brief generic alarm state
     * 
     */
    struct LightState : public GenericState<Service, LightState> 
    {
        using GenericState::GenericState;
        virtual void lightOn() { unhandledEvent( "LightOn" ); }
        virtual void lightOff(){ unhandledEvent( "LightOff"); }
        virtual void sunRise() { unhandledEvent( "SunRise" ); }

        virtual const char *getLightState(){ unhandledEvent( "LightState"); return "unhandledEvent"; }

    };
    StateRef<LightState> lightState;
    /**
     * @brief LightOn
     * 
     */
    struct LightOn : public LightState 
    {
        using LightState::LightState;
        void entry() 
        { 
            Serial.println("-> entry LightOn");
            stm.fill(stm.Color(255,255,255,255), 0, stm.numPixels());
            stm.show();
            return;
        
        }
        void lightOn() 
        { 
            return; 
        }
        void lightOff() 
        {
            change<LightOff>();
        }
        void sunRise()
        {
            change<SunRise>();
        }
        const char *getLightState()
        {
            return "LightOn";
        }
        void exit() 
        { 
            Serial.println("<- exit LightOn");
            return;
        }
    };
    /**
     * @brief LightOff
     * 
     */
    struct LightOff : public LightState 
    {
        using LightState::LightState;
        void entry() 
        { 
            Serial.println("-> entry LightOff");
            stm.fill(stm.Color(0,0,0,0), 0, stm.numPixels());
            stm.show();
            return; 
        }
        void lightOff() 
        { 
            return; 
        }
        void lightOn() 
        {
            change<LightOn>();
            return;
        }
        void sunRise()
        {
            change<SunRise>();
        }
        const char *getLightState()
        {
            return "LightOff";
        }
        void exit() 
        { 
            Serial.println("<- exit LightOff");
            return;
        }
    };
    /**
     * @brief SunRise
     * 
     */
    struct SunRise : public LightState 
    {
        using LightState::LightState;
        void entry() 
        { 
            Serial.println("-> entry SunRise");
            stm.last_index = 0;
            stm.counter    = 0;
            stm.Start();
            Serial.println("-> entry done");
            return; 
        }
        void lightOff() 
        { 
            return; 
        }
        void lightOn() 
        {
            change<LightOn>();
            return;
        }
        void sunRise()
        {
            return;
        }
        const char *getLightState()
        {
            return "SunRise";
        }
        void exit() 
        { 
            Serial.println("<- exit SunRise");
            stm.Stop();
            return;
        }
    };

};