 Cliente MQTT Multicore com Raspberry Pi Pico W

Este projeto avançado para o Raspberry Pi Pico W implementa um cliente MQTT robusto, utilizando os dois núcleos do processador RP2040 para dividir as responsabilidades e garantir um funcionamento estável e sem bloqueios.

A arquitetura foi desenhada para separar as tarefas de conectividade Wi-Fi (que podem ter latências imprevisíveis) das tarefas de controle principal do sistema, uma prática comum em sistemas embarcados de alta confiabilidade.

 Arquitetura do Projeto

O sistema opera com uma clara divisão de trabalho entre os dois núcleos do processador:

 Núcleo 1 (`core1`): O Especialista em Conectividade
Responsabilidade Única:** Gerenciar a conexão Wi-Fi.
Funções:
     Tenta se conectar à rede Wi-Fi configurada.
     Monitora o status da conexão continuamente.
     Comunica o status da conexão (ex: "Conectando...", "Conectado", "Falha") e o endereço de IP obtido para o Núcleo 0 através de uma fila FIFO segura.

 Núcleo 0 (`core0`): O Cérebro Principal
Responsabilidade:** Orquestrar o sistema, processar dados e gerenciar a comunicação MQTT.
Funções:
     Inicializa todo o hardware (display OLED, LEDs).
     "Acorda" o Núcleo 1 para que ele comece a trabalhar.
     Fica à escuta por mensagens enviadas pelo Núcleo 1 via FIFO.
     Exibe o status da conexão Wi-Fi no display OLED.
     Assim que recebe o endereço de IP do Núcleo 1, ele **inicia o cliente MQTT**.
     Envia mensagens "PING" periódicas para um tópico MQTT, demonstrando que o dispositivo está online.
     Recebe comandos via FIFO para alterar parâmetros em tempo de execução (ex: mudar o intervalo do PING).

 Funcionalidades Principais

Processamento Paralelo:** Uso eficiente dos dois núcleos do RP2040 com a biblioteca `pico/multicore`.
Comunicação Inter-Core Segura:** Utilização de uma fila FIFO (`multicore_fifo`) para a troca de dados entre os núcleos, evitando condições de corrida e corrupção de dados.
Conectividade Robusta:** O cliente MQTT só é iniciado após a confirmação de que uma conexão Wi-Fi válida e um endereço de IP foram obtidos.
Fila de Mensagens:** Implementação de uma fila circular para armazenar e processar os status recebidos do Núcleo 1, garantindo que nenhuma atualização seja perdida.
Interface e Depuração:** Fornece feedback visual em tempo real através de um display OLED e mensagens detalhadas pela porta serial USB.
Controle Dinâmâmico:** Capacidade de alterar o intervalo de envio de mensagens PING em tempo de execução através de comandos via FIFO.

 Hardware Necessário

 1x Raspberry Pi Pico W
 1x Display OLED I2C (ex: SSD1306)
 (Opcional) 1x LED RGB para feedback de status visual.
 Protoboard e Jumpers para a montagem.

 Pinagem (Exemplo)

As conexões exatas dependem do seu ficheiro de configuração (`configura_geral.h`), mas um exemplo seria:

| Componente      | Conexão no Raspberry Pi Pico |
| :-------------- | :--------------------------- |
| Display OLED - SDA | GPIO 14 (ou outro pino I2C) |
| Display OLED - SCL | GPIO 15 (ou outro pino I2C) |
| Display OLED - VCC | 3.3V |
| Display OLED - GND | GND  |

Como Compilar e Usar

Para compilar este projeto, é necessário ter o ambiente de desenvolvimento para o Raspberry Pi Pico C/C++ SDK configurado, incluindo as bibliotecas `pico_stdlib`, `hardware_i2c`, e `pico_multicore`.

1.  Configure o `CMakeLists.txt`:** Garanta que todas as bibliotecas necessárias, incluindo as do FreeRTOS (se aplicável) e as bibliotecas locais (`fila_circular`, `oled_utils`, etc.), estejam corretamente listadas no seu `CMakeLists.txt`.
2.  Compile o Projeto:**
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
3.  Grave no Pico:** Pressione o botão BOOTSEL no seu Pico W, conecte-o ao computador e arraste o ficheiro `.uf2` gerado para a nova unidade de disco que aparecer.

4.  Monitore a Saída:** Use um terminal serial (como o `minicom` no Linux ou o monitor serial da IDE do Arduino) para ver as mensagens de status e depuração do dispositivo.

 Possíveis Melhorias

Reconexão Automática do MQTT:** Implementar uma lógica no Núcleo 0 para tentar reconectar o cliente MQTT caso a conexão com o broker caia.
Comandos MQTT:** Fazer o Núcleo 0 se inscrever em um tópico de "comando" para receber mensagens (ex: `pico/comando`) e tomar ações, como alterar o estado de um pino GPIO.
Integração com Sensores:** Adicionar a leitura de um sensor (como o AHT10) no Núcleo 0 e publicar os dados via MQTT, transformando o dispositivo num nó sensor completo para automação residencial.