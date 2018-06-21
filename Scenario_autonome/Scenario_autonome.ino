//ToneRecognition
#define  RCV_TIM_LIMIT  10000000

#define MY_LEDV  40 // LED bleu integrée carte
#define MY_LEDJ  39 // LED verte integrée carte

#define GIL_SW2  11 
#define GIL_SW1  12

#define GIL_BUTTON 9




#define BUF_SIZE  500     // size of the buffer containing the input samples. Must be greater than the order of the filters + K

#define KP 50             // number of samples for the estimation of the signal power
#define HFAO  228         // order of the first filter
#define HLAO  189         // order of the second filter

#define S 50000           // Threshold applied to the signal power  for the signal detection        


unsigned int  Ts=200;     // Sampling step in microseconds (sampling frequency = 5 KHz)

float buf[BUF_SIZE];      // Buffer containing the input samples
float pIn;                // input power
float y;                  // filtered signal
float z;
float pOut;
float pOut2;
float g;                  // computed pOut/pIn ratio
float h;
float Sg = 0.1;           // Threshod applied to g for the frequency detection

int compteur = 0;
int detect;

unsigned long current_time = 0;
unsigned long next_sample_time;


// Filters :
float HFA[HFAO] = { -0.047097, -0.000048, 0.000730, 0.001854, 0.003093, 0.004191, 0.004919, 0.005120, 0.004746, 0.003862, 0.002637, 0.001304, 0.000114, -0.000717, -0.001048, -0.000846, -0.000180, 0.000786, 0.001840, 0.002756, 0.003290, 0.003530, 0.003319, 0.002792, 0.002088, 0.001370, 0.000778, 0.000402, 0.000264, 0.000317, 0.000472, 0.000620, 0.000680, 0.000615, 0.000455, 0.000285, 0.000222, 0.000376, 0.000806, 0.001489, 0.002292, 0.003074, 0.003575, 0.003598, 0.003012, 0.001811, 0.000141, -0.001715, -0.003377, -0.004450, -0.004608, -0.003684, -0.001726, 0.000982, 0.003957, 0.006599, 0.008314, 0.008639, 0.007357, 0.004570, 0.000690, -0.003568, -0.007382, -0.009932, -0.010597, -0.009092, -0.005559, -0.000566, 0.004981, 0.009997, 0.013439, 0.014511, 0.012851, 0.008628, 0.002537, -0.004311, -0.010593, -0.015032, -0.016652, -0.014998, -0.010258, -0.003225, 0.004799, 0.012269, 0.017686, 0.019904, 0.018367, 0.013246, 0.005430, -0.003635, -0.012205, -0.018587, -0.021468, -0.020194, -0.014908, -0.006550, 0.003314, 0.012789, 0.020019, 0.023552, 0.022630, 0.017365, 0.008712, -0.001692, -0.011848, -0.019781, -0.023927, -0.023450, -0.018400, -0.009732, 0.000902, 0.011451, 0.019874, 0.024537, 0.024537, 0.019874, 0.011451, 0.000902, -0.009732, -0.018400, -0.023450, -0.023927, -0.019781, -0.011848, -0.001692, 0.008712, 0.017365, 0.022630, 0.023552, 0.020019, 0.012789, 0.003314, -0.006550, -0.014908, -0.020194, -0.021468, -0.018587, -0.012205, -0.003635, 0.005430, 0.013246, 0.018367, 0.019904, 0.017686, 0.012269, 0.004799, -0.003225, -0.010258, -0.014998, -0.016652, -0.015032, -0.010593, -0.004311, 0.002537, 0.008628, 0.012851, 0.014511, 0.013439, 0.009997, 0.004981, -0.000566, -0.005559, -0.009092, -0.010597, -0.009932, -0.007382, -0.003568, 0.000690, 0.004570, 0.007357, 0.008639, 0.008314, 0.006599, 0.003957, 0.000982, -0.001726, -0.003684, -0.004608, -0.004450, -0.003377, -0.001715, 0.000141, 0.001811, 0.003012, 0.003598, 0.003575, 0.003074, 0.002292, 0.001489, 0.000806, 0.000376, 0.000222, 0.000285, 0.000455, 0.000615, 0.000680, 0.000620, 0.000472, 0.000317, 0.000264, 0.000402, 0.000778, 0.001370, 0.002088, 0.002792, 0.003319, 0.003530, 0.003290, 0.002756, 0.001840, 0.000786, -0.000180, -0.000846, -0.001048, -0.000717, 0.000114, 0.001304, 0.002637, 0.003862, 0.004746, 0.005120, 0.004919, 0.004191, 0.003093, 0.001854, 0.000730, -0.000048, -0.047097};
float HLA[HLAO] = {0.033855,-0.021454,-0.017423,-0.014584,-0.011875,-0.008738,-0.005162,-0.001559,0.001424,0.003199,0.003447,0.002269,0.000152,-0.002145,-0.003865,-0.004455,-0.003765,-0.002044,0.000125,0.002062,0.003196,0.003272,0.002386,0.000945,-0.000525,-0.001549,-0.001894,-0.001602,-0.000960,-0.000330,-0.000005,-0.000053,-0.000313,-0.000466,-0.000218,0.000551,0.001668,0.002703,0.003102,0.002436,0.000620,-0.001969,-0.004551,-0.006163,-0.005990,-0.003755,0.000206,0.004729,0.008477,0.009973,0.008375,0.003864,-0.002483,-0.008811,-0.013038,-0.013515,-0.009706,-0.002417,0.006311,0.013817,0.017580,0.016099,0.009424,-0.000705,-0.011333,-0.019117,-0.021421,-0.017166,-0.007315,0.005364,0.017037,0.024010,0.023863,0.016326,0.003417,-0.011058,-0.022671,-0.027706,-0.024389,-0.013470,0.001898,0.017105,0.027450,0.029636,0.022801,0.008881,-0.007983,-0.022665,-0.030647,-0.029393,-0.019194,-0.003097,0.014026,0.026969,0.031773,0.026969,0.014026,-0.003097,-0.019194,-0.029393,-0.030647,-0.022665,-0.007983,0.008881,0.022801,0.029636,0.027450,0.017105,0.001898,-0.013470,-0.024389,-0.027706,-0.022671,-0.011058,0.003417,0.016326,0.023863,0.024010,0.017037,0.005364,-0.007315,-0.017166,-0.021421,-0.019117,-0.011333,-0.000705,0.009424,0.016099,0.017580,0.013817,0.006311,-0.002417,-0.009706,-0.013515,-0.013038,-0.008811,-0.002483,0.003864,0.008375,0.009973,0.008477,0.004729,0.000206,-0.003755,-0.005990,-0.006163,-0.004551,-0.001969,0.000620,0.002436,0.003102,0.002703,0.001668,0.000551,-0.000218,-0.000466,-0.000313,-0.000053,-0.000005,-0.000330,-0.000960,-0.001602,-0.001894,-0.001549,-0.000525,0.000945,0.002386,0.003272,0.003196,0.002062,0.000125,-0.002044,-0.003765,-0.004455,-0.003865,-0.002145,0.000152,0.002269,0.003447,0.003199,0.001424,-0.001559,-0.005162,-0.008738,-0.011875,-0.014584,-0.017423,-0.021454,0.033855};



