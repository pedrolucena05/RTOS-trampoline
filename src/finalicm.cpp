#include "tpl_os.h" 
#include <mcp_can.h>
#include <SPI.h>
#include "Board.h"

// Variáveis para armazenar informações do frame recebido
unsigned char mDLC = 0;
unsigned char mDATA[8];
unsigned long mID = 0;

double rotacao_motor = 0.0;
double velocidade = 0.0;
unsigned char marcha_atual = 0;

// Variáveis de controle para rastrear o estado da sequência
bool receivedRotacao = false;
bool receivedMarcha = false;
bool receivedVelocidade = false;

// Constroi um objeto MCP_CAN e configura o chip selector para o pino 10
MCP_CAN CAN1(ECU1_CAN1_CS);

void setup() {
    Serial.begin(115200);

    while (Serial.available() > 0) {
        Serial.read();
        delay(10);
    }

    while (CAN1.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
        delay(200);
    }

    Serial.println("Modulo CAN inicializado!");
    CAN1.setMode(MCP_NORMAL);
    pinMode(ECU1_CAN1_INT, INPUT);
}

TASK(ReceiveCanFrame) {
    unsigned int msData = 0, lsData = 0;

    while (!digitalRead(ECU1_CAN1_INT)) {
        CAN1.readMsgBuf(&mID, &mDLC, mDATA);

        // Processa a mensagem de "Marcha"
        if ((mID & CAN_EXTENDED_ID) == 418383107UL && !receivedMarcha) {
            marcha_atual = mDATA[4];
            receivedMarcha = true;
            Serial.println("Marcha atual");
        }

        // Processa a mensagem de "Rotação do motor"
        if ((mID & CAN_EXTENDED_ID) == 217056256UL && !receivedRotacao) {
            msData = mDATA[6];
            lsData = mDATA[5];
            rotacao_motor = ((msData << 8) | lsData) * 0.125;
            receivedRotacao = true;
            Serial.println("Rotacao do motor");
        }

        // Processa a mensagem de "Velocidade"
        if ((mID & CAN_EXTENDED_ID) == 419361024UL && !receivedVelocidade) {
            msData = mDATA[3];
            lsData = mDATA[2];
            velocidade = ((msData << 8) | lsData) * 0.00390625;
            receivedVelocidade = true;
            Serial.println("Velocidade");
        }

        // Exibir valores quando todas as mensagens forem recebidas
        if (receivedRotacao && receivedMarcha && receivedVelocidade) {
            // Exibindo na ordem correta (rotacao, velocidade, marcha)
            Serial.print("Rotacao do motor (RPM): ");
            Serial.print(rotacao_motor, 0);
            Serial.print(" | Velocidade do veiculo (KM/h): ");
            Serial.print(velocidade, 0);
            Serial.print(" | Marcha atual: ");
            Serial.println(marcha_atual);

            // Resetar as variáveis para o próximo ciclo de leitura
            receivedRotacao = false;
            receivedMarcha = false;
            receivedVelocidade = false;
        }

    }

    delayMicroseconds(500);
    TerminateTask();
}
