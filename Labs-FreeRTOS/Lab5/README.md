# Lab 05 - Usando Filas (Queues)

## Descrição

Desenvolva um programa utilizando o FreeRTOS para o ESP32 que:
- Execute duas tarefas simultâneas.
- A tarefa 1 deve coletar dados de um ADC a um taxa de amostragem 100 Hz. A cada amostra realizada, o dado deve ser enviado para a tarefa 2 através de uma fila (Queue).
- A tarefa 2, ao receber um dado novo, deve fazer coloca-lo em um vetor. A cada 100 dados recebidos o sistema deve mostrar a média do vetor na tela.

Certifique-se que o sistema funciona utilizando um potenciômetro conectado ao ADC.

---

## Link da simulação no Wokwi disponível em:

- Lab5: https://wokwi.com/projects/406367443314941953

Funcionamento das Tasks:

Task 1 (collectData): Coleta dados de um ADC a uma taxa de 100 Hz e envia os dados para uma fila (adcQueue). O ciclo de coleta de dados está sendo controlado por vTaskDelayUntil, que garante a taxa de 100 Hz.

Task 2 (processData): Recebe os dados da fila e armazena em um vetor. A cada 100 dados recebidos, calcula e exibe a média dos valores no vetor.