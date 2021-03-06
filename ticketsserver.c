#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>

#define N_MAX 100
#define QUEUE_SERV "/tickets_server"
#define QUEUE_CLIENTE "/tickets_received"

int tickets[N_MAX];

typedef struct msg {
    int type;
    mqd_t queue;
} Msg;

int getTicket(){
  int i, aux;
  for(i=0; i<N_MAX; i++){
      if(tickets[i]!=0){
        aux = tickets[i];
        tickets[i] = 0;
        return aux;
      }
  }
  return -1;
}

void tickets_init(){
  int i;
  for(i=0; i<N_MAX; i++){
    tickets[i] = i + 1;
  }
}

void return_ticket(int ticket){
  tickets[ticket-1] = ticket;
}

int main (){
  int ticket; // mensagem a enviar
  Msg msg;
  struct mq_attr attrserv, attrcli;

  attrserv.mq_flags = 0;
  attrserv.mq_maxmsg = 10;
  attrserv.mq_msgsize = sizeof(msg);
  attrserv.mq_curmsgs = 0;

  attrcli.mq_flags = 0;
  attrcli.mq_maxmsg = 20;
  attrserv.mq_msgsize = sizeof(ticket);
  attrcli.mq_curmsgs = 0;

  tickets_init();

  mqd_t queue_serv = mq_open(QUEUE_SERV, O_CREAT | O_RDWR, 0644, &attrserv);
  mqd_t queue_client = mq_open(QUEUE_CLIENTE, O_CREAT | O_RDWR , 0644, &attrcli); // Abre fila do cliente

  // abre a fila de mensagens, se existir
  if(queue_serv < 0){
    perror ("mq_open");
    exit (1);
  }
  while(1){
    printf("Aguardando requisição ..\n");
    ssize_t receive = mq_receive (queue_serv, (char *) &msg, sizeof(msg), NULL);
    if(receive != -1){
      if(msg.type == 0){ //Enviar ticket
        ticket = getTicket(); // valor entre 0 e 99
        printf("Ticket reservado: %d \n", ticket);
        int send = mq_send (queue_client, (void*) &ticket, sizeof(ticket), 0);

        if (send < 0){
          perror ("mq_send");
          exit (1);
        }
        printf ("Ticket Enviado: %d\n", ticket);
        sleep (1) ;
      }else{
        return_ticket(msg.type);
      }
    }else{
      perror("mq_receive");
    }
  }
  return 0;
}
