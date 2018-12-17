/**
 * CTEMPO_MAX_ontrole do Galinheiro
 *
 * @author Rafael Lima
 * @version 0.2 - "Definido Estrutura Básica"
 */

// Bibliotecas --------------------------------------------------------------
#include <elapsedMillis.h>

//-- Constantes -------------------------------------------------------------
// Portas Usadas
#define PORTA_ABRIR_PORTAO        2
#define PORTA_FECHAR_PORTAO       3
#define PORTA_LEVANTAR_COMEDOURO  4
#define PORTA_ABAIXAR_COMEDOURO   5

#define PORTA_LED  13

#define PORTA_SENSOR_LUZ          A0

// Estados
#define ESTADO_NOITE              0
#define ESTADO_NASCER_DO_SOL      1
#define ESTADO_DIA                2
#define ESTADO_POR_DO_SOL         3

// Estado Atuadores
#define COMEDOURO_BAIXO  0
#define COMEDOURO_ALTO   1
#define PORTA_ABERTA     0
#define PORTA_FECHADA    1
#define DIA              1
#define NOITE            0

// Constantes Sensores
#define MINIMO_LUZ         300
#define TEMPO_MINIMO_DIA   300     // ms
#define TEMPO_MINIMO_NOITE 300     // ms

#define TEMPO_MAX_ESTADO_NOITE          1000 // ms
#define TEMPO_MAX_ESTADO_NASCER_DO_SOL  1000 // ms
#define TEMPO_MAX_ESTADO_DIA            1000 // ms
#define TEMPO_MAX_ESTADO_POR_DO_SOL     1000 // ms

// Variáveis ----------------------------------------------------------------
int estadoAtual;

int estadoPorta;
int estadoComedouro;
int estadoDiaNoite;

int leituraSensorLuminosidade;

int duracaoDiaAnterior;
int duracaoNoiteAnterior;

// Temporizadores -----------------------------------------------------------
elapsedMillis timerAcionamentoPorta;
elapsedMillis timerAcionamentoComedouro;
elapsedMillis timerDuracaoDia;
elapsedMillis timerDuracaoNoite;
elapsedMillis timerEstadoAtual;

// Rotinas Principais  ------------------------------------------------------

/**
 * Rotina Executada uma única vez ao ligar Arduino
 */
void setup() {

  // Inicia Comunicação pela Serial
  Serial.begin(9600);

  // Define Estado Inicial
  estadoAtual = ESTADO_NOITE;

  // Define Estado Inicial Atuadores
  estadoPorta = PORTA_FECHADA;
  estadoComedouro = COMEDOURO_BAIXO;

  // Desliga Acionamentos por Segurança
  digitalWrite(PORTA_ABRIR_PORTAO,LOW);
  digitalWrite(PORTA_FECHAR_PORTAO,LOW);
  digitalWrite(PORTA_LEVANTAR_COMEDOURO,LOW);
  digitalWrite(PORTA_ABAIXAR_COMEDOURO,LOW);

  //Reinicia timers
  timerEstadoAtual = 0;
}

/**
 * Loop Infinito da Rotina Principal
 */
