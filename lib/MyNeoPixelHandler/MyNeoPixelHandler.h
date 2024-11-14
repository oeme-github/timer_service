#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#pragma once

// SK6812 RGBW LED auf Mini-Platine mit Anschluss-Pads SK6812_RGBW_1pix (Kompatibel zum WS2812B)

#define PIN 27      // Hier wird angegeben, an welchem digitalen Pin die WS2812 LEDs bzw. NeoPixel angeschlossen sind
#define NUMPIXELS 4 // Hier wird die Anzahl der angeschlossenen WS2812 LEDs bzw. NeoPixel angegeben

#include <stdio.h>
#include <iostream>

#include <genericstate.h>
#include <vector>


typedef struct GradientPalette_t {
    std::vector<std::tuple<int,int,int,int,int,int,const char*>> color_vec=
    {
        //                                              index, red, green, blue, white, brightness, name 
        std::tuple<int,int,int,int,int,int,const char*>{    0, 119,   110,  153,     5,          5, "Rhythm"        },
        std::tuple<int,int,int,int,int,int,const char*>{   35, 181,   114,  142,    15,         15, "Turkish Rose"  },
        std::tuple<int,int,int,int,int,int,const char*>{   70, 218,   127,  125,    20,         20, "New York Pink" },
        std::tuple<int,int,int,int,int,int,const char*>{  110, 235,   181,  138,    30,         25, "Crayola's Gold"},
        std::tuple<int,int,int,int,int,int,const char*>{  145, 244,   215,  151,    45,         40, "Caramel"       },
        std::tuple<int,int,int,int,int,int,const char*>{  210, 255,   242,  189,    80,        100, "Blond"         },
        std::tuple<int,int,int,int,int,int,const char*>{  255, 128,   128,  240,   100,        255, "light_blue"    },
    };
} GradientPalette_t;

class MyNeoPixelHandler: public Adafruit_NeoPixel
{
private:
    /* data */

    int16_t i_actualSunPhase     = 0;
    int16_t i_maxSunPhase        = 500;
    int16_t i_wakeDelayInMinutes = 20;
    int16_t i_loopDealy          = 100;
    int16_t i_maxBrightness      = 255;

    int16_t iRed   = 255;
    int16_t iGreen = 240;
    int16_t iBlue  = 240;
    int16_t iWhite = 100;

    struct MyColor{
        int8_t start;
        int8_t ende;

        int32_t iRed;
        int32_t iGreen;
        int32_t iBlue;
        int32_t iWhite;
        int32_t iBrightness;
    } myColor;

    uint32_t gTickRate;
    String   gServiceName;

    GradientPalette_t GradientPalette;
    
public:

    TimerHandle_t gWorkTimer;

    MyNeoPixelHandler( uint16_t n, int16_t p, neoPixelType t, const char * const ServiceName, uint32_t TickRate );
    ~MyNeoPixelHandler();

    void listener(const char *msg_);

    void lightOn();
    void lightOff();
    
    void letSunRise();
    void startSunRise();

    void begin();
    
    const char *getLightState();

    uint32_t calcColorValues(uint16_t i_Brightness);

    void testColorPalette();

    void Start();
    void Stop();
    static void vTimerCallback(TimerHandle_t TimerHandle);

    uint16_t getI_actualSunPhase(){ return this->i_actualSunPhase;}

/**
 * @brief Statemachine for alarm
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
    struct LightState : public GenericState<MyNeoPixelHandler, LightState> 
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
            //stm.Start(0);
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
            return;
        }
    };

};

