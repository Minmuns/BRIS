//   --- Bibliotecas Auxiliares ---
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//                                        --- Mapeamento de Hardware ---
extern volatile unsigned long timer0_millis;
#define SS_PIN 53
#define RST_PIN 49
#define OLED_RESET 4
#define pinReset AnologToDigital(analogRead(A0))
Adafruit_SSD1306 display(OLED_RESET);

 
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Cria instância com MFRC522
 
// --- Variáveis Globais --- 
//char st[20];
String mainString1 = "";
String mainString2 = "";
String mainString3 = "";

void Change_Pass_Word();
bool Pass_Word_Acesse();
void Memory_Write(int p[4]);
bool Memory_Read(int p[4]);
void Memory_Erase(int p[4]);
bool AnologToDigital(int p);
void Cartao_RFID(bool q);

//                                        --- Configurações Iniciais ---
void setup() 
{
  Serial.begin(9600);   // Inicia comunicação Serial em 9600 baud rate
  SPI.begin();          // Inicia comunicação SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //Inicializa OLED com endereço I2C 0x3C (para 128x64)
  display.clearDisplay();
  display.display();
  
  if(EEPROM.read(1) != 0){
    
      EEPROM.write(0,8);
      EEPROM.write(1,0);
      EEPROM.write(2,8);
      EEPROM.write(3,EEPROM.read(0));
      EEPROM.write(4,1);
      EEPROM.write(5,1);
      EEPROM.write(6,1);
      EEPROM.write(7,1);
      byte a = EEPROM.read(2);
      do{

        EEPROM.write(a,1);
        ++a;
        
        }while(a != 255);
    }
  for(byte a = 4;a != 8;++a){
    pinMode(a,INPUT);
  }

  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  
} //    end setup

void Oled_Write(){
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,16);
    display.clearDisplay();
    display.print(mainString1);
    display.setCursor(0,32);
    display.print(mainString2);
    display.setCursor(0,48);
    display.print(mainString3);
    display.display();
  
}

void Time0(){
      noInterrupts();
      timer0_millis = 0;
      interrupts();
}

void Cartao_RFID(bool p);

void RFID(){
  bool A = 1;
  while(1){
    
      mainString1 = "RFID.CONF";
      Oled_Write();

      if(digitalRead(4)){
        while(digitalRead(4)){}
        return;
      }
    
      
      if(digitalRead(6)){
        A = 1;  
        while(digitalRead(6)){}
         mainString2 = "C.WRITE";
      }else if(digitalRead(7)){
        A = 0;
        while(digitalRead(7)){}
         mainString2 = "C.DELETE";
        }
 //       Serial.println(A);
      if(A && digitalRead(5)){
      
        Cartao_RFID(1);
        
        mainString2 = "C.WRITE";
        mainString3 = "";
        

      }else if(!A && digitalRead(5)){ 
  
        Cartao_RFID(0);
   
        mainString2 = "C.WRITE";
        mainString3 = "";
       
        A = 1; 
      
      }
  }
}
/*
void Bio(){
  mainString1 = "BIO.CONF";
  mainString2 = "";
  Oled_Write();

}
*/
void Teclado(){
  mainString1 = "TCL.CONF";
  mainString2 = "MUDAR";
  mainString3 = "SENHA";
  Oled_Write();

  while(1){
    if(digitalRead(5)){
      while(digitalRead(5));
      Change_Pass_Word();
      return;
    
    }else if(digitalRead(4)){
      while(digitalRead(4));
      mainString3 = "";
      return;
    }
  }
}
  
void Configuracoes(){
  mainString1 = "MOD.CONFG";
  while(1){

      if(digitalRead(4)){
        while(digitalRead(4));
        return;
      }
      
    Oled_Write();
    
    static char a = 1;
    if(digitalRead(6)){
      a == 2? a = 1:++a;
      
      while(digitalRead(6));
      
    }
    if(digitalRead(7)){
      a == 1? a = 2:--a;
      
      while(digitalRead(7));
     
    }

    switch(a){
      case 1: 

        mainString2 = "RFID";
        Oled_Write();
      
        if(digitalRead(5)){
          while(digitalRead(5));
          mainString2 = "C.WRITE";
          RFID();
        }
      break;

/*      case 3: 

         mainString2 = "BIO";
         Oled_Write();
         
         if(digitalRead(5)){
           while(digitalRead(5)){}
           mainString2 = "";
           Bio();
         }
      break;*/

      case 2:
      
          mainString2 = "TECLADO";
          Oled_Write();
          
          if(digitalRead(5)){
            while(digitalRead(5));
            mainString2 = "";
            Teclado();
          }

      break;
      
    }
  }  
}

