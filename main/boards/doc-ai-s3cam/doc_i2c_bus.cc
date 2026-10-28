#include "doc_i2c_bus.h"

#include "config.h"

#include <esp_log.h>

namespace {
static const char* TAG = "doc_i2c_bus";
static i2c_master_bus_handle_t s_doc_i2c_bus = nullptr;
}

// DOC INÍCIO — inicialização centralizada do barramento I²C
// A instância fica ativa durante a vida da placa para permitir compartilhamento
// entre PCA9685, VL53L0X e expansor MCP23008. Não chamar i2c_del_master_bus()
// nos módulos consumidores.
esp_err_t DocI2cBusInitialize() {
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

    const esp_err_t err = i2c_new_master_bus(&bus_cfg, &s_doc_i2c_bus);
    if (err != ESP_OK) {
        s_doc_i2c_bus = nullptr;
        ESP_LOGE(TAG, "Falha ao criar barramento I2C: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Barramento compartilhado ativo: SDA=%d, SCL=%d",
             PCA9685_SDA_GPIO, PCA9685_SCL_GPIO);
    return ESP_OK;
}
// DOC FIM — inicialização centralizada

// DOC INÍCIO — acesso seguro ao handle do barramento
// A inicialização é repetível e não recria um barramento já ativo.
esp_err_t DocI2cBusGet(i2c_master_bus_handle_t* bus) {
    if (bus == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    const esp_err_t err = DocI2cBusInitialize();
    if (err != ESP_OK) {
        *bus = nullptr;
        return err;
    }
    *bus = s_doc_i2c_bus;
    return ESP_OK;
}
// DOC FIM — acesso ao barramento
