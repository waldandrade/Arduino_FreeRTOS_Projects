#include <Arduino_FreeRTOS.h>


// Do exemplo passado no enunciado do trabalho
// https://www.arduinoecia.com.br/2013/02/sequencial-de-leds-arduino.html


// Array para controlar os pinos digitais dos leds que deverão piscar
byte Pinosled[]={2,3,4,5};
byte ChavePress[]={false,false,false,false};
byte ChavesArray[]={8,9,10,11};

int ledDinamico = 0;
int estadoLedDinamico = 0;
int ledatual = 0;
// Fim do trecho retirado do exemplo do enunciado do trabalho


//definindo o escopo das tarefas...
void TaskChaves(void *p);
void TaskPiscaLED(void *p);
void TaskPiscaDinamico(void *p);
void TaskChaveDinamica(void *p);

//definindo o escopo das funcoes...
void Chaves(void);
void PiscaLED(void);
void PiscaDinamico(void);
void ChaveDinamica(void);

//criando flag para controlar a passagem de estado do LED azul, mediante pressionamento da chave tactil correspondente
//boolean chaveAzPress=false;
int ledBlock = 0;
int clickCount = 0;

void setup() {  
  pinMode(13,OUTPUT);

  for (int x=0; x<4; x++) // Loop que define todos os pinos como saída
  {
    pinMode(Pinosled[x],OUTPUT);
  }

  for (int x=0; x<4; x++) // Loop que define todos os pinos como saída
  {
    pinMode(ChavesArray[x],INPUT_PULLUP);
  }

  // Precisa ter maior prioridade, para que as chaves serão prioritariamente observadas
  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskChaves
    ,  (const portCHAR *)"Chaves"   // A name just for humans
    ,  128  // O tamanho do espaço que vai ser criado que a tarefa manipule em palavras, no caso 128 * 4 bytes
    ,  NULL // passagem de parâmetro do tipo void
    ,  0  // Priority
    ,  NULL );
/*
  xTaskCreate(
    TaskChaveDinamica
    ,  (const portCHAR *)"ChaveDinamica"   // A name just for humans
    ,  128  // O tamanho do espaço que vai ser criado que a tarefa manipule em palavras, no caso 128 * 4 bytes
    ,  NULL // passagem de parâmetro do tipo void
    ,  1  // Priority
    ,  NULL );
    */

   /*
  xTaskCreate(
    TaskPiscaDinamico
    ,  (const portCHAR *) "PiscarLedDinamico"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );
    */
    
  xTaskCreate(
    TaskPiscaLED
    ,  (const portCHAR *) "PiscarLeds"
    ,  128  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL );

}

void loop() {
  // put your main code here, to run repeatedly:

}

void TaskChaves(void *p){
  while(1){
    Chaves();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void TaskChaveDinamica(void *p){
  while(1){
    ChaveDinamica();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void TaskPiscaDinamico(void *p){
  while(1){
    PiscaDinamico();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void TaskPiscaLED(void *p){
  while(1){
    PiscaLED();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

//**** Criando as funcoes....
void Chaves(void){
  for (int x=0; x<4; x++) // Loop que define todos os pinos como saída
  {
    if(!digitalRead(ChavesArray[x])){
      ChavePress[x] = true;
      digitalWrite(Pinosled[x],HIGH); //caso o usuario aperte o botao, o LED é aceso        
     }else{
       if(ChavePress[x]){  //verifica se a chave foi pressionada, atraves do estado da flag...
         if(x != ledDinamico || estadoLedDinamico != 1){
          digitalWrite(Pinosled[x],LOW);
         }
         if(x == ledDinamico){
          estadoLedDinamico++;
          if(estadoLedDinamico > 2){
            estadoLedDinamico = 0;
          }
         }
         ChavePress[x] = false;
       }
     }
  }
}

void PiscaDinamico(void){
  if(estadoLedDinamico == 1){
    digitalWrite(Pinosled[ledDinamico], HIGH);      
  }else if(estadoLedDinamico == 2){      
    digitalWrite(Pinosled[ledDinamico], HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS); //Cada unidade passada representa portTICK_PERIOD_MS (contante igual a 15) milisegundos
    digitalWrite(Pinosled[ledDinamico],LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void PiscaLED(void){
  if(ledatual != ledDinamico || (ledatual == ledDinamico && estadoLedDinamico == 0)){
    digitalWrite(Pinosled[ledatual], HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS); //Cada unidade passada representa portTICK_PERIOD_MS (contante igual a 15) milisegundos
    digitalWrite(Pinosled[ledatual],LOW);
  }
  ledatual += 1;
  if(ledatual == 4){
    ledatual -= 4;
  }
}
