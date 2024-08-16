# Lab 04 - Usando semáforos

## Descrição

Desenvolva um programa utilizando o FreeRTOS para o ESP32 que:
- Execute duas tarefas simultâneas.
- A tarefa 1 deve coletar monitorar um botão, quando o botão for apertado ela deve enviar uma sinalização por um semáforo binário para a tarefa 2.
- A tarefa 2, ao receber a sinalização do semáforo, deve fazer uma coleta de um ADC e exibir o resultado na tela.

---

## Link da simulação no Wokwi disponível em:

- Lab4: https://wokwi.com/projects/406337968407684097

Funcionamento das Tasks:

Task 1 (monitor_button): Monitora o estado do botão conectado ao pino 12. Quando o botão é pressionado, a tarefa envia um sinal pelo semáforo binário (xSemaphoreGive).

Task 2 (read_adc): Fica aguardando a sinalização do semáforo. Quando a task 1 sinaliza que o botão foi pressionado, essa tarefa realiza a leitura do ADC no pino 4 e imprime o valor no monitor serial.
