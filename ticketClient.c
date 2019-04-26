#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>


#define QUEUE_SERV "/tickets_service"
#define QUEUE_CLIENTE "/tickets_received"

typedef struct msg {
    int type;
    mqd_t queue;
} Msg;

int get_ticket() {
    Msg msg;
    int ticket;
    mqd_t queue_serv;
    mqd_t queue_client;

    queue_serv =  mq_open(QUEUE_SERV, O_CREAT | O_RDWR, 0644, NULL);
    queue_client = mq_open(QUEUE_CLIENTE, O_CREAT | O_RDWR , 0644, NULL); // Abre fila do cliente

    if(queue_serv < 0 || queue_client < 0) { // Se der erro apresenta mensagem
        perror("mq_open");
        exit(1);
    }else{
      //Configurando mensagem a ser enviada
      msg.type = 0; //Tipo 0 - Solicitar ticket
      msg.queue = queue_client; // Enviar a fila em que deve ser armazenado o ticket solicitado

      int send = mq_send(queue_serv, (const char *) &msg, sizeof(msg), 0);

      if(send == 0){
        printf("Solicitação enviada ao servidor!\n");
      }else{
        perror("mq_send");
        exit(1);
      }
      ssize_t receive = mq_receive (queue_client, (void*)&ticket, sizeof(ticket), 0);

      if(receive < 0){
        perror("mq_receive");
        exit(1);
      }else{
        printf("\nTicket recebido: %d\n", ticket);
      }
      mq_close(queue_serv);
      mq_close(queue_client);
  }
    return ticket;
}

void return_ticket(int t) {
  Msg msg;
  mqd_t queue_serv = mq_open(QUEUE_SERV, O_WRONLY); //Abre fila do servidor

  if(queue_serv < 0) { // Se der erro apresenta mensagem
      perror("mq_open");
      exit(1);
  }

  //Configurando mensagem a ser enviada
  msg.type = t; //Tipo != 0 - Devolver ticket
  msg.queue = queue_serv;

  int send = mq_send(queue_serv, (void *)&msg, sizeof(msg), 0);

  if(send == 0)
    printf("Ticket Devolvido!\n");
  else
    printf("Houve um erro ao devolver o ticket!\n");

  mq_close(queue_serv);
}

int main () {
    int ticket;

    while(1){
      ticket = get_ticket();
      sleep(1);
      return_ticket(ticket);
   }

    return 0;
}
