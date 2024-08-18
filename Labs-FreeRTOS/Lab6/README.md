# Lab 06 - Usando Grupos de Eventos

## Descrição

Desenvolva um programa utilizando o FreeRTOS para o ESP32 que:
- Execute duas tarefas (1 e 2) com loop infinito e uma interrupção que execute uma função a cada 1 segundo.
- A função de tratamento de interrupção deve coordenar o funcionamento das duas tarefas, liberando-as para executar através de grupos de eventos.
- A cada 3 segundos a tarefa 1 é liberada para fazer uma leitura do ADC e envia o dado para tarefa 2 usando um fila.
- A cada 15 segundos a tarefa 2 é liberada para fazer a média dos valores recebidos e ligar ou apagar um led se o valor ultrapassar um limiar.
- Perceba que nos instantes multiplos de 15s, as duas tarefas executam.

Certifique-se que o sistema funciona utilizando um potenciômetro conectado ao ADC.

---

## Link da simulação no Wokwi disponível em:

- Lab6: https://wokwi.com/projects/406369442197766145

Funcionamento:

Task 1: Lê o valor do ADC (potenciômetro) a cada 3 segundos e envia o valor para uma fila.

Task 2: Calcula a média dos valores recebidos da fila a cada 15 segundos e controla o LED com base no limiar definido para ativar o led.

Timer: Dispara a cada 1 segundo e libera as tasks 1 e 2 nos intervalos apropriados (a cada 3 e 15 segundos, respectivamente) através de um grupo de eventos.