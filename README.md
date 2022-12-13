# Balanceboard

A program for the esp32 that broadcasts a mpu6050's rotation data to the connected lan using udp.

The idea was to create a more user friendly and seamless version of the [MPU6050_DMP6_ESPWiFi](https://github.com/ElectronicCats/mpu6050/tree/master/examples/MPU6050_DMP6) example that comes with the standard MPU6050 Arduino library. As this example uses hard coded wifi credentials and hard coded IP addresses to send/receive the data, the program would need to be adjusted and reflashed everytime it would be used in a new network.

As the general ability to wirelessly transmit gyroscope/accelerometer data can open the door to manifold motion based use cases, an easy to use and read example might lower the barrier for other developers to make use of the measured data. 
Attached to a balanceboard, the data measured that way might be used as a way to compete with others or even as an input for games that are to be controlled via the player balancing. 

As a proof of work, I created two minimal exmaples:
* [Python listener](https://github.com/ninedraft/python-udp/blob/master/client.py) (requires changing the port to 50000 to work with this project)
* [Godot listener and visualizer](https://github.com/c0decaps/balanceboard-visualizer) 

![balanceboard_shorter_640×360](https://user-images.githubusercontent.com/20572466/207466992-66e49eb4-1712-4d4a-bd5e-82fa3720f5ee.gif)

## Details

The project uses the [WiFiManager](https://github.com/tzapu/WiFiManager) library to let the user connect to the desired network.
After that is done, it uses the [MPU6050_light](https://github.com/rfetick/MPU6050_light) library to read the rotation angles in degrees and broadcasts that data via UDP to the connected network on port 50000.

To transmit the measured data, the format that is used looks like the following: `x:X.XX,y:Y.YY,z:Z.ZZ` i.e. as comma separated floating points with two decimals and a descriptor of the angle.

Like that, one just needs to listen for UDP packages on that port in order to read the measured data on a client within the same network. 

In case one wants to run the device within a new network, the settings can be reset by pressing and holding the reset button (GPIO 0) and going through the WiFiManager setup again.

## Wiring
### ESP32
![balanceboard_bb_600](https://user-images.githubusercontent.com/20572466/188127178-7c0d1f00-f457-4785-b145-c8609655a0ed.png)
### ESP8266
![balanceboard-esp8266_bb_66](https://user-images.githubusercontent.com/20572466/188127608-50d05c35-5297-40ff-bcfc-4425c4534abf.png)

