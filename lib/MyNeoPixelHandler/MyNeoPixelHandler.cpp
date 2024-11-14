#include <MyNeoPixelHandler.h>


MyNeoPixelHandler::MyNeoPixelHandler( uint16_t n
                                    , int16_t pin = 6
                                    , neoPixelType type = NEO_GRBW + NEO_KHZ800
                                    , const char * const ServiceName = "MyNeoPixelHandler"
                                    , uint32_t TickRate = 1000 )
    : Adafruit_NeoPixel( n, pin, type)
{
    Serial.printf("*** MyNeoPixelHandler::MyNeoPixelHandler( %i, %i, %i)\n", n, pin, type);
    Adafruit_NeoPixel::begin();
    LightState::init<LightOff>(*this, lightState);

    gServiceName = ServiceName;

}

MyNeoPixelHandler::~MyNeoPixelHandler()
{
}

void MyNeoPixelHandler::lightOn()
{  
    this->lightState->lightOn();
}

void MyNeoPixelHandler::lightOff()
{ 
    this->lightState->lightOff(); 
}

void MyNeoPixelHandler::startSunRise()
{
    this->lightState->sunRise();
}

void MyNeoPixelHandler::begin()
{
    Serial.println("***  MyNeoPixelHandler::begin() ***");
    /* -------------------------------------------------- */
    /*  1 Sekunde entspricht 1.000 Millisekunden          */
    uint32_t ul_delay = this->i_wakeDelayInMinutes*60*(1000-this->i_loopDealy)/this->i_maxSunPhase;

    Serial.printf("ul_delay [%i]\n", ul_delay);

    this->gTickRate = ul_delay;

    Serial.printf("-> create timer [%s]\n", this->gServiceName);

    this->gWorkTimer = xTimerCreate( this->gServiceName.c_str()
                                   , pdMS_TO_TICKS(this->gTickRate)
                                   , pdTRUE
                                   , this
                                   , &vTimerCallback);

    Serial.println("-> timer created");

}

const char *MyNeoPixelHandler::getLightState()
{
    return lightState->getLightState();
}


uint32_t MyNeoPixelHandler::calcColorValues(uint16_t i_Headindex)
{
    Serial.printf("*** calcColorValues(uint16_t i_Headindex[%i]) ***\n", i_Headindex);
    uint8_t index = 0;
    
    for(auto & elem : GradientPalette.color_vec )
    {
        if( std::get<0>(elem) == i_Headindex  )
        {
            this->myColor.start = index;
            this->myColor.ende  = index+1;
            break;
        }
        else if( std::get<0>(elem) > i_Headindex  )
        {
            break;
        }
        else
        {
            index++;
        }
    }    

    Serial.printf("this->myColor.start[%i]\n", this->myColor.start );
    Serial.printf("this->myColor.ende [%i]\n", this->myColor.ende  );

    auto s_element = GradientPalette.color_vec[this->myColor.start];
    auto e_element = GradientPalette.color_vec[this->myColor.ende];

    Serial.printf( "s_element[%i -> %i %i %i %i %i %s]\n", std::get<0>(s_element), std::get<1>(s_element), std::get<2>(s_element), std::get<3>(s_element), std::get<4>(s_element), std::get<5>(s_element), std::get<6>(s_element)  );
    Serial.printf( "e_element[%i -> %i %i %i %i %i %s]\n", std::get<0>(e_element), std::get<1>(e_element), std::get<2>(e_element), std::get<3>(e_element), std::get<4>(e_element), std::get<5>(e_element), std::get<6>(e_element)  );

    this->myColor.iRed        = map(i_Headindex, 0, this->i_maxSunPhase, std::get<1>(s_element), std::get<1>(e_element));
    this->myColor.iGreen      = map(i_Headindex, 0, this->i_maxSunPhase, std::get<2>(s_element), std::get<2>(e_element));
    this->myColor.iBlue       = map(i_Headindex, 0, this->i_maxSunPhase, std::get<3>(s_element), std::get<3>(e_element));
    this->myColor.iWhite      = map(i_Headindex, 0, this->i_maxSunPhase, std::get<4>(s_element), std::get<4>(e_element));
    this->myColor.iBrightness = map(i_Headindex, 0, this->i_maxSunPhase, std::get<5>(s_element), std::get<5>(e_element));


    uint32_t i_color = this->Color(this->myColor.iRed, this->myColor.iGreen, this->myColor.iBlue, this->myColor.iWhite);

    Serial.printf("r[%i] g[%i] b[%i] w[%i] br[%i] color[%i]\n", this->myColor.iRed, this->myColor.iGreen, this->myColor.iBlue, this->myColor.iWhite, this->myColor.iBrightness, i_color );

    delay(2000);

    return i_color;
}

