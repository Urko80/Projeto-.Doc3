#pragma once

#include <stdbool.h>

/**
 * DOC INÍCIO — API web para controle seguro do PCA9685
 *
 * Este módulo expõe endpoints HTTP para a interface de controle do Projeto .DOC.
 * A inicialização deve ocorrer somente após a conexão Wi-Fi.
 *
 * ATENÇÃO:
 * - GPIO19/GPIO20 são usados aqui como SDA/SCL conforme a fiação testada.
 * - Esses pinos também são USB D-/D+ no ESP32-S3; não usar USB nativo enquanto
 *   o barramento I²C estiver conectado.
 * - O código ainda precisa ser compilado e testado na placa antes de uso físico.
 * DOC FIM
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DOC INÍCIO — retorno explícito da inicialização
 * Retorna true somente se I²C/PCA9685, watchdog, servidor HTTP e rotas iniciarem.
 * Retorna false se houver falha; nesse caso os recursos parciais são liberados.
 * DOC FIM
 */
bool DocWebServoApiStart(void);

/** Para motores contínuos e encerra o servidor HTTP. */
void DocWebServoApiStop(void);

#ifdef __cplusplus
}
#endif
