//Adapted from the SunFounder ADXL335 3 axis accelerometer project
int sensorPin = A0;

void setup()
{  
  Serial.begin(9600);  
  Serial.print("Hello:\r\n"); 
} 

void readAccel(){
  int x = analogRead(A5);
  int y = analogRead(A4);
  int z = analogRead(A3);
                         
  float mVPerStep = (float)5000 / (float)1024;
  float zero_g_value = 1650 / mVPerStep; //ADC is 0~1023 with range of 0V-5V.  The range of the ADXL335 is 0V-3.3V.  The zero g output is equal to Vs/2 = 1.65V.                     
  float mVPerG = (float)330;  //ADXL335 Sensitivity is 330mv/g
  float scale = mVPerStep / mVPerG;
  
  Serial.print((((float)x - zero_g_value)) * scale);  //print x value on serial monitor
  Serial.print("\t");
  Serial.print((((float)y - zero_g_value)) * scale);  //print y value on serial monitor
  Serial.print("\t");
  Serial.print((((float)z - zero_g_value)) * scale);  //print z value on serial monitor
  Serial.print("\n");
  
  Serial.println("");
  
  delay(10);
}

void loop()
{
  readAccel();
}
