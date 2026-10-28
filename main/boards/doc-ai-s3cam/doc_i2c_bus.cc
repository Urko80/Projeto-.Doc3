#include "doc_i2c_bus.h"

#include "config.h"

#include <esp_log.h>
#include <mutex>

namespace {
static const char* TAG = "doc_i2c_bus";
static i2c_master_bus_handle_t s_doc_i2c_bus = nullptr;

// DOC INÍCIO — sincronização do ciclo de vida do barramento
// Protege criação e leitura do handle caso a placa e consumidores solicitem
// acesso em tarefas diferentes. O mutex não é mantido fora deste módulo.
static std::mutex s_doc_i2c_bus_mutex;

// A função interna pressupõe que s_doc_i2c_bus_mutex já está bloqueado.
static esp_err_t EnsureDocI2cBusInitialized() {
    if (s_doc_i2c_bus != nullptr) {
        return ESP_OK;
    }

    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = I2C_NUM_0;
    bus_cfg.sda_io_num = PCA9685_SDA_GPIO;
    bus_cfg.scl_io_num = PCA9685_SCL_GPIO;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.glitch_ignore_cnt = 7;
    bus_cfg.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t new_bus = nullptr;
    const esp_err_t err = i2c_new_master_bus(&bus_cfg, &new_bus);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao criar barramento I2C: %s", esp_err_to_name(err));
        return err;
    }

    // Publica o handle apenas após a criação ter sido concluída com sucesso.
    s_doc_i2c_bus = new_bus;
    ESP_LOGI(TAG, "Barramento compartilhado ativo: SDA=%d, SCL=%d",
             PCA9685_SDA_GPIO, PCA9685_SCL_GPIO);
    return ESP_OK;
}
// DOC FIM — sincronização do ciclo de vida do barramento
}

// DOC INÍCIO — inicialização centralizada do barramento I²C
// A instância fica ativa durante a vida da placa para permitir compartilhamento
// entre PCA9685, VL53L0X e expansor MCP23008. Consumidores não devem apagar
// o barramento global.
esp_err_t DocI2cBusInitialize() {
    const std::lock_guard<std::mutex> lock(s_doc_i2c_bus_mutex);
    return EnsureDocI2cBusInitialized();
}
// DOC FIM — inicialização centralizada

// DOC INÍCIO — acesso sincronizado ao handle do barramento
// A inicialização pode ser repetida; chamadas concorrentes não criam barramentos
// duplicados nem leem o handle durante sua publicação.
esp_err_t DocI2cBusGet(i2c_master_bus_handle_t* bus) {
    if (bus == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    const std::lock_guard<std::mutex> lock(s_doc_i2c_bus_mutex);
    const esp_err_t err = EnsureDocI2cBusInitialized();
    if (err != ESP_OK) {
        *bus = nullptr;
        return err;
    }

    *bus = s_doc_i2c_bus;
    return ESP_OK;
}
// DOC FIM — acesso sincronizado ao barramento
