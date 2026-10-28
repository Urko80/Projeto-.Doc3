# .DOC Control Center — painel web experimental

## Estado atual

A interface web chama a API HTTP do firmware para consultar status, controlar os SG90 das pernas (CH0/CH3) e comandar os MG90S das rodas. Nos testes físicos relatados para esta montagem, frente, ré, giros e parada das rodas funcionaram; os dois servos das pernas também se moveram suavemente. Esses resultados são registros de teste, não certificação de segurança nem garantia para outras montagens.

Confirme a compilação mais recente no GitHub Actions antes de integrar o PR.

## Como usar após compilar e gravar a branch de teste

1. Conecte o ESP32-S3-CAM à mesma rede Wi-Fi do computador/celular.
2. Consulte o IP atribuído à placa no roteador ou no log serial.
3. Abra `index.html` localmente no navegador.
4. No campo “Endereço da API”, informe `http://IP-DA-PLACA` e pressione **Testar conexão**.
5. Mantenha as rodas suspensas, teste um comando por vez e confirme a parada entre movimentos.
6. Teste os servos individualmente, começando perto de 1500 µs; interrompa se houver esforço mecânico, travamento ou ruído anormal.

O navegador pode bloquear a conexão por política de rede local/CORS ou firewall. A API define cabeçalhos CORS para permitir uma página local; isso não substitui autenticação.

## Endpoints do firmware

- `GET /api/status`: consulta status e calibração corrente.
- `POST /api/servo`: `{"channel":0,"pulse_us":1500}` ou canal 3.
- `POST /api/legs`: `{"left_us":1500,"right_us":1500}`.
- `POST /api/wheels`: `{"left":"cw","right":"ccw"}`; cada lado aceita `cw`, `ccw` ou `stop`.
- `POST /api/stop`: tenta parar os dois motores contínuos.

## Mapa de canais e valores registrados

| Canal PCA9685 | Servo | Valores registrados |
|---:|---|---|
| 0 | SG90 — perna 1 | 1500 µs centro de teste; faixa inicial da API 1250–1750 µs |
| 1 | MG90S — roda esquerda | horário 1665 µs; neutro 1705 µs; anti-horário 1800 µs |
| 2 | MG90S — roda direita | horário 1665 µs; neutro 1705 µs; anti-horário 1800 µs |
| 3 | SG90 — perna 2 | 1500 µs centro; faixa inicial da API 1250–1750 µs |

Os valores dos motores contínuos são calibrações medidas para esta montagem. Não os aplique a outros servos sem calibrar.

## Limitações e segurança

- A API não tem autenticação: use apenas em uma rede local confiável; não encaminhe a porta 80 para a internet.
- Os comandos das rodas estão habilitados. CCW=1800 µs foi confirmado nesta montagem; mantenha as rodas suspensas ao validar mudanças.
- O watchdog tenta parar as rodas após 1,5 s sem novo comando; não é uma parada de emergência certificada.
- O slider de velocidade está desativado porque o firmware aceita apenas CW, CCW e STOP com pulsos fixos.
- A calibração não é gravada em NVS.
- Na inicialização, os canais das pernas ficam sem PWM para evitar movimento inesperado; o status diferencia valor de referência e saída ativa.
- GPIO19/GPIO20 também são USB D-/D+ do ESP32-S3. Use conversor USB-UART externo enquanto o PCA9685 estiver conectado.
- A API não tem autenticação e permite CORS amplo. Use apenas em rede confiável; não encaminhe a porta 80 para a internet.
