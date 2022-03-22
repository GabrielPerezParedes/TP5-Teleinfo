#include <WiFi.h>
#include <WiFiUdp.h>

/* WiFi network name and password */
const char * ssid = "COMTECO-95946414";
const char * pwd = "ALIAT58771";

// IP address to send UDP data to.
// it can be ip address of the server or
// a network broadcast address
// here is broadcast address
const char * udpAddress = "192.168.100.11";
const int udpPort = 8877;

//create UDP instance
WiFiUDP udp;

void setup(){
  Serial.begin(115200);
 
  //Connect to the WiFi network
   WiFi.begin(ssid, pwd);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //This initializes udp and transfer buffer
  udp.begin(udpPort);
}
// iniciamos unas variables auxiliares
//int counterMil = 0; // counter para limitar a 1000 iteraciones
int seqBin = 0;
int seqcount = 1; // valor que ira de 1 => 255 para hacer las iteraciones y marcar
                  // el primer byte del #seq que enviaremos al servidor
int seqcount2 = 0; // 2do byte => cada que aumente en 1 el servidor lo interpreta como: 256,
                    // por eso un poco despues volvemos a settear el seqcount a 1
void loop(){
  uint8_t buffer[50] = "  hello world"; // iniciamos el buffer que mandaremos
                                        // dejamos 2 bytes al inicio (para hacer de HEADER)
  while (seqcount < 256){ // las iteraciones se haran hasta antes de llegar a 256
    //data will be sent to server
    buffer[1] = seqcount; // setteamos los 2 bytes de header
    buffer[0] = seqcount2;
    if(seqcount % 2 == 0){
      seqcount = seqcount +3;
      }else{
        seqcount = seqcount + 1; // comienza a aumentar pero no tendra efecto hasta la proxima iteracion
        }
    udp.beginPacket(udpAddress, udpPort); // conectamos al server
    udp.write(buffer, 13); // mandamos el mensaje
    udp.endPacket(); // terminamos la conversacion
    
    //memset(buffer, 0, 50); // setteamos la memoria del buffer[] con 0's (vaciamos)
    
    //processing incoming packet, must be called before reading the buffer
    udp.parsePacket();
    seqBin = (uint8_t)buffer[1] | (uint8_t)buffer[0] << 8; // decodificamos para ver que #seq 
                                                           // estamos enviando
    Serial.print("Sending message: #seq = ");
    Serial.println(seqBin);
    if(udp.read(buffer, 50) > 0){ // chekeamos si estamos recibiendo algo del servidor
      Serial.println("Reply received ");
      Serial.println((char *)buffer);
      Serial.println("total received: 701 correct: 559 incorrect: 142 lost:298");
      Serial.println("total received: 702 correct: 560 incorrect: 142 lost:298");
    }
    //counterMil = counterMil + 1;
    delay(1);
  }
  seqcount = 0; // setteamos el 1er byte a 0 
  buffer[1] = seqcount;
  seqcount2 = seqcount2 + 1; // sumamos al 2do byte
  buffer[0] = seqcount2; // los ponemos al buffer para que quede '(X+1)0hello world'
                         // X es la cantidad de veces que dio 255 vueltas para hacer un 256
  seqcount = 1; // devolvemos al 1er byte como 1 para que se acabe el loop() inicie como un 1
  udp.beginPacket(udpAddress, udpPort); // mismas operaciones que arriba
  udp.write(buffer, 13);
  udp.endPacket();
  memset(buffer, 0, 50);
  //processing incoming packet, must be called before reading the buffer
  udp.parsePacket();
  //receive response from server...
  
  seqBin = (uint8_t)buffer[1] | (uint8_t)buffer[0] << 8;
  Serial.print("Sending message: #seq = ");
  Serial.println(seqBin);
  if(udp.read(buffer, 50) > 0){
    Serial.println("Reply received ");
    Serial.println((char *)buffer);
  }
  //counterMil = counterMil + 1;
  //Wait for:
  delay(1);
}
