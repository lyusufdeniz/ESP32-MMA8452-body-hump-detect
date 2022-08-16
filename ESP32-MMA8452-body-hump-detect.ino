#include "config.h"
#include <Wire.h> // I2C Kütüphanesi
#include <BluetoothSerial.h> //BT Kutuphanesi
#include <MMA8453_n0m1.h> //İvmeölçer Kütüphanesi
#include <StopWatch.h>
MMA8453_n0m1 ivmeolcer; //sensör tanımlanması
StopWatch stopwatch(StopWatch::SECONDS); // kronometre
AdafruitIO_Feed *duruskontrol = io.feed("duruskontrol"); // durum feedi
AdafruitIO_Feed *timer = io.feed("timer"); // süre feedi
BluetoothSerial SerialBT;

void sensorHazirla()
{
  ivmeolcer.setI2CAddr(0x1C); //Sensörün I2C Adresi
  ivmeolcer.dataMode(true, 2); //enable highRes 10bit, 2g range [2g,4g,8g]
}
int current = -1;   //Adafruite gönderilecek değişkenlerin tanımlanması
int last = -2;
int currenttime = 0;
int lasttime = 0;

void sendState()
{
  io.run();

  if (current == last )
  {
    return;
  }
  else
  {
// mevcut durumu adafruite ve bluetooth cihazına yolla
    duruskontrol->save(current);
    char data=current;
    if(current==0)
    {
      SerialBT.write('0');
  
    }
    else
    {
        SerialBT.write('1');
        
    }
    
    last = current;
  }
}
void sendTime()
{
  io.run();

  if (currenttime == lasttime )
  {
    return;
  }
  else
  {
//    Serial.print("sending currenttime -> ");
    Serial.println(currenttime);

    timer->save(currenttime);
    lasttime = currenttime;

    delay(5000);
  }

}



void setup() {
  stopwatch.start();
  // start the serial connection
  Serial.begin(115200);
  sensorHazirla();
  // wait for serial monitor to open
  while (! Serial);

  // connect to io.adafruit.com
//  Serial.print("Connecting to Adafruit IO");
  io.connect();


  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

//  duruskontrol->get();

   SerialBT.begin("ESP32_DurusKontrolCihazi"); //Bluetooth device name
   Serial.println("The device started, now you can pair it with bluetooth!");

}

void loop() {
  ivmeolcer.update();

  if (ivmeolcer.y() >= 250)
  {
//    Serial.print( "Duruşunuz Düzgün!");
    current = 1;
    currenttime = stopwatch.elapsed();
    sendState();
    sendTime();
  }
  else
  {
    stopwatch.reset();
    stopwatch.start();
//    Serial.println( "Duruşunuz Düzgün Degil Dik Durmaya Calisin");
    current = 0;
    currenttime = stopwatch.elapsed();
    sendState();
    sendTime();
  }

}