// Code_son
#define LED_1 2
#define LED_2 34
#define LED_3 35
#define LED_4 36
#define LED_5 37
int son = 0; 

// Code_infra_photoresistance
int calibrationTime = 5;

int ledPinRed = 8; // Led rouge rajoutée
int ledPinBlue = 10;// Led bleu rajoutée
int ledPinGreen = 13; // Led verte rajoutée

int infraPin = 26;  
int photoResPin = 25;// choose the input pin (for PIR sensor)

int infra = 0;  // variable for reading the pin status
int photoRes = 0;

//Code_moteur
const int pinPwm1 = 14;
const int pinPwm2 = 15;
const int pot = 28;

const float VitMax = 255; // vitesse max du moteur

int VitM1; 
int VitM2;
int potValue = 0;


void setup() {

  //ToneRecognition
    int i,n;

  // initialize  serial ports:

  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(MY_LEDJ, OUTPUT);
  pinMode(MY_LEDV, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GIL_SW1, INPUT_PULLUP);
  pinMode(GIL_SW2, INPUT_PULLUP);
  pinMode(GIL_BUTTON, INPUT_PULLUP);


  // Switch on the LEDs
  digitalWrite(MY_LEDJ, HIGH);
  digitalWrite(MY_LEDV, HIGH);
  
  // Initialize buffer and power
  for (i = 0; i < BUF_SIZE; i++)
  {
    buf[i]  = 0;
  }

  pIn = 0;
  pOut = 0;
  detect = 0;
  g = 0;
  y = 0;
  delay(1);

  // switch off LEDs  
  digitalWrite(MY_LEDJ, LOW);
  digitalWrite(MY_LEDV, LOW);


  // Define the next smapling time
  next_sample_time = micros() + (unsigned long)Ts;

  // Code_son
  Serial.begin(9600);  
  pinMode(LED_1, OUTPUT);   
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);

  // Code_infra_photoresistance
   pinMode(ledPinRed, OUTPUT);    
 pinMode(ledPinBlue, OUTPUT);
 pinMode(ledPinGreen, OUTPUT);  
 
 pinMode(infra, INPUT);
 
 Serial.begin(9600);
 Serial.print("calibrating sensor ");
 for(int i = 0; i < calibrationTime; i++){
   Serial.print(".");
   delay(1000);
 }

 //Code_moteur
  // Initialise les pins PWM comme sorties digitals.
  pinMode(pinPwm1, OUTPUT);
  pinMode(pinPwm2, OUTPUT);

  Serial.begin(9600); // on lance la communication avec la console
  

}

