# Aplicação RTOS para Arduino com Comunicação CAN entre ECUs

Projeto desenvolvido durante a Residência Tecnológica em Software Automotivo da UFPE para consolidar conhecimentos de sistemas embarcados, RTOS, comunicação automotiva e restrições de hardware.

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
