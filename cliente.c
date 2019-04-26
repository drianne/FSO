#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>

#define QUEUE "/tickets_x"

int main () {

   mqd_t queue; // descritor da fila de mensagens
   int msg ; // as mensagens são números inteiros

   queue = mq_open(QUEUE, O_CREAT | O_RDONLY, 0644, NULL);
   // abre ou cria a fila com permissoes 0666
   if (queue < 0)   {
     perror ("mq_open");
     exit (1);
   }
   // recebe cada mensagem e imprime seu conteudo
   for (;;){
     ssize_t receive = mq_receive (queue, (void*)&msg, sizeof(msg), NULL);
     if (receive < 0)     {
       perror("mq_receive:") ;
       exit (1) ;
     }
     printf ("Received msg value %d\n", msg);
   }
}