void MyNeoPixelHandler::letSunRise()
{
    Serial.println("*** letSunsRise() ***");

    Serial.printf("i_actualSunPhase: %i\n", this->i_actualSunPhase);

    this->i_actualSunPhase = this->i_actualSunPhase + 1;

    Serial.printf("this->i_actualSunPhase[%i]\n", this->i_actualSunPhase);
    Serial.printf("this->i_maxSunPhase   [%i]\n", this->i_maxSunPhase);

    uint16_t iHeatindex= map(this->i_actualSunPhase, 0, this->i_maxSunPhase, 0, 255); 

    Serial.printf("iHeatindex[%i]\n", iHeatindex);

    this->fill(this->calcColorValues(iHeatindex), 0, this->numPixels());
    
    this->setBrightness( this->myColor.iBrightness );
    this->show();

    if(this->i_actualSunPhase>=this->i_maxSunPhase)
    {
        this->Stop();
        this->i_actualSunPhase = 0;
        this->lightState->lightOn();
    }
}



void MyNeoPixelHandler::listener(const char *msg_)
{
    /* -------------------------------------------------- */
    /* light on                                           */
    if( (strcmp("LightOn", msg_ ) == 0))  
    {
        this->lightOn();
    }
    /* -------------------------------------------------- */
    /* light off                                          */
    if( (strcmp("LightOff", msg_ ) == 0))  
    {
        this->lightOff();
    }
    /* -------------------------------------------------- */
    /* sunrise                                            */
    if( (strcmp("SunRise", msg_ ) == 0))  
    {
        this->startSunRise();
    }
    return;
} 

void MyNeoPixelHandler::testColorPalette()
{
    Serial.println("*** MyNeoPixelHandler::testColorPalette() ***");
    GradientPalette_t GradientPalette;
    for(auto & elem : GradientPalette.color_vec )
    {
        this->myColor.iRed        = std::get<1>(elem);
        this->myColor.iGreen      = std::get<2>(elem);
        this->myColor.iBlue       = std::get<3>(elem);
        this->myColor.iWhite      = std::get<4>(elem);
        this->myColor.iBrightness = std::get<5>(elem);

        
        this->fill(this->Color(this->myColor.iRed, this->myColor.iGreen, this->myColor.iBlue, this->myColor.iWhite), 0, this->numPixels());
        this->setBrightness( this->myColor.iBrightness );
        this->show();
        delay(3000);
    }
}

void MyNeoPixelHandler::Start()
{
    Serial.printf("Start %s\n", this->gServiceName);
    if( this->gWorkTimer != NULL) 
    {
        // Start the timer
        xTimerStart( this->gWorkTimer
                   , 0);
    }
}

void MyNeoPixelHandler::Stop()
{
    Serial.printf("Stop %s\n", this->gServiceName);
    xTimerStop(this->gWorkTimer,0);

}

void MyNeoPixelHandler::vTimerCallback(TimerHandle_t TimerHandle)
{
    Serial.println("*** vTimerCallback() ***");
    MyNeoPixelHandler *service = static_cast<MyNeoPixelHandler *>(pvTimerGetTimerID(TimerHandle));

    Serial.printf("-> %s\n", service->gServiceName);
    service->letSunRise();
}
