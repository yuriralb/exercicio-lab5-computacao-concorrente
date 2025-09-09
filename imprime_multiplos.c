/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Alteração no codigo do lab 4 para fazer o print corretamente de todos os valores de soma */
/* Nome: Yuri Rocha de Albuquerque */
/* DRE: 123166143 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long int soma = 0; //variavel compartilhada entre as threads
int nthreads; //transformada em global para controle do while da função extra
int nthreads_ativas; //quantidade de threads ativas no momento
int nthreads_fila = 0; //quantidade de threads na fila no momento
pthread_mutex_t mutex; //variavel de lock para exclusao mutua da soma
pthread_mutex_t mutex_treads_ativas; //variavel de lock para exclusao mutua do numero de threads ativas
pthread_cond_t cond_soma; //variavel que inicializa a fila das threads que incrementam soma
pthread_cond_t cond_printa; //variavel que inciializa fila da thread que printa soma

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  //printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<100001; i++) {
    //--entrada na SC
    pthread_mutex_lock(&mutex);
    //--SC (seção critica)

    if (soma % 1000 == 0) {
      nthreads_fila++;
      //printf("Thread %ld entrou na fila\n", id);
      if (nthreads_fila == nthreads_ativas) {
        pthread_cond_signal(&cond_printa);
      }
      pthread_cond_wait(&cond_soma, &mutex);
    }

    soma++; //incrementa a variavel compartilhada
    pthread_mutex_unlock(&mutex);
  }

  pthread_mutex_lock(&mutex_treads_ativas);
  nthreads_ativas -= 1; //decrementa caso a thread tenha saído do for loop
  pthread_mutex_unlock(&mutex_treads_ativas);
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
    //printf("Thread PRINTA começou a executar!\n");
    printf("soma = %ld \n", soma);
    soma_local = soma;
    nthreads_fila = 0;
    pthread_cond_broadcast(&cond_soma); //libera todos da fila
    if (soma < MAX_VAL) { //checa se soma ja atingiu o limite para evitar loop infinito
      pthread_cond_wait(&cond_printa, &mutex); //libera o mutex para as threads de soma e entra na fila
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
   nthreads_ativas = nthreads;
   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);
   pthread_mutex_init(&mutex_treads_ativas, NULL);
   pthread_cond_init(&cond_soma, NULL);
   pthread_cond_init(&cond_printa, NULL);

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
   pthread_mutex_destroy(&mutex_treads_ativas);
   pthread_cond_destroy(&cond_soma);
   pthread_cond_destroy(&cond_printa);
   return 0;
}