void UnLock(){

  digitalWrite(11,1);
  Time0();
  while(millis() < 300);
  digitalWrite(11,0);
  while(millis() < 600);
  digitalWrite(11,0);
  digitalWrite(10,1);
  Time0();
  while(millis() < 5000);
  digitalWrite(10,0);
}

void User(){

  while(1){
    Time0();
    mainString1 = "USER ON";
    mainString2 = "";
    mainString3 = "";
    Oled_Write();
    
  // Verifica novos cartões
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  { 
    goto here;
  }
  // Seleciona um dos cartões
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  { 
    goto here;
  }          

  int q[4];
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {     
      q[i]=mfrc522.uid.uidByte[i];      
  }

    if(!Memory_Read(q)){

      mainString2 = "ACESSO";
      mainString3 = "LIBERADO";
      Oled_Write();
      UnLock();
      mainString2 = "";
      mainString3 = "";
      Oled_Write();
      
    }else{

      mainString2 = "ACESSO";
      mainString3 = "NEGADO";
      Oled_Write();
      Time0();
      while(millis() <= 3000);
      mainString2 = "";
      mainString3 = "";
      Oled_Write();
      
    }

    here:
    if((digitalRead(4)||digitalRead(5))||(digitalRead(6)||digitalRead(7))){
      if(Pass_Word_Acesse()){
        
        mainString2 = "ACESSO";
        mainString3 = "LIBERADO";
        Oled_Write();
        UnLock();
        mainString2 = "";
        mainString3 = "";
        Oled_Write();

      }
    }
  }
}

// --- Loop Infinito ---
void loop() 
{ 
  Time0();
  bool q = 0;

  Serial.println(EEPROM.read(0));
   
  for(int a = millis(); (millis() < a+4000 && pinReset);){
    if(millis() > a+3000){
      q = 1;
    }
  }
   
  if(q){ 
    mainString1 = "WAITING...";  
    mainString2 = "PASS WORD";
    if(Pass_Word_Acesse()){
  
      mainString1 = "";
      mainString2 = "";
      mainString3 = "";
      Configuracoes();
      mainString1 = "";
      mainString2 = "";
      mainString3 = "";
      User();
    }
    
  }else{

    User();
    
  }
} 

bool Pass_Word_Acesse(){

  static int a = 0;
  int A[4];

  Oled_Write();
  
  while(1){
    
    if(digitalRead(4)){
     A[a] = 1;
     ++a; 
        
     while(digitalRead(4));
    }else if(digitalRead(5)){
      A[a] = 2;
      ++a;
     
      
      while(digitalRead(5));
    }else if(digitalRead(6)){
      A[a] = 3;
      ++a;
      
      
      while(digitalRead(6));
    }else if(digitalRead(7)){
      A[a] = 4;
      ++a;
   
      
      while(digitalRead(7));
    }
      if(a > 0){
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,16);
    display.clearDisplay();
    display.print(mainString1);
    for(int B = 0;B < a; ++B){
    display.setCursor((B*10),32);
    display.print("*");
    }
    display.setCursor(0,48);
    display.print("");
    display.display();
  }
  
    if(a >= 4){
      
    int ref = 0;
    for(int g = 0; g < 4; ++g){
      if(A[g] == EEPROM.read(4+g)){
        ++ref;
      }
      Serial.println(A[g]);
      Serial.println(EEPROM.read(4+g));
    }

    if(ref == 4){
      
      mainString2 = "";
      mainString3 = "";
      Oled_Write();
      a = 0;
      return 1;
      
    }else{
      mainString2 = "ACESSO";
      mainString3 = "NEGADO";
      Oled_Write();
  
      Time0();
      while(millis() < 2000);
  
    }
    a = 0;
    return 0;
    }
  }
}

void Change_Pass_Word(){
  mainString2 = "DIGITE A";
  mainString3 = "NOVA SENHA";
  Oled_Write();
  
  int a = 0;
  int valor = 0;
  
  while(1){
    
  if(digitalRead(4)){
   ++a;
   EEPROM.write(3+a,1);
   
   while(digitalRead(4));
  }else if(digitalRead(5)){
    ++a;
    EEPROM.write(3+a,2);
    
    while(digitalRead(5));
  }else if(digitalRead(6)){
    ++a;
    EEPROM.write(3+a,3);
    
    while(digitalRead(6));
  }else if(digitalRead(7)){
    ++a;
    EEPROM.write(3+a,4);
    
    while(digitalRead(7));
  }
  if(a > 0){
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,16);
    display.clearDisplay();
    display.print(mainString1);
    for(int A = 0;A < a; ++A){
    display.setCursor((A*10),32);
    display.print("*");
    }
    display.setCursor(0,48);
    display.print("");
    display.display();
  }
  

  if(a >= 4){
    mainString2 = "SENHA";
    mainString3 = "ALTERADA";
    Oled_Write();
    Time0();
    while(millis() <= 2000);
    mainString3 = "";
    return;
  }
  }
}

