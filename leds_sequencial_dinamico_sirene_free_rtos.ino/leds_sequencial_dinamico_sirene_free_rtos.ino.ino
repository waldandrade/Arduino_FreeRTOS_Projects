#include <Arduino_FreeRTOS.h>

// Do exemplo passado no enunciado do trabalho
// https://www.arduinoecia.com.br/2013/02/sequencial-de-leds-arduino.html


int sirene = 7;
// Array para controlar os pinos digitais dos leds que deverão piscar
byte Pinosled[]={2,3,4,5};
byte ChavePress[]={false,false,false,false};
byte ChavesArray[]={8,9,10,11};

int ledDinamico = 0;
int estadoLedDinamico = 0;
int ledatual = 0;
// Fim do trecho retirado do exemplo do enunciado do trabalho

unsigned int sons[]={262,294,330,349,349,349,262,294,262,294,294,294,262,392,349,330,330,330,262,294,330,349,349,349};
unsigned long waits[]={200,300,300,300,300,300,100,300,100,300,300,300,200,200,200,300,300,300,200,300,300,300,300,300};

int disparo = 0; //Buzzer permanece desativado

//definindo o escopo das tarefas...
void TaskChaves(void *p);
void TaskPiscaLED(void *p);
void TaskPiscaDinamico(void *p);
void TaskDisparaSOM(void *p);

//definindo o escopo das funcoes...
void Chaves(void);
void PiscaLED(void);
void PiscaDinamico(void);
void DisparaSom(void);

void setup() {  
  pinMode(13,OUTPUT);
  pinMode(sirene, OUTPUT);

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
    ,  1  // Priority
    ,  NULL );

   xTaskCreate(
    TaskPiscaDinamico
    ,  (const portCHAR *) "PiscarLedDinamico"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
        
   xTaskCreate(
    TaskDisparaSOM
    ,  (const portCHAR *) "DisparaSom"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );

  xTaskCreate(
    TaskPiscaLED
    ,  (const portCHAR *) "PiscarLeds"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );
    
}

void loop() {
  // put your main code here, to run repeatedly:

}

void TaskDisparaSOM(void *p){
  while(1){
    DisparaSom();
    vTaskDelay(1);
  }
}

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

void DisparaSom(void){
  tone(sirene, (unsigned int)sons[disparo], waits[disparo]);
  vTaskDelay(waits[disparo] / portTICK_PERIOD_MS);
  disparo += 1;
  if(disparo == 24){
    disparo = 0;
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
         if(x == ledDinamico){
          estadoLedDinamico++;
          if(estadoLedDinamico > 2){
            estadoLedDinamico = 0;
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

void PiscaDinamico(void){
  if(estadoLedDinamico == 2){      
    digitalWrite(Pinosled[ledDinamico], HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS); //Cada unidade passada representa portTICK_PERIOD_MS (contante igual a 15) milisegundos
    digitalWrite(Pinosled[ledDinamico],LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }else if(estadoLedDinamico == 1){
    digitalWrite(Pinosled[ledDinamico], HIGH);    
  }
}

void PiscaLED(void){
  if(ledatual != ledDinamico || (ledatual == ledDinamico && estadoLedDinamico == 0)){
    digitalWrite(Pinosled[ledatual], HIGH);
    // tone(ledatual, sons[ledatual], 500);
    vTaskDelay(200 / portTICK_PERIOD_MS); //Cada unidade passada representa portTICK_PERIOD_MS (contante igual a 15) milisegundos
    digitalWrite(Pinosled[ledatual],LOW);
  }
  ledatual += 1;
  if(ledatual == 4){
    ledatual -= 4;
  }
}
