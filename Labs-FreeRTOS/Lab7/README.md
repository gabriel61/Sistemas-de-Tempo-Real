# Lab 07 - Notificações

## Descrição

Desenvolva um programa utilizando o FreeRTOS para o ESP32 que:
- Execute duas tarefas (1 e 2) com loop infinito e uma interrupção que execute uma função quando o botão for apertado.
- A função de tratamento de interrupção deve enviar uma notificação para as outras duas tarefas do acontecimento do evento.
- Quando receber a notificação, a tarefa 1 deve mudar o estado de um LED.
- Quando receber a notificação, a tarefa 2 deve mandar uma mensagem pelo terminal indicando que o botão foi apertado.

---

## Link da simulação no Wokwi disponível em:

- Lab7: https://wokwi.com/projects/406633816034379777

Funcionamento:

Task 1: Altera o estado de um LED (ligado/desligado) quando recebe uma notificação.

Task 2: Exibe uma mensagem no terminal serial indicando que o botão foi pressionado quando recebe uma notificação.

Debounce: Implementado para evitar múltiplas notificações causadas pelo efeito "bouncing" do botão.