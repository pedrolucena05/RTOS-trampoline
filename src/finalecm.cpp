
#include "tpl_os.h"  
#include <mcp_can.h>
#include <SPI.h>
#include "Board.h"
#include <avr/pgmspace.h>


void enviarMensagemParaCAN(unsigned long id, unsigned char* data);
void enviarMensagemER();
void enviarMensagemVV();

double engine_tx = 0.0;
unsigned char contador = 0;
unsigned char ciclos = 0;
unsigned char ig = 1;
double velocidade = 0.0;
unsigned char flag = 0;
bool estorou_limite = false;

const int taxa_marcha[5] PROGMEM = {383, 263, 169, 131, 100};
const int maxima_rotacao[5] PROGMEM = {4000, 4800, 5600, 6400, 7200};
const unsigned int sequencia[20] PROGMEM = {0, 5600, 11200, 16800, 22400, 28000, 33600, 39200, 44800, 50400, 56000 , 50400, 44800, 39200, 33600, 28000, 22400, 16800, 11200, 5600};

//Variavel para armazenar informacoes do frame recebido
unsigned char mDLC = 0;
unsigned char mDATA[8];
unsigned long mID = 0;

//Constroi um objeto MCP_CAN e configura o chip selector para o pino 10.
MCP_CAN CAN1(ECU2_CAN1_CS);

void setup() {

    // Inicializa a interface serial : baudrate = 9600
    Serial.begin(115200);
    
    while (Serial.available() > 0) {
        Serial.read();
        delay(10);
    }

    // Inicializa o controlador can : baudrate = 250K, clock=8MHz
    while(CAN1.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
        delay(200);
    }
    
    Serial.println ("ECM inicializado com sucesso! 22EN");
    
    CAN1.setMode(MCP_NORMAL);
    pinMode(ECU2_CAN1_INT, INPUT);
    
}

TASK (SendErFrame) {

    ciclos = ciclos + 1;
    
    GetResource(VariaveisCompartilhadas);
    if (ciclos == 4) {
        enviarMensagemER();

    }
    else if (ciclos == 8) {
        enviarMensagemVV();
    }
    else if (ciclos == 10) {
        ciclos = 0;
    }

    ReleaseResource(VariaveisCompartilhadas);
    TerminateTask();

}

TASK (ReceiveFrame) {
    
    GetResource(VariaveisCompartilhadas);
    
    //Se uma interrupção ocorreu interrupção (ECU4_CAN1_INT pino = 0), lê o buffer de recepção
    while (!digitalRead(ECU2_CAN1_INT)) {
    //Lê os dados: mID = identificador, mDLC = comprimento, mDATA = dados do freame
        CAN1.readMsgBuf(&mID, &mDLC, mDATA);
        Serial.print("Mensagem CAN Recebida.");
        
        // mensagem de ignicao                                                                                       
        if ((mID & CAN_EXTENDED_ID) == 418383107UL) {
            ig = mDATA[4];
            Serial.print("Ignicao: ");
            Serial.println(ig);
            contador = mDATA[0];
        }
    }
    ReleaseResource(VariaveisCompartilhadas);

    TerminateTask();
}

void enviarMensagemER() {
    unsigned char ECM_data_ER[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned char lsData = 0, msData = 0;

    if (estorou_limite) {
        engine_tx = 5600*contador;
        estorou_limite = false;
    }
    


    engine_tx = pgm_read_word(&sequencia[contador]);

    lsData = (unsigned int)engine_tx & 0xFF;
    msData = ((unsigned int)engine_tx >> 8) & 0xFF;

    ECM_data_ER[5] = lsData;
    ECM_data_ER[6] = msData;

    // Chama a função para enviar a mensagem

    Serial.print("Rotacao: ");
    Serial.println(engine_tx);
    
    enviarMensagemParaCAN(217056256UL, ECM_data_ER);
    
}

void enviarMensagemVV() {
    
    unsigned char ECM_data_VV[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned char msData = 0, lsData = 0;

    double taxa_ig = (double)pgm_read_word(&taxa_marcha[ig - 1]) / 100.0;
    velocidade = (0.326 * (engine_tx*0.125)) / (3.55 * taxa_ig);
    velocidade = velocidade*71.168; // 71.168 = resolucao(256) / 3.6

    lsData = (unsigned int)velocidade & 0xFF; 
    msData = ((unsigned int)velocidade >> 8) & 0xFF;

    ECM_data_VV[2] = lsData; 
    ECM_data_VV[3] = msData;
    
    enviarMensagemParaCAN(419361024UL, ECM_data_VV);
        
}

void enviarMensagemParaCAN(unsigned long id, unsigned char* data) {
    unsigned char ret;

    // Envia a mensagem para o barramento CAN
    ret = CAN1.sendMsgBuf(id, 1, 8, data);

    // Verifica o status do envio
    if (ret == CAN_OK) {
        Serial.println("ECM: mensagem transmitida com sucesso! Pedro Lucena ");
        Serial.print("Id da mensagem: ");
        Serial.println(id);
    } else if (ret == CAN_SENDMSGTIMEOUT) {
        Serial.println("ECM: Message timeout! NN");
    } else {
        Serial.println("ECM: Error to send! NN");
    }
    
}