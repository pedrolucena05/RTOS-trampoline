# Aplicação RTOS para Arduino com Comunicação CAN entre ECUs

Projeto desenvolvido durante a Residência Tecnológica em Software Automotivo da UFPE para consolidar conhecimentos de sistemas embarcados, sistemas operacionais de tempo real (RTOS), comunicação automotiva e desenvolvimento sob restrições de hardware.

A aplicação simula a troca de informações entre três ECUs de um veículo por meio de uma rede CAN. Cada ECU foi executada em uma placa Arduino Nano com microcontrolador **ATmega328P**, uma arquitetura de 8 bits com apenas **2 KB de memória RAM**, utilizando o RTOS Trampoline que eram gravados remotamente nas placas Arduino que ficam no laboratório do CIn UFPE.

---

## Visão geral

O projeto simula uma arquitetura distribuída semelhante à utilizada em veículos, na qual diferentes módulos eletrônicos trocam informações pelo barramento CAN.

As ECUs implementadas foram:

| ECU     | Nome                        | Responsabilidade                                                                                       |
| ------- | --------------------------- | ------------------------------------------------------------------------------------------------------ |
| **TCM** | Transmission Control Module | Enviar a marcha atual do veículo.                                                                      |
| **ECM** | Engine Control Module       | Receber a marcha, calcular a rotação do motor e estimar a velocidade do veículo.                       |
| **ICM** | Instrument Control Module   | Receber os dados da rede CAN, desempacotá-los e exibir marcha, rotação e velocidade pela porta serial. |

---

## Restrições de hardware e gerenciamento de memória

Cada Arduino Nano utilizou um microcontrolador **ATmega328P**, com arquitetura de 8 bits e apenas **2 KB de RAM**.

O Trampoline RTOS ocupava aproximadamente metade dessa memória, deixando cerca de **1 KB disponível para a aplicação**. Além das variáveis globais, buffers e estruturas de comunicação, cada ECU possuía pelo menos duas tarefas configuradas com **256 bytes de stack** cada.

Esse cenário exigiu decisões cuidadosas relacionadas a:

* tamanho e tipo das variáveis;
* uso de variáveis inteiras de 8 e 16 bits;
* dimensionamento da stack das tasks;
* Uso do PROGMEM para guardar vetores diretamente na memoria flash.

---

## Comunicação por rede CAN

A comunicação entre as ECUs foi realizada por meio de **CAN frames**.

- Quando um CAN frame é enviado todas as placas recebem e armazenam o frame num buffer (podendo ter mais frames guardados em fila);
- Cicle time e alarm time das tasks foram utilizadas para orquestrar o envio e recebimento dos CAN frames, sincronizando as placas corretamente;

---

## Empacotamento e desempacotamento dos dados

Como o ATmega328P trabalha com arquitetura de 8 bits, valores maiores precisaram ser divididos em bytes menores antes de serem enviados pela rede CAN.

Além disso, valores originalmente calculados como ponto flutuante precisaram ser convertidos para inteiros, considerando a resolução definida para cada informação transmitida.

| Informação            | Estrutura de transmissão | Faixa de resolução |
| --------------------- | ------------------------ | ------------------ |
| Marcha                | 1 variável de 8 bits     | 0 a 255            |
| Rotação do motor      | 2 variáveis de 8 bits    | 0 a 65.535         |
| Velocidade do veículo | 2 variáveis de 8 bits    | 0 a 65.535         |

A estrutura de transmissão utilizou a separação entre:

* **MSB — Most Significant Byte:** byte mais significativo;
* **LSB — Least Significant Byte:** byte menos significativo.
* No envio, valores de 16 bits eram separados em dois bytes para compor o payload do CAN frame.
* Na recepção, a ECU realizava o processo inverso, combinando MSB e LSB para reconstruir o valor original.

Esse processo permitiu uma transmissão compacta e adequada às limitações de memória e processamento do microcontrolador.

---

## Frames CAN utilizados

| Informação            | ECU transmissora | Momento do ciclo | ID decimal | ID hexadecimal | Resolução  |
| --------------------- | ---------------- | ---------------- | ---------- | -------------- | ---------- |
| Marcha atual          | TCM              | 100 ms           | 418383107  | `0x18F00503`   | 0 a 255    |
| Recepção da marcha    | ECM              | 150 ms           | -          | -              | -          |
| Rotação do motor      | ECM              | 200 ms           | 217056256  | `0x0CF00400`   | 0 a 65.535 |
| Velocidade do veículo | ECM              | 300 ms           | 419361024  | `0x18FEF100`   | 0 a 65.535 |
| Recepção do ICM       | ICM              | 500 ms           | -          | -              | -          |

---

## Fluxo de execução

A aplicação foi organizada em ciclos periódicos de aproximadamente **500 ms**.

```text
100 ms  → TCM transmite a marcha atual pela rede CAN

150 ms  → ECM recebe e processa a marcha

200 ms  → ECM transmite a rotação do motor em RPM

300 ms  → ECM calcula a velocidade com base na marcha e na rotação do motor
          e transmite a velocidade pela rede CAN

500 ms → ICM lê os buffers CAN, desempacota os dados
                 e exibe marcha, RPM e velocidade pela porta serial
```

A velocidade do veículo foi calculada pela ECM utilizando a marcha atual e a rotação estimada do motor.

---

## Principais desafios técnicos

* Desenvolvimento para microcontrolador ATmega328P de 8 bits com apenas 2 KB de RAM.
* Uso de aproximadamente 1 KB de RAM disponível para a aplicação após o carregamento do RTOS.
* Dimensionamento de stack para múltiplas tarefas concorrentes.
* Organização de tarefas periódicas utilizando Trampoline RTOS.
* Configuração de alarmes e tempos de ciclo para sincronização entre três placas.
* Controle de atrasos e monitoramento de possíveis timeouts entre tarefas.
* Comunicação entre três placas Arduino por meio de rede CAN.
* Leitura de mensagens a partir dos buffers de recepção CAN.
* Estruturação de frames CAN com identificadores em representação hexadecimal.
* Empacotamento e desempacotamento de valores utilizando MSB e LSB.
* Conversão de valores de ponto flutuante para inteiros conforme a resolução de transmissão.
* Uso de variáveis inteiras de 8 e 16 bits para reduzir o consumo de memória.
* Sincronização de marcha, rotação do motor e velocidade entre ECUs.
* Uso de temporização para preservar a previsibilidade da execução das tarefas.

---

## Tecnologias e conceitos utilizados

```text
C/C++
Arduino Nano
ATmega328P
Trampoline RTOS
Rede CAN
CAN Frames
ECUs automotivas
Sistemas embarcados
Arquitetura de 8 bits
MSB / LSB
Serial Monitor
Controle de tempo real
Buffers de recepção CAN
Empacotamento de dados
```

---

## Execução do projeto

Após a configuração do ambiente e a implementação do algoritmo, os arquivs .oil e .cpp são compilados para gerar o `.hex` específico para cada ECU (TCM, ECM, ICM).

Os arquivos compilados eram enviados remotamente para o desktop onde cada placa correspondente estava conectada, permitindo a gravação e execução distribuída das ECUs simuladas.

---

## Projeto em execução

> [🔗 Link para o vídeo de execução](https://drive.google.com/file/d/1vNMXSnivWpOF6JQkKgDDDEVOV0rcgod9/view?usp=sharing)
