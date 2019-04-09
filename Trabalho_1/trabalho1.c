#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct {
  uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                           // li.   Two bits.   Leap indicator.
                           // vn.   Three bits. Version number of the protocol.
                           // mode. Three bits. Client will pick mode 3 for client.
  uint8_t stratum;         // Eight bits. Stratum level of the local clock.
  uint8_t poll;            // Eight bits. Maximum interval between successive messages.
  uint8_t precision;       // Eight bits. Precision of the local clock.
  uint32_t rootDelay;      // 32 bits. Total round trip delay time.
  uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;          // 32 bits. Reference clock identifier.
  uint32_t refTm_s;        // 32 bits. Reference time­stamp seconds.
  uint32_t refTm_f;        // 32 bits. Reference time­stamp fraction of a second.
  uint32_t origTm_s;       // 32 bits. Originate time­stamp seconds.
  uint32_t origTm_f;       // 32 bits. Originate time­stamp fraction of a second.
  uint32_t rxTm_s;         // 32 bits. Received time­stamp seconds.
  uint32_t rxTm_f;         // 32 bits. Received time­stamp fraction of a second.
  uint32_t txTm_s;        // 32 bits and the most important field the client cares about. Transmit time­stamp seconds.
  uint32_t txTm_f;         // 32 bits. Transmit time­stamp fraction of a second.
} ntpPacket;              // Total: 384 bits or 48 bytes.

int main(int argc, char *argv[]) {
    // printf("Número de argumentos: %d\n", argc);
    // for(int cont = 0; cont < argc; cont++){
    //     printf("%dº = %s\n", cont, argv[cont]);
    // }

    char *ipAdress = argv[1];
    // printf("ip Adress: %s\n", ipAdress);

    ntpPacket reqMessage;

    //Setando a mensagem de requisição
    reqMessage.li_vn_mode = 0x1B;  //li =0, vn = 3 e mode = 3 | li = 00, vn = 011, mode = 011   
    reqMessage.stratum = 0x00;         
    reqMessage.poll = 0x00;            
    reqMessage.precision = 0x00000000;       
    reqMessage.rootDelay = 0x00000000;      
    reqMessage.rootDispersion = 0x00000000; 
    reqMessage.refId = 0x00000000;          
    reqMessage.refTm_s = 0x00000000;        
    reqMessage.refTm_f = 0x00000000;        
    reqMessage.origTm_s = 0x00000000;      
    reqMessage.origTm_f = 0x00000000;       
    reqMessage.rxTm_s = 0x00000000;         
    reqMessage.rxTm_f = 0x00000000;         
    reqMessage.txTm_s = 0x00000000;        
    reqMessage.txTm_f = 0x00000000;  


    return 0;
}