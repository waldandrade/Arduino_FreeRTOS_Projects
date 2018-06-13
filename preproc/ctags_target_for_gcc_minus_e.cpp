# 1 "C:\\Users\\Waldney\\Documents\\uern\\sistemas em tempo real\\terceira unidade\\leds_free_rtos\\leds_free_rtos.ino"
# 1 "C:\\Users\\Waldney\\Documents\\uern\\sistemas em tempo real\\terceira unidade\\leds_free_rtos\\leds_free_rtos.ino"
# 2 "C:\\Users\\Waldney\\Documents\\uern\\sistemas em tempo real\\terceira unidade\\leds_free_rtos\\leds_free_rtos.ino" 2


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
int LEDVm = 2;
int LEDAm = 3;
int LEDVd = 4;
int LEDAz = 5;
int CHLedVm = 8;
int CHLedAm = 9;
int CHLedVd = 10;
int CHLedAz = 11;

void setup() {
  pinMode(13,0x1);
  pinMode(LEDVm,0x1);
  pinMode(LEDAm,0x1);
  pinMode(LEDVd,0x1);
  pinMode(LEDAz,0x1);
  pinMode(CHLedVm,0x2);
  pinMode(CHLedAm,0x2);
  pinMode(CHLedVd,0x2);
  pinMode(CHLedAz,0x2);

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskChaves
    , (const char *)"Chaves" // A name just for humans
    , 128 // O tamanho do espaço que vai ser criado que a tarefa manipule em palavras, no caso 128 * 4 bytes
    , __null // passagem de parâmetro do tipo void
    , 1 // Priority
    , __null );

  xTaskCreate(
    TaskPiscaLEDAz
    , (const char *) "PiscarLeds"
    , 128 // Stack size
    , __null
    , 2 // Priority
    , __null );

}

void loop() {
  // put your main code here, to run repeatedly:

}

void TaskChaves(void *p){
  while(1){
    Chaves();
    vTaskDelay(1 / ( (TickType_t) (1 << (0 /* portUSE_WDTO to use the Watchdog Timer for xTaskIncrementTick*/ + 4)) ) /* Inaccurately assuming 128 kHz Watchdog Timer.*/);
  }
}

void TaskPiscaLEDAz(void *p){
  while(1){
    PiscaLEDAz();
    vTaskDelay(1 / ( (TickType_t) (1 << (0 /* portUSE_WDTO to use the Watchdog Timer for xTaskIncrementTick*/ + 4)) ) /* Inaccurately assuming 128 kHz Watchdog Timer.*/);
  }
}

//**** Criando as funcoes....
void Chaves(void){
  if(!digitalRead(CHLedVm)){
    digitalWrite(LEDVm,0x1); //caso o usuario aperte o botao, o LED é aceso
   }else{
    digitalWrite(LEDVm,0x0);
   }
   if(!digitalRead(CHLedAm)){
    digitalWrite(LEDAm,0x1); //caso o usuario aperte o botao, o LED é aceso
   }else{
    digitalWrite(LEDAm,0x0);
   }
   if(!digitalRead(CHLedVd)){
    digitalWrite(LEDVd,0x1); //caso o usuario aperte o botao, o LED é aceso
   }else{
    digitalWrite(LEDVd,0x0);
   }
   //*************************** ROTINA DE TRATAMENTO DO CHAVE TACTIL DO LED AZUL **********************************
   /**** Devido ao LED azul ser compartilhado por duas tarefas (recurso compartilhado), um tratamento adicional    *
    *  faz-se necessário para tentar amenizar o choque entre deixar o LED apagado permanentemente e deixá-lo aceso *
    *  ateh que a terefa TaskLEDs fosse chamada.  Variaveis publicas compartilhadas entre tarefas jamais devem     *                                            
    *  ser empregadas, pois nao podemos sempre garantir que uma tarefa seja executada antes da outra               */
   if(!digitalRead(CHLedAz)){
    chaveAzPress = true; //positiva a flag indicando que a chave tactil do LED azul foi pressionada
    digitalWrite(LEDAz,0x1); //caso o usuario aperte o botao, o LED é aceso
   }else{
     if(chaveAzPress){ //verifica se a chave foi pressionada, atraves do estado da flag...
      digitalWrite(LEDAz,0x0); //desliga imediatamente o LED azul, tao logo a chave tactil nao seja pressionada.  O LED voltará
                               //a piscar assim que a terefa "TaskPiscaLEDAz" ganhe do escalonador a permissao de ser executada 
      chaveAzPress=false; //negativa a flag pois, se estamos em um loop infinito e se o usuario nao mais pressionasse a chave, 
                           //o estado do LED azul ficaria eternamente baixo, interferindo diretamente na tarefa de pisca-lo
     }
   }
}

void PiscaLEDAz(void){
  digitalWrite(LEDAz,0x1);
  vTaskDelay(200 / ( (TickType_t) (1 << (0 /* portUSE_WDTO to use the Watchdog Timer for xTaskIncrementTick*/ + 4)) ) /* Inaccurately assuming 128 kHz Watchdog Timer.*/); //Cada unidade passada representa portTICK_PERIOD_MS (contante igual a 15) milisegundos
  digitalWrite(LEDAz,0x0);
  vTaskDelay(200 / ( (TickType_t) (1 << (0 /* portUSE_WDTO to use the Watchdog Timer for xTaskIncrementTick*/ + 4)) ) /* Inaccurately assuming 128 kHz Watchdog Timer.*/);
}
