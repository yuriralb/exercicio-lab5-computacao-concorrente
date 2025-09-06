/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long int soma = 0; //variavel compartilhada entre as threads
int nthreads; //transformada em global para controle do while da função extra
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
pthread_cond_t cond; //variavel que inicializa a fila


//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  //printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<100000; i++) {
     //--entrada na SC
     pthread_mutex_lock(&mutex);
     //--SC (seção critica)

     if (soma % 1000 == 0) {
       pthread_cond_wait(&cond, &mutex); //tranca a thread se soma for divisível por 1000
     }

     soma++; //incrementa a variavel compartilhada
     pthread_mutex_unlock(&mutex);
  }
  //printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
  long int soma_local = 1; //variável de controle para a função printar
  //somente um valor por vez e também para controle do while
  const long int MAX_VAL = nthreads*100000; //máximo valor é quando todas as threads
  //tiverem contribuído para a soma 100 mil vezes

  while (soma_local < MAX_VAL) {
    pthread_mutex_lock(&mutex);
    if (soma % 1000 == 0) { //imprime se 'soma' for multiplo de 1000
      if (soma != soma_local) { //checa se o valor ja foi printado
        printf("soma = %ld \n", soma);
      }
      soma_local = soma;
      pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&mutex);
  }
  //printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&cond, NULL);

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--cria thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--espera todas as threads terminarem
   for (int t=0; t<nthreads+1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1);
     }
   }

   //--finaliza o mutex
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond);
   printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}
