# Detecção Sonora e Análise de Frequência com ESP32

Este projeto implementa um sistema de detecção sonora e análise de frequência utilizando o microcontrolador ESP32, que captura sinais de áudio através do microfone I2S, aplica uma FFT (Transformada Rápida de Fourier) para identificar frequências dominantes e detecta a presença de som com base em uma média de potência. O projeto é baseado em um executivo cíclico que controla a execução das tarefas em intervalos de tempo regulares.

## Estrutura do Projeto

### Diagrama das Tasks e Ciclos

O projeto segue um modelo cíclico com quatro ciclos de 64 ms cada. As tasks são distribuídas entre esses ciclos da seguinte forma:

```plaintext
┌─────────────────────────────┐
│        Ciclo 1 - 64 ms      │
│  Task: read_i2s_samples()   │
│  Tempo de Execução: 0 ms    │
│  Período: 64 ms             │
│  Utilização: 0%             │
└─────────────┬───────────────┘
              │
┌─────────────▼───────────────┐
│        Ciclo 2 - 128 ms     │
│  Task: read_i2s_samples()   │
│  Task: deteccaoSonora()     │
│  Tempo de Execução: 0.098 ms│
│  Período: 256 ms            │
│  Utilização: 0.038%         │
└─────────────┬───────────────┘
              │
┌─────────────▼───────────────┐
│        Ciclo 3 - 192 ms     │
│  Task: read_i2s_samples()   │
│  Tempo de Execução: 0 ms    │
│  Período: 64 ms             │
│  Utilização: 0%             │
└─────────────┬───────────────┘
              │
┌─────────────▼───────────────┐
│        Ciclo 4 - 256 ms     │
│  Task: read_i2s_samples()   │
│  Task: fftFuction()         │
│  Tempo de Execução: 6 ms    │
│  Período: 256 ms            │
│  Utilização: 2.34%          │
└─────────────────────────────┘

Carga total de CPU: 2.378%

```

### Tasks Utilizadas

| Task                | Período (ms) | Tempo de Execução (ms) | Utilização (%) |
|---------------------|--------------|------------------------|----------------|
| `read_i2s_samples`  | 64           | 0                      | 0%             |
| `deteccaoSonora`    | 256          | 0.098                  | 0.038%         |
| `fftFuction`        | 256          | 6                      | 2.34%          |

### Configurações Principais

- **Frequência de Amostragem**: 16 kHz
- **Tamanho do Buffer de I2S**: 1024 amostras
- **Frequência de Execução da FFT**: 4 vezes por segundo (a cada 256 ms)
- **Detecção Sonora**: Utiliza uma média deslizante com fator multiplicador para detecção de picos de potência.

### Estrutura de Arquivos

- [cyclic_executive/main/main.c](https://github.com/gabriel61/Sistemas-de-Tempo-Real/blob/main/cyclic_executive/main/main.c): Contém o código principal do projeto.

### Como Executar

1. **Clone o repositório**:
    ```bash
    git clone https://github.com/gabriel61/Sistemas-de-Tempo-Real.git
    cd Sistemas-de-Tempo-Real
    ```

2. **Configure o ambiente**: Certifique-se de que o ambiente de desenvolvimento do ESP-IDF está configurado.

3. **Compile o projeto**:
    ```bash
    idf.py build
    ```

4. **Faça o upload para o ESP32**:
    ```bash
    idf.py flash
    ```

5. **Monitore a saída**:
    ```bash
    idf.py monitor
    ```

### Licença

Este projeto está licenciado sob a Licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.

---

### ✒️ Autor

</br>

<a href="https://github.com/gabriel61">
 <img style="border-radius: 50%;" src="https://avatars.githubusercontent.com/gabriel61" width="100px;" alt=""/>
 <br />
 
 [![Linkedin Badge](https://img.shields.io/badge/-gabrielsampaio-blue?style=flat-square&logo=Linkedin&logoColor=white&link=https://www.linkedin.com/in/gabriel-oliveira-852759190/)](https://www.linkedin.com/in/gabriel-oliveira-852759190/)
<br>
sogabris@gmail.com
<br>

---
