
#include "Service.h"



Service::Service(const char* ServiceName, uint32_t TickRate)
: Adafruit_NeoPixel( NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800)
{
    this->gTickRate    = TickRate;
    this->gServiceName = ServiceName;


    Adafruit_NeoPixel::begin();
}

void Service::ServiceInit()
{
    Serial.printf("init %s\n", this->gServiceName);

    LightState::init<LightOff>(*this, lightState);
    
    this->gWorkTimer = xTimerCreate( "Test" //this->gServiceName
                                   , pdMS_TO_TICKS(this->gTickRate)
                                   , pdTRUE
                                   , this
                                   , &vTimerCallback);
    Serial.println("Test xxx");
}

void Service::Start()
{
    Serial.printf("Start %s\n", this->gServiceName);
    if( this->gWorkTimer != NULL) 
    {
        // Start the timer
        xTimerStart( this->gWorkTimer
                   , 0);
    }
}


void Service::lightOn()
{  
    this->lightState->lightOn();
}

void Service::lightOff()
{ 
    this->lightState->lightOff(); 
}

void Service::startSunRise()
{
    Serial.println("*** startSunRise() ***");
    this->lightState->sunRise();
}


void Service::Stop()
{
    Serial.printf("Stop %s\n", this->gServiceName);
    xTimerStop(this->gWorkTimer,0);

}

void Service::vTimerCallback(TimerHandle_t TimerHandle)
{
    uint32_t index=0;
    Service *service = static_cast<Service *>(pvTimerGetTimerID(TimerHandle));

    Serial.println("\n*** vTimerCallback ***");
    Serial.printf("service->counter[%i]\n", service->counter );

    for( index = service->last_index; index < service->GradientPalette.color_vec.size(); index++ )
    {
        auto elem = service->GradientPalette.color_vec[index];
        Serial.printf("element [%i]\n", std::get<0>(elem) );

        if( std::get<0>(elem) == service->counter )
        {
            Serial.println("elemt == service-counter");
            break;
        }
        else if( std::get<0>(elem) > service->counter )
        {
            index--;
            service->last_index = index;
            break;
        }
    }    

    Serial.printf("index   [%i] size[%i]\n", index, service->GradientPalette.color_vec.size() );

    uint32_t index2 = 0;
    if( index+1 >= service->GradientPalette.color_vec.size() )
        index2 = index;
    else
        index2 = index+1;
    auto s_element = service->GradientPalette.color_vec[index];
    auto e_element = service->GradientPalette.color_vec[index2];

    Serial.printf( "s_element[%i -> %i %i %i %i %i %s]\n", std::get<0>(s_element), std::get<1>(s_element), std::get<2>(s_element), std::get<3>(s_element), std::get<4>(s_element), std::get<5>(s_element), std::get<6>(s_element)  );
    Serial.printf( "e_element[%i -> %i %i %i %i %i %s]\n", std::get<0>(e_element), std::get<1>(e_element), std::get<2>(e_element), std::get<3>(e_element), std::get<4>(e_element), std::get<5>(e_element), std::get<6>(e_element)  );

    uint32_t iRed        = map(service->counter, 0, service->max_counter, std::get<1>(s_element), std::get<1>(e_element));
    uint32_t iGreen      = map(service->counter, 0, service->max_counter, std::get<2>(s_element), std::get<2>(e_element));
    uint32_t iBlue       = map(service->counter, 0, service->max_counter, std::get<3>(s_element), std::get<3>(e_element));
    uint32_t iWhite      = map(service->counter, 0, service->max_counter, std::get<4>(s_element), std::get<4>(e_element));
    uint32_t iBrightness = map(service->counter, 0, service->max_counter, std::get<5>(s_element), std::get<5>(e_element));

    uint32_t i_color = Color(iRed, iGreen, iBlue, iWhite);

    Serial.printf("r[%i] g[%i] b[%i] w[%i] br[%i] color[%i]\n", iRed, iGreen, iBlue, iWhite, iBrightness, i_color );

    service->fill(Color(iRed,iGreen,iBlue,iWhite),0,NUMPIXELS);
    service->setBrightness(iBrightness);
    service->show();

    if( service->counter == service->max_counter)
    {
        //service->Stop();
        //service->last_index = 0;
        //service->counter    = 0;
        service->lightState->lightOn();
    }
    else
        service->counter++;

}
