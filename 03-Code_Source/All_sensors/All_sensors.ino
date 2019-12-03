#include <Wire.h>
#include <HTS221.h> // temperature and humidity sensor
#include <LPS25H.h> //Pressure sensor
#include <Arduino.h>

// FOR I2C sensors
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10
// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 80

// for LCD
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// LED Matrix parameters
uint8_t scrollSpeed = 50;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
int brightness = 1; // 0 to 9
uint16_t scrollPause = 2000; // in milliseconds
#define  BUF_SIZE  75
char Message[BUF_SIZE] = { "" };

// display parameters
unsigned int pollingInterval = 1000; // in milliseconds
int refreshCounter = 0;    // refresh counter
int knockFilter = 0;      // knock filter is a counter used to filter fast knock
int parameter = 0;        // Current parameter to display
bool forceUpdate = true;    // Boolean to force the update of the display

// Initialization
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    // Display init
    P.begin();
    P.setTextEffect(scrollEffect, scrollEffect);
    P.setIntensity(brightness);

    Wire.begin();
    if (!smeHumidity.begin())
    { // Led On if there is an error initializing the sensor
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
    smePressure.begin();

    if (!lis.begin(0x18))
    {   // change this to 0x19 for alternative i2c address
        Serial.println("Couldnt start");
        //while (1);
    }
    Serial.println("LIS3DH found!");

    lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!

    Serial.print("Range = "); Serial.print(2 << lis.getRange());
    Serial.println("G");

    // 0 = turn off click detection & interrupt
    // 1 = single click only interrupt output
    // 2 = double click only interrupt output, detect single click
    // Adjust threshhold, higher numbers are less sensitive
    lis.setClick(2, CLICKTHRESHHOLD);
    delay(pollingInterval);
}

void loop()
{
    // put your main code here, to run repeatedly:

    // if the refresh counter is greater than 1000 and the DisplayAnimate equal true (the display is finished)
    // or if we want to force the update (the user knocked)
    if ((P.displayAnimate() == true && refreshCounter > 1000) || forceUpdate == true) // filter display in console, to give more time to knock detection
    {
        // ---------------------------------
        // Read information from the sensors

        // read the pressure and write it to the serial port
        int pressure = smePressure.readPressure();
        Serial.print("Pressure: ");
        Serial.print(pressure);

        // read the luminous flux and write it to the serial port
        int light = analogRead(A1);
        Serial.print(" | Light: ");
        Serial.print(light);

        // read the temperature and write it to the serial port
        double temperature = smeHumidity.readTemperature();
        Serial.print(" | Temperature: ");
        Serial.print(temperature);

        // Read the humidity and write it to the serial port
        // The value is returned as a percentage of humidity (0 = dry desert, 100 = rain forests)
        double humidity = smeHumidity.readHumidity();
        Serial.print(" | Humidity (%): ");
        Serial.print(humidity);
        Serial.print(" ");

        // print the current parameter
        Serial.println(parameter);

        // ----------------------------------------------------------------------------------
        // depending on the parameter, we will display selected information on the led matrix 

        if (parameter == 0)         // if the parameter is 0, display the temperature on the led matrix
            P.print(String(temperature, 1) + " C");
        else if (parameter == 1)    // if the parameter is 1, display the humidity on the led matrix
            P.print(String(humidity, 1) + " %");
        else if (parameter == 2)    // if the parameter is 2, display the luminous flux on the led matrix
            P.print(String(light) + " lum");
        else if (parameter == 3)    // if the parameter is 3, display the pressure on the led matrix
            P.print(String(pressure) + " mB");
        else                        // if the parameter is different from previous value, display all the information on the led matrix
        {
            // format the string to display
            sprintf(Message, "%d C %d %% %d lum %d mB", (int)temperature, (int)humidity, light, pressure);
            Serial.println(Message);
            P.displayText(Message, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);            
            P.displayReset();
        }
        
        forceUpdate = false;    // reset forceUpdate to false
        refreshCounter = 0;     // reset the refresh counter
    }


    refreshCounter++;           // increment the refresh counter
    knockFilter++;      // increment the click counter

    // get the click information
    uint8_t click = lis.getClick();
    
    // if we have a click (or double click) and the knockFilter > 1000
    // note that if the knockFilter is < 1000, the click will be ignore
    if ((click & 0x10 || click & 0x20) && knockFilter > 1000)
    {
        knockFilter = 0;                // reset the knockFilter to 0
        Serial.println(" click !!");    // write the "click !!" message on the serial port
        parameter++;                    // change the parameter to display to the next one
        forceUpdate = true;             // force the update of the display on the led matrix
        P.displayReset();
    }

    if (parameter > 4) // if the parameter is greater the last one, reset it 
        parameter = 0;
    
    return;
}
