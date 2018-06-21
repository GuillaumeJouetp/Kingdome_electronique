
//Initialisation de la trame
String trame = "";

int CHK = 0;
int time = 0;

//On déclare les références des différents capteurs et effecteurs

// ---------------CAPTEURS---------------------
//Potentiomètre
const int pot = 28;
const String potType = "D";
const String potId = "01";

//Infrarouge
const int infraPin = 26;
const String infraType = "9";
const String infraId = "02";

//Photoresistance
int photoResPin = 25;
const String photoType = "5";
const String photoId = "03";

//Bouton poussoir
int bouton = 17;

// ---------------EFFECTEURS---------------------

//Moteur
const int pinPwm1 = 14;
const int pinPwm2 = 15;
const float VitMax = 255; // vitesse max du moteur
const String motId = "04";

//LED soudée
int ledPinRed = 8; // Led rouge rajoutée
int ledPinBlue = 10;// Led bleu rajoutée
int ledPinGreen = 13; // Led verte rajoutée
const String ledId = "05";


void setup() {
  
  Serial.begin(9600);
  Serial1.begin(9600); // initialise la communication bluetooth avec la passerelle
  pinMode(bouton,INPUT);
  pinMode(pinPwm1, OUTPUT);
  pinMode(pinPwm2, OUTPUT);
  pinMode(ledPinRed, OUTPUT);    
  pinMode(ledPinBlue, OUTPUT);
  pinMode(ledPinGreen, OUTPUT); 

  // La trame doit être un String contenant tous ces élements pour un total de 19 caractères :
  
  // type de trame (1 = trame courante)  
  // numéro d'équipe (groupe = 009A) 
  // type de requete (1 = requête en écriture : l’objet envoie des informations à sauvegarder dans la base de données)
  // type de capteur (1=>Capteur de distance modèle 1 | 2=>Capteur de distance modèle 2 | 3=>Capteur de température | 4=>Capteur d’humidité | 5=>Capteur de lumière modèle 1 |6=>Capteur de couleur | 7=>Capteur de présence | 8=>Capteur de lumière modèle 2 | 9=>Capteur de mouvement | A=>Capteur présence son modèle 1
  // numéro du capteur (nn)
  // valeur du capteur en hexadecimal (00FF)
  // valeur de temps (nnnn : peut être une valeur silplement incrémenté a chaque trame)
  // somme des valeurs précedentes (nn)
  
  /* pour lire des données envoyés par la passerelle
  for (i=0 ; i<15 ; i++){
    Serial1.read
  }
  */
  
}

void loop() {

  // Pour l'instant ou envoie une trame uniquement si on appuie sur le bouton poussoir
  //if(digitalRead(bouton) == 0){
    
    // ------------------- ENVOIE DE TRAME : CARTE => PASSERELLE -----------------
    envoi_Trame(pot, potType,potId);
    reception_Trame();
    envoi_Trame(infraPin, infraType,infraId);
    reception_Trame();
    envoi_Trame(photoResPin, photoType,photoId);
    reception_Trame();
   
    //interval 
    delay(3000);
  //}
 
  // ------------------- RECEPTION DE TRAME :PASSERELLE => CARTE -----------------
  
  //Reception d'une trame
  String trameRecu = reception_Trame();
  
  //Cette fonction traite la trame reçu en modifiant la valeur du moteur uniquement si le numéro du capteur reçu correspnd au numéro du moteur
  handleReceptionTrameMoteur(trameRecu);
  //De meme pour la LED
  handleReceptionTrameLed(trameRecu);
  
  //Serial.print(reception_Trame()); 
  //debug_Trame_recu();  
 
}
  

void  envoi_Trame(int device, String device_type, String device_id){
  
    trame = trame + "1" + "009A" + "1" + device_type + device_id + fill_VAL(device) + fill_TIME() + "FF"; 
    Serial1.println(trame); 
    trame = "";
}

