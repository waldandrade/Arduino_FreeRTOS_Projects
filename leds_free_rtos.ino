#include <Arduino_FreeRTOS.h>


// Do exemplo passado no enunciado do trabalho
// https://www.arduinoecia.com.br/2013/02/sequencial-de-leds-arduino.html


// Array para controlar os pinos digitais dos leds que deverão piscar
// byte Pinosled[]={2,3,4,5};
// int ledDelay; // Intervalo entre as alterações
// int direcao = 1; // Direcao do "movimento"
// int ledatual = 0;
// Fim do trecho retirado do exemplo do enunciado do trabalho


//definindo o escopo das tarefas...
void TaskChaves(void *p);
void TaskPiscaLEDAz(void *p);

//definindo o escopo das funcoes...
void Chaves(void);
void PiscaLEDAz(void);

//criando flag para controlar a passagem de estado do LED azul, mediante pressionamento da chave tactil correspondente
boolean chaveAzPress=false;

//Definicao dos pinos dos LEDs e das Chaves
int LEDVm   = 2;
int LEDAm   = 3;
int LEDVd   = 4;
int LEDAz   = 5;
int CHLedVm = 8;
int CHLedAm = 9;
int CHLedVd = 10;
int CHLedAz = 11;

void setup() {  
  pinMode(13,OUTPUT);
  pinMode(LEDVm,OUTPUT);
  pinMode(LEDAm,OUTPUT);
  pinMode(LEDVd,OUTPUT);
  pinMode(LEDAz,OUTPUT);
  pinMode(CHLedVm,INPUT_PULLUP);
  pinMode(CHLedAm,INPUT_PULLUP);
  pinMode(CHLedVd,INPUT_PULLUP);
  pinMode(CHLedAz,INPUT_PULLUP);

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
    TaskPiscaLEDAz
    ,  (const portCHAR *) "PiscarLeds"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
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

void TaskPiscaLEDAz(void *p){
  while(1){
    PiscaLEDAz();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

//**** Criando as funcoes....
void Chaves(void){
  if(!digitalRead(CHLedVm)){ 
    digitalWrite(LEDVm,HIGH); //caso o usuario aperte o botao, o LED é aceso
   }else{
    digitalWrite(LEDVm,LOW);
   }
   if(!digitalRead(CHLedAm)){ 
    digitalWrite(LEDAm,HIGH); //caso o usuario aperte o botao, o LED é aceso
   }else{
    digitalWrite(LEDAm,LOW);
   }
   if(!digitalRead(CHLedVd)){ 
    digitalWrite(LEDVd,HIGH); //caso o usuario aperte o botao, o LED é aceso
   }else{
    digitalWrite(LEDVd,LOW);
   }
   //*************************** ROTINA DE TRATAMENTO DO CHAVE TACTIL DO LED AZUL **********************************
   /**** Devido ao LED azul ser compartilhado por duas tarefas (recurso compartilhado), um tratamento adicional    *
    *  faz-se necessário para tentar amenizar o choque entre deixar o LED apagado permanentemente e deixá-lo aceso *
    *  ateh que a terefa TaskLEDs fosse chamada.  Variaveis publicas compartilhadas entre tarefas jamais devem     *                                            
    *  ser empregadas, pois nao podemos sempre garantir que uma tarefa seja executada antes da outra               */
   if(!digitalRead(CHLedAz)){
    chaveAzPress = true;       //positiva a flag indicando que a chave tactil do LED azul foi pressionada
    digitalWrite(LEDAz,HIGH); //caso o usuario aperte o botao, o LED é aceso
   }else{
     if(chaveAzPress){  //verifica se a chave foi pressionada, atraves do estado da flag...
      digitalWrite(LEDAz,LOW); //desliga imediatamente o LED azul, tao logo a chave tactil nao seja pressionada.  O LED voltará
                               //a piscar assim que a terefa "TaskPiscaLEDAz" ganhe do escalonador a permissao de ser executada 
      chaveAzPress=false;  //negativa a flag pois, se estamos em um loop infinito e se o usuario nao mais pressionasse a chave, 
                           //o estado do LED azul ficaria eternamente baixo, interferindo diretamente na tarefa de pisca-lo
     }
   }
}

void PiscaLEDAz(void){
  digitalWrite(LEDAz,HIGH);
  vTaskDelay(200 / portTICK_PERIOD_MS); //Cada unidade passada representa portTICK_PERIOD_MS (contante igual a 15) milisegundos
  digitalWrite(LEDAz,LOW);
  vTaskDelay(200 / portTICK_PERIOD_MS);
}
