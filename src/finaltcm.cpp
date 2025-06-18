#include "tpl_os.h" 
#include <mcp_can.h>
#include <SPI.h>
#include "Board.h"

void enviarMensagemParaCan();

const int sequencia[20] PROGMEM = {1, 1, 1, 1, 1, 1, 2, 3, 3, 4, 5, 4, 3, 3, 2, 1, 1, 1, 1, 1};

// Variáveis para armazenar informações do frame recebido
unsigned char mDLC = 0;
unsigned char mDATA[8];
unsigned long mID = 0;
unsigned char contador = 0;
unsigned char ciclos = 0;

// Variáveis que armazenam o FRAME_DATA
unsigned char tcm_data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
char msgString[7];
unsigned char ig = 0;

// Constroi um objeto MCP_CAN
MCP_CAN CAN1(ECU3_CAN1_CS);

void setup() {
    Serial.begin(115200);

    while (Serial.available() > 0) {
        Serial.read();
        delay(10);
    }

    // Inicializa o controlador CAN: baudrate = 500K, clock = 8MHz
    while (CAN1.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
        delay(200);
    }
    Serial.println("TCM inicializado com sucesso!");

    CAN1.setMode(MCP_NORMAL);
    pinMode(ECU3_CAN1_INT, INPUT);
}

// Tarefa SendIgFrame que envia as mensagens de acordo com o contador
TASK(SendIgFrame) {
    // Atualiza o contador de forma linear
    ciclos++;

    
    if (ciclos == 2) {
        enviarMensagemParaCan();
    }

    else if (ciclos == 10) {
        ciclos = 0;
    }

    TerminateTask();
}

// Função para enviar a mensagem para o barramento CAN
void enviarMensagemParaCan() {
    static byte ret = 0;
    
    contador++;
    if (contador >= 20) {
        contador = 0; // Reseta o contador quando atingir o fim da sequência
    }


    // Leitura da sequência da marcha com pgm_read_word()
    ig = pgm_read_word(&sequencia[contador]);

    // Preenche os dados do frame para o CAN
    tcm_data[4] = ig;
    tcm_data[0] = contador;

    // Envia a mensagem para o barramento CAN
    ret = CAN1.sendMsgBuf(418383107UL, 1, 8, tcm_data);
    if (ret == CAN_OK) {
        Serial.println("TCM: mensagem transmitida com sucesso!");
        Serial.print("Marcha: ");
        Serial.println(ig);
    } else if (ret == CAN_SENDMSGTIMEOUT) {
        Serial.println("TCM: Message timeout!");
    } else {
        Serial.println("TCM: Error to send!");
    }
}