String reception_Trame(){
  
    // variable contenant le caractère à lire
    char carlu;
    // variable contenant le nombre de caractère disponibles dans le buffer
    int cardispo = 0;
    String lastTrame = "";

    cardispo = Serial1.available();

    if(cardispo > 0){ // tant qu'il y a des caractères à lire
        for (int i=0 ; i<15 ; i++){
          carlu = Serial1.read(); // on lit le caractère
          lastTrame = lastTrame + String(carlu);
      }
      cardispo = Serial1.available(); // on relit le nombre de caractères dispo
      return lastTrame;
     
    }
}

void handleReceptionTrameMoteur(String trame){
  
  String idRecu = String(trame[7])+String(trame[8]);
  //Serial.print("trame : " + trame);
  //Serial.println("");
    if( idRecu == motId){
      
      float VitM1 = VitMax; 
      float VitM2 = VitMax;
      String motValue = String(trame[9])+String(trame[10])+String(trame[11])+String(trame[12]);
      Serial.println("flag1");
      Serial.print("motValue : " + motValue);
      Serial.print("vitMax : " + String(VitMax));
      Serial.print("vitM1 : " + String(VitM1));
      if (motValue == String("0000")){
        Serial.print("arret : " + String(VitM1));
        analogWrite(pinPwm1,0);
        analogWrite(pinPwm2,0);
      }
      
      if (motValue == String("0001")){
        Serial.print("avant, vitesse M1 : " + String(VitM1));
        analogWrite(pinPwm2,0);
        analogWrite(pinPwm1,VitM1); //On définit la vitesse de rotation marche avant
        
      }
      
      if (motValue == String("0002")){
        Serial.print("arriere: " +  String(VitM1));
        analogWrite(pinPwm1,0);
        analogWrite(pinPwm2,VitM2); //On définit la vitesse de rotation marche arriere
      }  
    }
}

void handleReceptionTrameLed(String trame){
  
  String idRecu = String(trame[7])+String(trame[8]);

    if( idRecu == ledId){
  
      String ledValue = String(trame[9])+String(trame[10])+String(trame[11])+String(trame[12]);
      
      if (ledValue == String("1111")){
        digitalWrite(ledPinRed, HIGH);
        digitalWrite(ledPinGreen, HIGH);
        digitalWrite(ledPinBlue, HIGH);
      }
      
      if (ledValue == String("0000")){
        digitalWrite(ledPinRed, LOW);
        digitalWrite(ledPinGreen, LOW);
        digitalWrite(ledPinBlue, LOW);
      }  
  }
}

// Cette fonction permet d'incrémenter le champs TIME de la trame à chaque envoie de trame
// De plus elle vérifie que le champs est toujours composé de 4 caractères
String fill_TIME(){
    String timeString = "";
  
    time ++;
    
    if (time == 9999){
      time = 0;
    }
    
    timeString = String(time);
    
    if (timeString.length() == 1){
      timeString = "000" + timeString;
    }
    
    if (timeString.length() == 2){
      timeString = "00" + timeString;
    }
    
    if (timeString.length() == 3){
      timeString = "0" + timeString;
    }
    
    return timeString;
}

// Cette fonction permet de remplir le champ [VAL] de la trame
// De plus elle vérifie que le champs est toujours composé de 4 caractères
String fill_VAL(int device){
  
    int deviceValue = analogRead(device);
    
    //On traite la chaine de caractère
    String deviceValueString = String(deviceValue);
    
    
    if (deviceValueString.length() == 1){
      deviceValueString = "000" + deviceValueString;
    }
    
    if (deviceValueString.length() == 2){
      deviceValueString = "00" + deviceValueString;
    }
    
    if (deviceValueString.length() == 3){
      deviceValueString = "0" + deviceValueString;
    }
    
    return deviceValueString;
    
}

void debug_Trame_recu(){
  
      // variable contenant le caractère à lire
    char carlu;
    // variable contenant le nombre de caractère disponibles dans le buffer
    int cardispo = 0;
    String lastTrame = "";

    cardispo = Serial1.available();

    if(cardispo > 0){ // tant qu'il y a des caractères à lire
    
      for (int i=0 ; i<15 ; i++){
        carlu = Serial1.read(); // on lit le caractère
        lastTrame = lastTrame + String(carlu);
      }
      cardispo = Serial1.available(); // on relit le nombre de caractères dispo
      Serial.print("trame :" + lastTrame);
      Serial.println("");
      //lastTrame="";
    }
}





