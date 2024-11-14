#include <Arduino.h>

#include "Service.h"
//#include <MyNeoPixelHandler.h>

Service* s = new Service("dummy", 2000);
Service s1("dummy1", 2000);



void setup() 
{
  Serial.begin(115200);
  Serial.println("setup");

//  MyNeoPixelHandler pixels(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800, "pixels", 1000); 

//  s->ServiceInit();
//  s->Start();
//  delay(2000);
//  s->Stop();
//
//
  s1.ServiceInit();
  delay(100);
  s1.startSunRise();

  //delay(2000);
  //s1.Stop();


//  pixels.begin();
//  pixels.Start();


  Serial.println("setup ende");
}

void loop() 
{
}
