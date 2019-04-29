#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define QUEUE_T "/ticket"
#define QUEUE_S "/server"

typedef struct msg{
  int type;
  mqd_t client;
}Msg;

void return_ticket(int ticket){
  Msg msg;

  mqd_t queue_requests = mq_open(QUEUE_S,O_WRONLY);

  if (queue_requests <0){
    perror ("mq_open");
    exit (1);
  }

  msg.type = ticket;
  printf("Trying to return ticket: %d\n", msg.type);
  msg.client = queue_requests;

  int send = mq_send (queue_requests,(const char *) &msg, sizeof(msg), 0);

  if ( send < 0){
    perror ("mq_send");
    exit (1);
  }
  printf (">> Return Request of ticket: %d\n", msg.type);

  mq_close(queue_requests);
}

int get_ticket(){
  Msg msg;
  int ticket;

  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof(ticket);
  attr.mq_curmsgs = 0;

  mqd_t queue_tickets = mq_open(QUEUE_T, O_CREAT | O_EXCL | O_RDONLY , 0666, &attr);
  mqd_t queue_requests = mq_open(QUEUE_S,O_WRONLY);

  if (queue_tickets < 0 || queue_requests <0)   {
    perror ("mq_open");
    exit (1);
  }

  if (mq_getattr(queue_tickets, &attr) == -1)
      perror("mq_getattr");

   printf("\n\nMaximum of tickets stored: %ld\n", attr.mq_maxmsg);
   printf("Maximum ticket size: %ld\n\n", attr.mq_msgsize);

  msg.type = 0;
  msg.client = queue_tickets;

  int send = mq_send (queue_requests,(const char *) &msg, sizeof(msg), 0);

  if ( send < 0){
    perror ("mq_send");
    exit (1);
  }
  printf (">> Waiting for ticket (cod. %d)\n\n", msg.type);

  ssize_t receive = mq_receive (queue_tickets, (void *) &ticket, sizeof(ticket), 0);
  
  if(receive < 0){
    perror("mq_receive");
    exit(1);
  }
    printf("\nTicket received: %d\n", ticket);

  mq_close(queue_tickets);
  mq_close(queue_requests);
  return ticket;
}

int main(){
  mq_unlink(QUEUE_T); // Do possible set new attributes to queue
  int ticket;

  while(1){
    ticket = get_ticket();
    sleep(1);
    return_ticket(ticket);
  }

}
