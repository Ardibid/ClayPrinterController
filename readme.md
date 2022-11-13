# Clay 3D Printer Extruder Controlelr Board
by [Ardavan Bidgoli](https://github.com/Ardibid)
An Arduino sketch to control a clay extruder for robotic 3d printing.

---
## Pin Setup

* **D2**: Load button: When pushed, the plunge moves up and let you load clay,
* **D3**: Print button: If not in load mode, pushing this will push the plunge down,
* **D4**: Direction signal pin: Sends the **direction** signal to the microstepper,
* **D5**: Step signal pin: Sends the **step** or **pull** signal to the microstepper,
* **A0**: potentiometer pin: reads the value of a potentiometer to adjust the motor speed.
* **A4**: Display SDA pin
* **A5**: Display SCL pin

## Hardware Pieces
* [I2C IIC Serial 128X64 OLED Display](https://www.amazon.com/Diymall-Yellow-Arduino-Display-Raspberry/dp/B00O2LLT30/ref=pd_ybh_a_sccl_14/134-0528381-5720948?pd_rd_w=5pb3W&content-id=amzn1.sym.67f8cf21-ade4-4299-b433-69e404eeecf1&pf_rd_p=67f8cf21-ade4-4299-b433-69e404eeecf1&pf_rd_r=HYT8AFBDCJMV97BM15M0&pd_rd_wg=oMxLi&pd_rd_r=b3105527-350c-42c0-93e3-7de0d7a3c01b&pd_rd_i=B00O2LLT30&psc=1)
* [Arduino Nano](https://www.amazon.com/gp/product/B07WK4VG58/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)
* [Digital Microstep driver DM542](https://www.amazon.com/gp/product/B07YWZRXGR/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1)
* Push Button
* Flip Switch

### Dependencies
* Adafruit_GFX (for display module)
* Adafruit_SSD1306 (for display module)
* Wire (for display module)

### Known Issues

* Not tested yet.

### To do

* [ ] Test on the robot
* [ ] Complete the documentation

# Notes

Just like any other porject that involves robots, take extreme caution while operating the system.
* In case of emergency hit STOP to stop the robot. Please be noted that the robot will not stop immediately, but (probably) after finishing its last buffered action. 

* The extruder will not stop unless you have the power cord detached.
--- 
By Ardavn Bidgoli, Fall 2022
