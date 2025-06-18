# Exemplo de uma aplicação RTOS para o Arduino usando a biblioteca Trampoline

Aplicação desenvolvida para consolidar conhecimentos de RTOS na Residência Tecnológica da UFPE. O sistema envolve a comunicação entre 3 placas via rede CAN, utilizando uma placa artesanal com 4 Arduinos Nano, todos com o microprocessador Atmega328p (8 bits e 2 KB de RAM).

## O projeto

O projeto consiste em simular a comunicação entre 3 ECUs de um carro, sendo:

1. **TCM** – Responsável pelo envio da marcha atual;
2. **ECM** – Responsável pelo envio da rotação do motor e cálculo da velocidade do veículo;
3. **ICM** – Responsável por exibir as três informações sincronizadas: marcha atual, rotação e velocidade.

## Fluxo do algoritmo

1. Um ciclo de 500 ms é iniciado;
2. A **TCM** envia a marcha atual do veículo pela rede CAN;
3. A **ECM** calcula e envia a rotação do motor;
4. A **ECM** calcula e envia a velocidade baseada na rotação e marcha atuais;
5. A **ICM** exibe, via porta serial, os dados de marcha, rotação do motor e velocidade.

## Execução do projeto

Após configurar o ambiente e concluir o algoritmo, o arquivo `.hex` de cada placa é gerado e enviado remotamente para o desktop onde está conectado o Arduino correspondente.

## Projeto em execução

[🔗 Link para o vídeo de execução](https://drive.google.com/file/d/1UwUIkrjhLCyTlZAEDUMWRYV9XebpAj96/view?usp=sharing)
