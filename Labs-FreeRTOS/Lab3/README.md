# Lab 03 - Tarefas no FreeRTOS

## Parte 1

Desenvolva um programa utilizando o FreeRTOS para o ESP32 que
- Execute três tarefas simultâneas
- Cada tarefa deverá ter loops infinitos e piscar um led
- Os leds deverão piscar com períodos de 1000ms, 500ms e 250ms.

## Parte 2

Modifique o código da parte 1 para:
- Além de piscar o led, a tarefa 1 (período=1000ms) deve ler um botão e, se acionado, pausar/resumir a tarefa 2.
- Além de piscar o led, a tarefa 2 (período=500ms) deve ler um botão e, se acionado, pausar/resumir a tarefa 3.

---

## Links das simulações no Wokwi disponível em:

- Parte 1:   https://wokwi.com/projects/406256247696776193
- Parte 2.1: https://wokwi.com/projects/406334924460491777
- Parte 2.2: https://wokwi.com/projects/406261536799763457

Os códigos da Parte 2.1 e Parte 2.2 implementam a mesma funcionalidade. A diferença é que na Parte 2.1, o ambiente de simulação apresentou travamentos devido ao fato de uma única task estar controlando duas tarefas, o que afetou a leitura dos botões. Para resolver esse problema, na Parte 2.2, criei duas novas tasks específicas para os botões, o que resultou em um funcionamento mais estável no Wokwi, sem travamentos.
