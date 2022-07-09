#  Baro alti project

Ok so the idea is pretty simple : build a simple altimeter for paramotoring/paragliding.

Most of off the shelf stuff cost way too much for what it is and often comes with features I won't ever use.

So I decided to build on from a esp32 devkit, a barometer sensor and a battery.

After few tries with some WeMos D1 mini Pro – ESP8266 I had laying around and a oled screen shield, I decided to move to a more convinient heltec wifi-kit-32 

![heltec](https://user-images.githubusercontent.com/1192256/178111970-3f657b75-b228-42df-8bed-2034303d7a40.png)

which already have a (even bigger) oled screen, and underneith is a lipo plug that will handle charge for 1s packs.

The code is simple : It will used the pressure recorded at power on time as its base level (0m and 0 feet)

Then it will do its thing, every second, it will give you the new relative altitude.

## The barometer 
I used the **Adafruit BMP388** the precision is about 30cm !

![image](https://user-images.githubusercontent.com/1192256/178112329-c6ac9e52-88cd-4eee-907c-6df79c46e3c2.png)

## The battery
Is tiny, it's an old **1s lipo 300mah** used in tiny quadcopter. 

The battery life of the device is expected to be between 7 and 10 hours.

## The box
I used the box that the heltec wifi-kit-32 came with, I 3d printed a piece to organize components a bam ! We have an altimeter :D

![image](https://user-images.githubusercontent.com/1192256/178112479-63ccc113-d100-4736-84e0-078552f08c99.png)

![image](https://user-images.githubusercontent.com/1192256/178112380-9e4f9c98-29b9-4080-bb94-8adac8fe2bbc.png)

On the back of the box, I hotglued some scratch tape

![image](https://user-images.githubusercontent.com/1192256/178112562-6fd11925-1b35-4c68-add4-f85fa307b109.png)

And from now on I can put the device on my cockpit !


![Untitled](https://user-images.githubusercontent.com/1192256/178113171-dea33538-47c8-4e6c-912d-f334357cbd84.png)
