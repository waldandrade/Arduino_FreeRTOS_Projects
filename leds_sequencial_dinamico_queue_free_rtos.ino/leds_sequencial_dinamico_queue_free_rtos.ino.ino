#include <Arduino_FreeRTOS.h>
#include "queue.h"
#include "task.h"

/** 
 * Array para controle dos pinos digitais dos leds que deverao piscar
 */
byte Pinosled[]={2,3,4,5};

/** 
 * Array para controle do estado do botao ['pressionado', 'nao pressionado']
 */
byte ChavePress[]={false,false,false,false};
byte ChavesArray[]={8,9,10,11};

/**
 * Variavel para definicao do Led que tera o comportamento dinamico no array 
 */
QueueHandle_t queue = NULL;
unsigned int ledDinamico = 0;

/**
 * Variavel para guardar o estado do Led, como ele vai se comportar
 * 0 - Comportamento natural, piscando na sequência em 200ms
 * 1 - Sempre aceso
 * 2 - Piscando com 100ms
 */
unsigned int estadoLedDinamico = 0;

/**
 * Variavel define qual o led que deve acender na sequencia em intervalos de 200ms
 */
unsigned int ledatual = 0;

/*
 * Definindo o escopo das tarefas que serao implementadas futuramente
 */
void TaskChaves(void *p);
void TaskPiscaLED(void *p);
void TaskPiscaDinamico(void *p);

/**
 * Definindo o escopo das funcoes que serao implementadas futuramente
 */
void Chaves(void);
void PiscaLED(void);
void PiscaDinamico(void);

/**
 * Configurando o sistema
 */
void setup() {  
  /**
   * Criacao da fila
   * Parametros:
   * tamanho (length) da fila
   * tamanho do dado (inteiro)
   */
  queue = xQueueCreate(1, sizeof(int));

  /**
   * Inicializando a fila
   * Registrando o primeiro estado do LED dinamico
   */
  xQueueSend(queue, &estadoLedDinamico, (TickType_t) 0);
  
  pinMode(13,OUTPUT);

  /**
   * Loop que define os pinos de Led como saida
   */
  for (int x=0; x<4; x++){ pinMode(Pinosled[x],OUTPUT); }

  /**
   * Loop que define os pinos das chaves (botoes) como entrada
   * INPUT_PULLUP implica que o sinal de ativacao sera invertido
   */
  for (int x=0; x<4; x++){ pinMode(ChavesArray[x],INPUT_PULLUP); }

  /**
   * Criando a tarefa das Chaves no FreeRTOS
   * Tarefa de maior prioridade
   */
  xTaskCreate(TaskChaves,(const portCHAR *)"Chaves", 128, NULL, 3, NULL);

  /**
   * Criando a tarefa controle o comportamento natural dos Leds
   */
  xTaskCreate(TaskPiscaLED, (const portCHAR *)"PiscarLeds", 128, NULL, 2, NULL);
  
  /**
   * Criando a tarefa de controle do Led que tem o comportamento dinâmico
   */
  xTaskCreate(TaskPiscaDinamico, (const portCHAR *)"PiscarLedDinamico", 128, NULL, 1, NULL);



}

void loop() {
}

/**
 * Definindo das tarefas
 * 
 * NOTE:
 * Para que as tarefas prendam o processador eh necessario que a cada ciclo seja dado um delay de 1*portTICK_PERIOD_MS,
 * que equivale a 15 milissegundos
 */

void TaskChaves(void *p){
  while(1){
    Chaves();
    vTaskDelay(1);
  }
}

void TaskPiscaDinamico(void *p){
  while(1){
    PiscaDinamico();
    vTaskDelay(1);
  }
}

void TaskPiscaLED(void *p){
  while(1){
    PiscaLED();
    vTaskDelay(1);
  }
}

/**
 * Funcao de controle do estado das chaces
 */
void Chaves(void){
  for (int x=0; x<4; x++){
    if(!digitalRead(ChavesArray[x])){
      ChavePress[x] = true;
      /**
       * Caso o usuario aperte o botao, o LED eh aceso
       */
      digitalWrite(Pinosled[x],HIGH);      
     } else {
      if(ChavePress[x]){ 
        if(x == ledDinamico){
          
          static unsigned long delayBounce = 0;
          static unsigned long delayPisca;

          /**
           * Verificar a lógica desta instrução e por que nunca entra nela
           */
          if((xTaskGetTickCount() - delayBounce) > 10){         
            estadoLedDinamico++;
            if(estadoLedDinamico > 2){
              estadoLedDinamico = 0;
            }

            delayBounce = xTaskGetTickCount();

            xQueueOverwrite(queue, &estadoLedDinamico);
          }
        }
        if(x != ledDinamico || (x == ledDinamico && estadoLedDinamico != 1)){
          digitalWrite(Pinosled[x],LOW);
        }
        ChavePress[x] = false;
      }
    }
  }
}

/**
 * Funcao que gerencia o comportamento do LED dinamico em funcao da variavel que guarda o seu estado
 */
void PiscaDinamico(void){
  unsigned int estadoDinamico;
  xQueuePeek(queue, &estadoDinamico, (TickType_t) 0);
  
  if(estadoDinamico == 2){      
    digitalWrite(Pinosled[ledDinamico], HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS); //Cada unidade passada representa portTICK_PERIOD_MS (contante igual a 15) milisegundos
    digitalWrite(Pinosled[ledDinamico],LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }else if(estadoDinamico == 1){
    digitalWrite(Pinosled[ledDinamico], HIGH);    
  }
}

/**
 * Funcao que gerencia o comportamento natural dos LEDS, piscando um a um em sequencia de 200ms
 */
void PiscaLED(void){
  unsigned int estadoDinamico;
  xQueuePeek(queue, &estadoDinamico, (TickType_t) 0);
  
  if((ledatual != ledDinamico || (ledatual == ledDinamico && estadoDinamico == 0)) && ChavePress[ledatual] == false){
    digitalWrite(Pinosled[ledatual], HIGH);
    /**
     * Cada LED deve ficar 200 milissegundos aceso
     */
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(Pinosled[ledatual],LOW);
  }
  ledatual += 1;
  if(ledatual == 4){
    ledatual -= 4;
  }
}