void loop() {
  // Verifica Sensores
  int leituraSensorLuminosidade = analogRead(PORTA_SENSOR_LUZ);

  /**
   * Detecta Dia e Noite
   * @todo Incorporar Tempo mínimo para detectar dia
   */
  if(leituraSensorLuminosidade>=MINIMO_LUZ){
    estadoDiaNoite = DIA;
    timerDuracaoDia = 0; // Reinicia Contagem de Tempo
  }else{
    estadoDiaNoite = NOITE;
    timerDuracaoNoite = 0; // Reinicia Contagem de Tempo
  }

  /**
   * Define Estado da Porta e do Comedouro de acordo com momento do dia
   * @todo Avaliar Transição entre estados
   */
  switch(estadoAtual){
    case ESTADO_NASCER_DO_SOL:
      estadoPorta = PORTA_FECHADA;
      estadoComedouro = COMEDOURO_ALTO;

      // Muda para Próximo Estado
      if(timerEstadoAtual>TEMPO_MAX_ESTADO_NASCER_DO_SOL){
          estadoAtual = ESTADO_DIA;
          timerEstadoAtual = 0;
      }
      break;

    case ESTADO_DIA:
      estadoPorta = PORTA_ABERTA;
      estadoComedouro = COMEDOURO_ALTO;

      // Muda para Próximo Estado
      if((estadoDiaNoite==NOITE)||(timerEstadoAtual>TEMPO_MAX_ESTADO_DIA)){
          estadoAtual = ESTADO_POR_DO_SOL;
          timerEstadoAtual = 0;
      }
      break;

    case ESTADO_POR_DO_SOL:
      estadoPorta = PORTA_ABERTA;
      estadoComedouro = COMEDOURO_BAIXO;

      // Muda para Próximo Estado
      if(timerEstadoAtual>TEMPO_MAX_ESTADO_POR_DO_SOL){
        estadoAtual = ESTADO_NOITE;
        timerEstadoAtual = 0;
      }
      break;

    case ESTADO_NOITE:
    default:
      estadoPorta = PORTA_FECHADA;
      estadoComedouro = COMEDOURO_BAIXO;

      // Muda para Próximo Estado
      if((estadoDiaNoite==DIA)||(timerEstadoAtual>TEMPO_MAX_ESTADO_NOITE)){
        estadoAtual = ESTADO_NASCER_DO_SOL;
        timerEstadoAtual = 0;
      }
      break;
  }

  // Aciona Atuadores caso necessário
  movePortao(estadoPorta);
  moveComedouro(estadoComedouro);

  // Imprime Situação Atual para Debug
  imprimeEstados(estadoAtual,estadoDiaNoite,estadoComedouro,estadoPorta);
}

// Rotinas Auxiliares -------------------------------------------------------

/**
 * Comandos para Mover o Portão de Entrada
 *
 * @todo Incorporar estados intermediários para sinalizar motor em movimento
 */
void movePortao(int estadoPorta){
    switch(estadoPorta){
      case PORTA_FECHADA:
        // Fecha Porta
        digitalWrite(PORTA_ABRIR_PORTAO,LOW);
        digitalWrite(PORTA_FECHAR_PORTAO,HIGH);
        break;

      case PORTA_ABERTA:
        // Abre Porta
        digitalWrite(PORTA_ABRIR_PORTAO,HIGH);
        digitalWrite(PORTA_FECHAR_PORTAO,LOW);
        break;

      default:
        // Desliga Motor
        digitalWrite(PORTA_ABRIR_PORTAO,LOW);
        digitalWrite(PORTA_FECHAR_PORTAO,LOW);
    }
}

/**
 * Comandos para Mover o Comedouro
 *
 * @todo Incorporar estados intermediários para sinalizar motor em movimento
 */
void moveComedouro(int estadoComedouro){
    switch(estadoComedouro){
      case COMEDOURO_BAIXO:
        // Abaixar Comedouro
        digitalWrite(PORTA_LEVANTAR_COMEDOURO,LOW);
        digitalWrite(PORTA_ABAIXAR_COMEDOURO,HIGH);
        break;

      case COMEDOURO_ALTO:
         // Levantar Comedouro
        digitalWrite(PORTA_LEVANTAR_COMEDOURO,HIGH);
        digitalWrite(PORTA_ABAIXAR_COMEDOURO,LOW);
        break;

      default:
        // Desliga Motor
        digitalWrite(PORTA_LEVANTAR_COMEDOURO,LOW);
        digitalWrite(PORTA_ABAIXAR_COMEDOURO,LOW);
    }
}

/**
 * Imprime Variáves de Estado e temporizadores na serial para Debug
 */
void imprimeEstados(int estadoAtual, int estadoDiaNoite, int estadoComedouro,int estadoPorta){
  Serial.print(millis());
  Serial.print(" , ");
  Serial.print(timerEstadoAtual);
  Serial.print(" , ");
  Serial.print(estadoAtual);
  Serial.print(" , ");
  Serial.print(estadoComedouro);
  Serial.print(" , ");
  Serial.println(estadoPorta);
}

/**
 * Debug Function - Blink Led 'n' times
 * @author Rafael
 */
void blink(int n,int tempo){
  int k;
  for(k=(n>=0)?n:0;k<n;k++){
    digitalWrite(PORTA_LED,HIGH);
    delay(tempo);
    digitalWrite(PORTA_LED,LOW);
    delay(tempo);
  }
}