//                                           Leitura do Cartão
 void Cartao_RFID(bool p){
  
  mainString2 = "WAITING";
  mainString3 = "CARD";
  Oled_Write();
  
  here:
  // Verifica novos cartões
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  { 
    goto here;
  }
  // Seleciona um dos cartões
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  { 
    goto here;
  }                                   
  int q[4];
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {     
      q[i]=mfrc522.uid.uidByte[i];      
  }

  if(p){
    
    Memory_Write(q);
  
  }else{
     
    Memory_Erase(q);
  
  }
}

 
void Memory_Erase(int p[4]){

  for(int i=EEPROM.read(2); i <= EEPROM.read(0);++i){
      if(p[0] == EEPROM.read(i)){
        if(p[1] == EEPROM.read(i+1)){
          if(p[2] == EEPROM.read(i+2)){
            if(p[3] == EEPROM.read(i+3)){
              for(int I=0;I<=3;++I){
                 Serial.println(EEPROM.read(i+I));
                EEPROM.write(i+I,0);
                Serial.println(EEPROM.read(i+I));
              }
              EEPROM.write(3,EEPROM.read(3)-1);
              mainString2 = "ID";
              mainString3 = "APAGADO";
              Oled_Write();


              Time0();

              while(millis() <= 2000){}
              return;
            }
          }
        }      
       }
      }
  mainString2 = "ID NAO";
  mainString3 = "ENCONTRADO";
  Oled_Write();

  Time0();
  
  while(millis() <= 2000){}
}

bool Memory_Read(int p[4]){
  
//    Serial.println(EEPROM.read(2));
//    Serial.println(EEPROM.read(0));
    
  for(int i=EEPROM.read(2); i <= EEPROM.read(0)+1;++i){
//    Serial.print(EEPROM.read(i));
    Serial.print(" = ");
    Serial.println(p[0]);

    if(p[0] == EEPROM.read(i)){
 
      Serial.println();
      Serial.println(" primeiro foi");
      Serial.println();
     
      if(p[1] == EEPROM.read(i+1)){

        Serial.println();
        Serial.println(" segundo foi");
        Serial.println();

        if(p[2] == EEPROM.read(i+2)){
          
          Serial.println();
          Serial.println("terceiro foi");
          Serial.println();
         
          if(p[3] == EEPROM.read(i+3)){
            return 0;
          }
        }
      }
    }
  }
   return 1;
}

bool AnologToDigital(int p){
  
  if(p >= 1020){
    return 1;
  }else{
    return 0;
  }
}

 void Memory_Write(int p[4]){

  if(Memory_Read(p)){
    
    bool A = 0;
    int flag;

    
    for(int a=EEPROM.read(2); a <= EEPROM.read(0);++a){
      if(EEPROM.read(a) == 0){
        if(EEPROM.read(a+1) == 0){
          if(EEPROM.read(a+2) == 0){
            if(EEPROM.read(a+3) == 0){
               A = 1;
               flag = a;
            }
          }
        }
      }
    }
//    Serial.println(EEPROM.read(0));
    if(!A){
      for(int i=0;i <= 3;++i){
       
        EEPROM.write(EEPROM.read(0)+i+1,p[i]);
      }
      EEPROM.write(0,EEPROM.read(0)+4);
      EEPROM.write(3,EEPROM.read(3)+1);
      mainString2 = "ID 2";
      mainString3 = "REGISTRADO";
    
    }else{
      for(int i=0;i <= 3;++i){
       Serial.println(flag+i);
        EEPROM.write(flag+i,p[i]);
      }
      EEPROM.write(3,EEPROM.read(3)+1);
      Serial.println(EEPROM.read(0));
      mainString2 = "ID 1";
      mainString3 = "REGISTRADO";
    }

      Oled_Write();
            
      Time0();

      while(millis() <= 2000){}
      
  }else{
      mainString2 = "ID";
      mainString3 = "EXISTENTE";
      Oled_Write();

      Time0();
      
      while(millis() <= 2000){}
  }
     Serial.println();
     for(int a = 0;a <= 3;++a){
  Serial.println(p[0+a]);
  }
   for(int a=0;a != 4;++a){

      Serial.print(EEPROM.read(EEPROM.read(0)-2+a));
      Serial.print(" ");
    }
    
    Serial.println();
}
