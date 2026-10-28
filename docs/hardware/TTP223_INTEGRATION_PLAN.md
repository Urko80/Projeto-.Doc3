# Plano de integração de sensores e barra RGB — Projeto DOC

> Estado: plano técnico atualizado; ainda não implementado, compilado nem testado fisicamente. A PR #13 funcional permanece intacta.

## Objetivo

Integrar, em mudanças isoladas, o TTP223, o VL53L0X e a barra RGB WS2812B de 8 LEDs, preservando o firmware funcional da PR #13.

## Decisão de pinagem para a barra WS2812B

A barra informada tem pinos `GND, IN, VCC, GND` no lado de entrada e `GND, OUT, VCC, GND` no lado de saída. `IN` recebe os dados; `OUT` é a saída para encadear outro módulo.

**Proposta de trabalho: usar GPIO48 como sinal de dados compartilhado com o LED RGB integrado, e não para o TTP223.** O firmware DOC atual define `BUILTIN_LED_GPIO GPIO_NUM_NC`, então não há indicação nesta configuração de que o software já controle o LED integrado. Como o LED integrado e a barra receberiam o mesmo fluxo de dados em paralelo, o LED integrado poderá espelhar o primeiro pixel da barra. Isso é uma consequência prevista, não controle independente.

Esta opção evita reservar GPIO43, que normalmente é UART0 TX e pode ser usado pelo adaptador USB-UART para logs/programação. Antes de conectar fisicamente, confirmar no esquema da revisão exata da placa que GPIO48 chega ao DIN do LED integrado e que a saída pode ser compartilhada; a proposta ainda não foi testada.

### Ligação proposta da barra

| Barra WS2812B | Destino | Estado |
|---|---|---|
| VCC | Alimentação regulada de 5 V com capacidade de corrente confirmada | Pendente de verificar a fonte |
| GND | GND comum com a ESP32-S3 | Necessário |
| IN | GPIO48 através de conversor de nível lógico compatível com WS2812B | Proposta; validar esquema e sinal |
| OUT | Sem conexão se não houver outro módulo em série | Definido para módulo único |
| GND extra e VCC extra | Não ligar em duplicidade sem necessidade; confirmar continuidade/roteamento no módulo | Pendente de conferir placa |

Não aplicar 5 V a nenhum GPIO da ESP32-S3. O conversor de nível deve ser apropriado para sinal digital rápido e alimentação de 5 V; um divisor resistivo não é a escolha recomendada para esta interface.

## TTP223 — entrada digital

| Terminal TTP223 | Destino previsto | Estado |
|---|---|---|
| VCC | 3V3 da ESP32-S3 | Recomendado conforme a faixa informada para o módulo |
| GND | GND comum | Previsto |
| I/O ou OUT | Entrada via expansor digital MCP23008 no barramento I²C existente | Estratégia proposta, ainda não implementada |

O expansor de entrada evita ocupar um GPIO adicional da placa. A integração precisa compartilhar o barramento I²C existente de forma correta e respeitar seu ciclo de vida.

## VL53L0X — distância

Proposta de conexão no mesmo barramento I²C usado pelo PCA9685:
- SDA: GPIO19
- SCL: GPIO20
- Endereço padrão esperado do VL53L0X: 0x29
- PCA9685 existente: endereço 0x40, 100 kHz

Os endereços são distintos, mas o código atual cria o barramento dentro de `DocWebServoApiStart()` quando o Wi-Fi conecta e o encerra em `DocWebServoApiStop()`. Antes de adicionar dispositivos, será necessário separar o ciclo de vida do barramento ou garantir que os sensores sejam inicializados/encerrados com ele. Não criar um segundo driver mestre no mesmo par de pinos.

## Verificações de hardware e conflitos

- GPIO48 é identificado como sinal do LED RGB WS2812 integrado; não será atribuído ao TTP223.
- GPIO43/44 são normalmente TX/RX da UART0 e podem ser necessários para console serial/programação; não serão usados para a barra nesta proposta.
- GPIO19/20 são SDA/SCL do PCA9685 e também USB D-/D+ nativos. Preservar a fiação funcional existente e não usar USB nativo simultaneamente com I²C nesses pinos, conforme alerta do `config.h`.
- Os pinos da câmera, display, áudio e BOOT registrados em `config.h` permanecem reservados.
- Alimentação da barra: verificar a corrente disponível de 5 V. Não alimentar a barra a partir de um GPIO.
- Confirmar o esquema/revisão exata da placa antes de conectar GPIO48 em paralelo com o LED integrado.

## Ordem de implementação

1. Confirmar o esquema elétrico da revisão da ESP32-S3-CAM e a capacidade da fonte de 5 V.
2. Implementar driver da WS2812B em GPIO48 numa branch isolada, com comentários `DOC INÍCIO` / `DOC FIM`; o primeiro pixel do LED integrado pode espelhar o primeiro pixel da barra.
3. Integrar o VL53L0X ao barramento I²C já existente, corrigindo o ciclo de vida do barramento sem alterar a configuração de GPIO19/20 nem os canais do PCA9685.
4. Integrar o TTP223 via MCP23008, depois de confirmar o endereço do expansor, os pull-ups e a disponibilidade elétrica do barramento.
5. Compilar e revisar as diferenças; só então preparar um teste físico com motores desativados ou rodas suspensas.

## Preservação do firmware aprovado

- Não modificar a PR #13 nem gravar nela alterações experimentais.
- A PR #14 continua sendo uma proposta separada; este documento não afirma que código de sensor foi implementado ou testado.
- Qualquer mudança funcional deve vir com comentários claros e ser compilada antes de teste na placa.