void loop() {

  //ToneRecognition
  int i,n;
  

  // Read data : shift the previous samples in the buffer ...
  for (i = BUF_SIZE - 2; i >= 0; i--)
  {
    buf[i + 1] = buf[i];
  }
  // ... and acquire the new sample
  current_time = micros() ;
  while (current_time < next_sample_time)
  {
    current_time = micros() ;
  }
  buf[0] = (float) analogRead(7) - 3200.0;

  // Define the next sampling time
  next_sample_time += (unsigned long)Ts;

  
  // Update "instantaneous" power
  pIn = 0;
  for (i = 0; i < KP; i++)
  {
    pIn = pIn + buf[i] * buf[i];
  }
  pIn = pIn / (float)KP;


  // Test the amplitude of the instantaneous power
  if (pIn > S)
  {
    detect++;
    digitalWrite(MY_LEDJ, HIGH);   // Switch on the LED to indicate that an audio signal is detected


    // Apply filter and compute output power
    if (detect == 200)
    {
      detect  = 0;

      pOut = 0;
      pOut2 = 0;

      for (i = 0; i < KP; i++)
      {
        y = 0;
        z = 0;
        
        // For the detection of a G
        for (n = 0; n < HFAO; n++)   // Filter
        {
          z = z + (buf[i + n] * HFA[n]);
        }
        pOut2 = pOut2 + z * z;
        
        // For the detection of a A
        for (n = 0; n < HLAO; n++)   // Filter
        {
          y = y + (buf[i + n] * HLA[n]);
        }
        pOut  = pOut + y * y;
      }
      
      pOut = pOut / (float)KP ;
      pOut2 = pOut2 / (float)KP ;
      
      // Compute the ratio between the signal power and the filtered signal power
      g = pOut / pIn;
      h = pOut2 / pIn;

      
      // The tone is detected if pOut > Sg * Pin      
      if (g > Sg && h > Sg)
      {
        digitalWrite(MY_LEDV, HIGH);    // Switch on the led to indicate that the tone has been detected
        //Serial.println("Display power ratio:");
        //Serial.println(g * 1000);
      }
      else
      {
        digitalWrite(MY_LEDV, LOW);    // Switch off the led to indicate that the tone is not still detected
      }
    }
  }
  else      //  No useful audio signal detected
  {
      digitalWrite(MY_LEDJ, LOW);
      digitalWrite(MY_LEDV, LOW);
  }

  // Code_son
  son=analogRead(7);
  digitalWrite(LED_1, LOW); 
  digitalWrite(LED_2, LOW); 
  digitalWrite(LED_3, LOW); 
  digitalWrite(LED_4, LOW); 
  digitalWrite(LED_5, LOW);
  
 if (son > 2781 && son < 3219) {
   digitalWrite(LED_1, HIGH);
 }
 if ((son <= 2781 && son > 2562) || (son > 3219 && son < 3438 )) {
   digitalWrite(LED_1, HIGH); 
   digitalWrite(LED_2, HIGH);
 }
  if ((son <= 2562 && son < 2343) ||(son > 3438 && son < 3657) ) {
   digitalWrite(LED_1, HIGH); 
   digitalWrite(LED_2, HIGH);
   digitalWrite(LED_3, HIGH);
 }
 if ((son <= 2343 && son < 2124) ||(son > 3657 && son < 3876) ) {
   digitalWrite(LED_1, HIGH); 
   digitalWrite(LED_2, HIGH);
   digitalWrite(LED_3, HIGH);
   digitalWrite(LED_4, HIGH);
 }
  if ((son <= 2124 && son < 0) ||(son > 3876 && son < 4096) ) {
   digitalWrite(LED_1, HIGH); 
   digitalWrite(LED_2, HIGH);
   digitalWrite(LED_3, HIGH);
   digitalWrite(LED_4, HIGH);
   digitalWrite(LED_5, HIGH);
 }


 // Code_infra_photoresistance
 infra = analogRead(infraPin);  // read input value
 photoRes = analogRead(photoResPin);
 
 Serial.println("infrarouge :");
 Serial.println(infra);
 Serial.println("photores :");
 Serial.println(photoRes);
 if (infra > 600) {
  if (photoRes >= 0 && photoRes < 600) { // check if the input is HIGH
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinBlue, HIGH);  // turn LED ON
  }

   
 if (photoRes >= 600 && photoRes < 3000 ) {
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinBlue, LOW);
  digitalWrite(ledPinGreen, HIGH);
  }

   
 if (photoRes >= 3000) {
  digitalWrite(ledPinBlue, LOW);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, HIGH);
  }
 } 
 else{
   digitalWrite(ledPinBlue, LOW);
   digitalWrite(ledPinGreen, LOW);
   digitalWrite(ledPinRed, LOW);
 }

 //Code_moteur
  potValue = analogRead(pot);
  VitM1 = VitMax;
  VitM2 = VitMax;
  
  if (0 <= potValue && potValue < 1365){
  analogWrite(pinPwm1,VitM1); //On définit la vitesse de rotation marche avant
  }
  if (1365 <= potValue && potValue < 2730){
  analogWrite(pinPwm1,0);
  analogWrite(pinPwm2,0);
  }
  if (2730 <= potValue && potValue <= 4096){
  analogWrite(pinPwm2,VitM2); //On définit la vitesse de rotation marche arriere
  }

 
  
}
