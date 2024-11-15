#include <Arduino.h>

#include "Service.h"

Service* s = new Service("dummy", 2000);
Service s1("dummy1", 2000);



void setup() 
{
  Serial.begin(115200);
  Serial.println("setup");

  s1.ServiceInit();
  delay(100);
  s1.startSunRise();

  Serial.println("setup ende");
}

void loop() 
{
}
