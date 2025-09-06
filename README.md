# Computação Concorrente
## Resolução da atividade 5 do laboratório 5

Essa atividade teve como objetivo praticar as funções relacionadas às filas de threads criadas pela estrutura
"pthread_cond_t", tais como: pthread_cond_wait, pthread_cond_broadcast e pthread_cond_signal.
O código em questão trava as threads que atualizam um contador global toda vez que este atinge um valor divisível por 1000 até que
uma outra thread, somente responsável por printar o contador, printe o valor no contador. Após o valor ser printado, as threads
responsáveis pelo incremento são todas liberadas e então o ciclo se repete, até o contador atingir seu valor limite determinado.
