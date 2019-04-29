#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>

#define QUEUE_S "/server"

#define N_MAX 10

int tickets[N_MAX];

typedef struct msg{
  int type;
  mqd_t client;
} Msg;

void tickets_init(){
  int i;
  for(i=0; i<N_MAX; i++){
    tickets[i] = i + 1;
  }
}

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

void return_ticket(int ticket){
  tickets[ticket-1] = ticket;
}

int main(){
  Msg msg;
  short int ticket; // mensagem a enviar

  mq_unlink(QUEUE_S);

  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof(msg);
  attr.mq_curmsgs = 0;

  tickets_init();

  //Abrir a fila
  mqd_t queue_server = mq_open(QUEUE_S, O_CREAT | O_RDWR, 0666, &attr);

  if(queue_server < 0){
    perror ("mq_open");
    exit (1);
  }

  if (mq_getattr(queue_server, &attr) == -1)
      perror("mq_getattr");

   printf("\n\nMaximum # of requests on server: %ld\n", attr.mq_maxmsg);
   printf("Maximum message size: %ld\n\n", attr.mq_msgsize);

  while(1){
    ssize_t receive = mq_receive (queue_server, (char *)&msg, sizeof(msg), NULL);
    if (receive < 0)     {
      perror("mq_receive:") ;
      exit (1) ;
    }
    if(msg.type == 0){
      ticket = getTicket(); // valor entre 0 e 99
      int send = mq_send (msg.client, (void *) &ticket, sizeof(ticket), 0);
      if (send < 0){
        perror ("mq_send");
        exit (1);
      }
      printf ("Sent ticket: %d\n", ticket);
    }else if(msg.type > 0){
      return_ticket(msg.type);
      printf("Returned ticket: %d\n", msg.type );
    }
  }
  mq_close(queue_server);
  return 0;
}
