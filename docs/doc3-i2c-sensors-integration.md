# Plano de integração dos sensores I²C — Projeto .DOC3

<!-- DOC INÍCIO — objetivo e escopo -->
## Objetivo

Preparar a integração do VL53L0X e do MCP23008 sem alterar a pinagem já testada, sem ativar hardware ainda não validado e sem modificar o firmware funcional da PR #13.

Este documento é planejamento técnico; não representa teste de bancada nem confirmação de funcionamento.
<!-- DOC FIM — objetivo e escopo -->

<!-- DOC INÍCIO — estado verificado no repositório -->
## Estado encontrado no Doc3

- O manifesto `main/idf_component.yml` declara ESP-IDF `>=6.0.1`.
- A busca no repositório não encontrou implementações existentes de `VL53L0X` ou `MCP23008`.
- A classe comum `I2cDevice` faz `ESP_ERROR_CHECK` nas operações de leitura/escrita; por isso, não deve ser usada diretamente como API recuperável de sensor sem antes revisar o tratamento de erros.
- A PR #5 propõe um proprietário único para o barramento I²C, com SDA GPIO19, SCL GPIO20 e PCA9685 em `0x40`.
- O código atual do barramento usa `std::mutex`; a compilação do projeto ainda precisa confirmar esse caminho no ambiente real de ESP-IDF.
<!-- DOC FIM — estado verificado no repositório -->

<!-- DOC INÍCIO — arquitetura proposta -->
## Arquitetura proposta

1. Manter um único barramento I²C, gerenciado por `doc_i2c_bus.cc`.
2. Não permitir que drivers de sensores apaguem o barramento compartilhado; cada driver deve remover apenas o handle do próprio dispositivo.
3. VL53L0X: endereço I²C padrão esperado `0x29`; confirmar endereço e compatibilidade do driver com a versão de ESP-IDF antes de adicionar dependência.
4. MCP23008: endereço depende dos pinos A0/A1/A2 (faixa usual `0x20–0x27`); não fixar endereço até confirmar a ligação física.
5. TTP223: avaliar sua saída digital conectada a uma entrada do MCP23008, com debounce e tratamento de falhas. Não atribuir GPIO direto ao TTP223 enquanto não houver pino livre confirmado.
6. Manter operações de I²C e estados dos sensores fora de callbacks de interface; documentar inicialização, leitura, erro e encerramento.
<!-- DOC FIM — arquitetura proposta -->

<!-- DOC INÍCIO — critérios de validação -->
## Critérios antes de ativar hardware

- [ ] Confirmar a compilação do Doc3 com ESP-IDF >= 6.0.1.
- [ ] Confirmar a estratégia de mutex/sincronização no ambiente de build.
- [ ] Definir tratamento de erros que não reinicie o firmware em uma falha transitória de sensor.
- [ ] Confirmar endereço real do MCP23008 e alimentação lógica de cada módulo.
- [ ] Confirmar pinagem elétrica e pull-ups do barramento.
- [ ] Compilar após cada driver e revisar logs de inicialização.
- [ ] Testar em bancada primeiro; não gravar firmware nem mover servos como parte desta etapa documental.
<!-- DOC FIM — critérios de validação -->

## Restrições preservadas

- PR #13 e o firmware funcional não foram alterados.
- SDA GPIO19 e SCL GPIO20 permanecem conforme a montagem já testada.
- Nenhum sensor foi ativado por este documento.
- Todas as mudanças de código futuras devem ser delimitadas por comentários `DOC INÍCIO` / `DOC FIM`.
