#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>

#define QUEUE "/tickets_x"
#define MAX_SIZE 1024

int main (){
   mqd_t queue; // descritor da fila
   int msg; // mensagem a enviar
   struct mq_attr attr;

   attr.mq_flags = 0;
   attr.mq_maxmsg = 10;
   attr.mq_msgsize = sizeof(msg);
   attr.mq_curmsgs = 0;

   queue = mq_open(QUEUE, O_CREAT | O_RDWR , 0644, &attr);

   // abre a fila de mensagens, se existir
   if(queue < 0){
     printf("%d\n",queue );
     perror ("mq_open");
     exit (1);
   }
   while(1){
     msg = random() % 100 ; // valor entre 0 e 99
     // envia a mensagem
     int send = mq_send (queue, (void*) &msg, sizeof(msg), 0);

     if ( send < 0){
       perror ("mq_send");
       exit (1);
     }
     printf ("Sent message with value %d\n", msg);
     sleep (1) ;
  }

}
