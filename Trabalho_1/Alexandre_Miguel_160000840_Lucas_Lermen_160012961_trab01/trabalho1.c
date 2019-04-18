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
#define PORT 123 //Porta a se conectar com o servidor

void format_time(struct tm * tm_info); //Função que formata a impressão do horário para português

typedef struct {
  uint8_t li_vn_mode;      // 8 bits. li, vn, and mode.
                           // "li".   2 bits. "Leap indicator" ou Indicador de salto.
                           // "vn".   3 bits. Número da versão do protocolo.
                           // "mode". 3 bits. Cliente escolhe o modo 3 para ser um cliente.
  uint8_t stratum;         // 8 bits. Nível de estrato do relógio local.
  uint8_t poll;            // 8 bits. Intervalo máximo entre mensagens sucessivas.
  uint8_t precision;       // 8 bits. Precisão do relógio local.
  uint32_t rootDelay;      // 32 bits. Tempo total de ida e volta de pacote.
  uint32_t rootDispersion; // 32 bits. Erro máximo permitido da fonte primária de clock.
  uint32_t refId;          // 32 bits. Identificador de referência de clock.
  uint32_t refTm_s;        // 32 bits. Referência do time­stamp em segundos.
  uint32_t refTm_f;        // 32 bits. Referência do time­stamp em fração de segundos.
  uint32_t origTm_s;       // 32 bits. Time­stamp em segundos original.
  uint32_t origTm_f;       // 32 bits. Time­stamp em fração segundos original.
  uint32_t rxTm_s;         // 32 bits. Timestamp recebido em segundos.
  uint32_t rxTm_f;         // 32 bits. Timestamp recebido em fração de segundos.
  uint32_t txTm_s;         // 32 bits e o campo mais importante para o cliente. Transmite o time­stamp em segundos.
  uint32_t txTm_f;         // 32 bits. Transmite o time­stamp em fração segundos.
} ntpPacket;               // Total: 384 bits ou 48 bytes.

int main(int argc, char *argv[]) {


    char *ipAdress = argv[1]; //Armazenamento do endereço de IP
    ntpPacket *reqMessage; //Cria pacote de mensagem para envio e recepção
    int thisSocket; //Declara informação do Socket
    struct sockaddr_in servaddr; //Declara informações do servidor
    struct timeval timeout={20,0}; //Valor do timeout em {segundos, microssegundos}
    int flag_timeout = 0; //Flag que identifica timeout



    //Setando a mensagem de requisição, com mode 3 para cliente
    reqMessage = calloc(1, sizeof(ntpPacket)); //Armazenamtno do espaço para o pacote
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

    //Cria socket -----------------------------------------------------------------
    if ( (thisSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Falha na criação do socket\n");
        exit(EXIT_FAILURE);
    }

    // Preenche informações para o servidor de destino da mensagem
    servaddr.sin_family = PF_INET;                      //protocolo de acesso
    servaddr.sin_addr.s_addr = inet_addr(ipAdress);     //endereço de IP
    servaddr.sin_port = htons(PORT);                    //porta de acesso - Host to network short converte do servidor local para servidor de internet
    //-----------------------------------------------------------------------------

    int n; //Variável que recebe o retorno do socket
    socklen_t server_addr_len; //

    // Seta uma opção de socket, no caso SO_RCVTIMEO que equivale a receber time out
    setsockopt(thisSocket, SOL_SOCKET ,SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval) );

    //Envia mensagem ------------------
    sendto(thisSocket, (const ntpPacket *) reqMessage, sizeof(ntpPacket), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("Requisição Enviada.\n");

    //Recebe Retorno -------------------
    n = recvfrom(thisSocket, (ntpPacket *) reqMessage, sizeof(ntpPacket), 0, (struct sockaddr *) &servaddr, &server_addr_len);

    if ( n < 0 ){
      //Informa mensagem de falha inicial
      printf( "Timeout atingido. Tentando receber informação novamente.\n" );
      //Realiza segunda tentativa de envio
      sendto(thisSocket, (const ntpPacket *) reqMessage, sizeof(ntpPacket), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
      //Informa da segunda tentativa realizada
      printf("Segunda Tentativa.\n");

      //Retorno do servidor
      int new_n = recvfrom(thisSocket, (ntpPacket *) reqMessage, sizeof(ntpPacket), 0, (struct sockaddr *) &servaddr, &server_addr_len);

      //Verificação do timeout
      if(new_n < 0){
        printf("Data/hora: não foi possível contactar servidor\n");
        flag_timeout = 1;
      }
    }

    if(flag_timeout == 0) {

      //Conversão do timestamp de network para host long, de internet para cliente local
      reqMessage->txTm_s = ntohl( reqMessage->txTm_s );
      //Conversão do timpestamp em relação ao horário base para o tipo time_t
      time_t txTm = ( time_t ) ( reqMessage->txTm_s - NTP_TIMESTAMP_DELTA );
      //Conversão para o tipo struct tm para posterior conversão para português
      struct tm * tm_info;
      tm_info = localtime (&txTm);

      format_time(tm_info);

    }

    //Fecha o socket
    close(thisSocket);



  return 0;
  }

  void format_time(struct tm * tm_info){
    printf("Data/hora: ");
    switch (tm_info->tm_wday){ // Dia da semana no atributo "week day" [0 - 6]
      case 0:
      printf ("Dom "); //Domingo
      break;

      case 1:
      printf ("Seg "); //Segunda-Feira
      break;

      case 2:
      printf ("Ter "); //Terça-Feira
      break;

      case 3:
      printf ("Qua "); //Quarta-Feira
      break;

      case 4:
      printf ("Qui "); //Quinta-Feira
      break;

      case 5:
      printf ("Sex "); //Sexta-Feira
      break;

      case 6:
      printf ("Sab "); //Sábado
      break;
    }

    switch (tm_info->tm_mon){ // Mês do ano no atributo "month" [0 - 11]
      case 0:
      printf ("Jan "); //Janeiro
      break;

      case 1:
      printf ("Fev "); //Fevereiro
      break;

      case 2:
      printf ("Mar "); //Março
      break;

      case 3:
      printf ("Abr "); //Abril
      break;

      case 4:
      printf ("Mai "); //Maio
      break;

      case 5:
      printf ("Jun "); //Junho
      break;

      case 6:
      printf ("Jul "); //Julho
      break;

      case 7:
      printf ("Ago "); //Agosto
      break;

      case 8:
      printf ("Set "); //Setembro
      break;

      case 9:
      printf ("Out "); //Outubro
      break;

      case 10:
      printf ("Nov "); //Novembro
      break;

      case 11:
      printf ("Dez "); //Dezembro
      break;
    }

    //Forma de printar o atributo "month day" [1 - 31], "hour" [0 - 23], "minute" [0 - 59], "second" [0 - 61] e "year" [anos desde 1900]
    printf("%d %02d:%02d:%02d %d\n", tm_info->tm_mday, tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tm_info->tm_year + 1900);
  }
