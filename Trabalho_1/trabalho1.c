#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>

#define NTP_TIMESTAMP_DELTA 2208988800ull
#define MAXLINE 1024
#define PORT 123

void format_time(struct tm * tm_info);

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

    
    char *ipAdress = argv[1]; //Armazenamento do endereço de IP
    ntpPacket *reqMessage; //Cria pacote de mensagem
    int thisSocket; //Declara informação do Socker
    struct sockaddr_in servaddr;
    struct timeval timeout={60,0};
    int flag_timeout = 0;
    


    //Setando a mensagem de requisição
    reqMessage = calloc(1, sizeof(ntpPacket));
    reqMessage->li_vn_mode = 0x1B;  //li = 0, vn = 3 e mode = 3 | li = 00, vn = 011, mode = 011   
    reqMessage->stratum = 0x00;         
    reqMessage->poll = 0x00;            
    reqMessage->precision = 0x00000000;       
    reqMessage->rootDelay = 0x00000000;      
    reqMessage->rootDispersion = 0x00000000; 
    reqMessage->refId = 0x00000000;          
    reqMessage->refTm_s = 0x00000000;        
    reqMessage->refTm_f = 0x00000000;        
    reqMessage->origTm_s = 0x00000000;      
    reqMessage->origTm_f = 0x00000000;       
    reqMessage->rxTm_s = 0x00000000;         
    reqMessage->rxTm_f = 0x00000000;         
    reqMessage->txTm_s = 0x00000000;       
    reqMessage->txTm_f = 0x00000000;  

    //Abre socket -----------------------------------------------------------------
    if ( (thisSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Falha na criação do socket\n"); 
        exit(EXIT_FAILURE); 
    }
      
    // Preenche informações do servidor 
    servaddr.sin_family = PF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(ipAdress); 
    servaddr.sin_port = htons(PORT);
    //-----------------------------------------------------------------------------
      
    int n;
    socklen_t server_addr_len; 

    setsockopt(thisSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));

    //Envia mensagem ------------------
    sendto(thisSocket, (const ntpPacket *) reqMessage, sizeof(ntpPacket), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
    printf("Message sent.\n"); 

    //Recebe Retorno -------------------
    n = recvfrom(thisSocket, (ntpPacket *) reqMessage, sizeof(ntpPacket), 0, (struct sockaddr *) &servaddr, &server_addr_len); 
    
    if ( n < 0 ){
      printf( "Timeout atingido. Tentando receber informação novamente.\n" );
      int new_n = recvfrom(thisSocket, (ntpPacket *) reqMessage, sizeof(ntpPacket), 0, (struct sockaddr *) &servaddr, &server_addr_len); 
      if(new_n < 0){
        printf("Data/hora: não foi possível contactar servidor\n");
        flag_timeout = 1;
      }
    } 
    
    if(flag_timeout == 0) {

      reqMessage->txTm_s = ntohl( reqMessage->txTm_s ); // Time-stamp seconds.

      time_t txTm = ( time_t ) ( reqMessage->txTm_s - NTP_TIMESTAMP_DELTA );
      struct tm * tm_info;
      tm_info = localtime (&txTm);

      format_time(tm_info);
    
    }
  
    close(thisSocket);



  return 0;
  }

  void format_time(struct tm * tm_info){
    printf("Data/hora: ");
    switch (tm_info->tm_wday){
      case 0:
      printf ("Dom ");
      break;

      case 1:
      printf ("Seg ");
      break;

      case 2:
      printf ("Ter ");
      break;

      case 3:
      printf ("Qua ");
      break;

      case 4:
      printf ("Qui ");
      break;

      case 5:
      printf ("Sex ");
      break;

      case 6:
      printf ("Sab ");
      break;
    }

    switch (tm_info->tm_mon){
      case 0:
      printf ("Jan ");
      break;

      case 1:
      printf ("Fev ");
      break;

      case 2:
      printf ("Mar ");
      break;

      case 3:
      printf ("Abr ");
      break;

      case 4:
      printf ("Mai ");
      break;

      case 5:
      printf ("Jun ");
      break;

      case 6:
      printf ("Jul ");
      break;

      case 7:
      printf ("Ago ");
      break;

      case 8:
      printf ("Set ");
      break;

      case 9:
      printf ("Out ");
      break;

      case 10:
      printf ("Nov ");
      break;

      case 11:
      printf ("Dez ");
      break;
    }

    printf("%d %02d:%02d:%02d %d\n", tm_info->tm_mday, tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tm_info->tm_year + 1900);
  }