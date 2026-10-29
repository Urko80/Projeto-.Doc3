#include "doc_mcp23008.h"

#include <esp_log.h>

namespace {
// DOC INÍCIO — registradores MCP23008
// Endereços definidos na folha de dados: IODIR=0x00, GPPU=0x06, GPIO=0x09.
constexpr uint8_t MCP23008_ADDRESS_MIN = 0x20;
constexpr uint8_t MCP23008_ADDRESS_MAX = 0x27;
constexpr uint8_t MCP23008_REG_IODIR = 0x00;
constexpr uint8_t MCP23008_REG_GPPU = 0x06;
constexpr uint8_t MCP23008_REG_GPIO = 0x09;
constexpr uint32_t MCP23008_I2C_SPEED_HZ = 100000;
constexpr int MCP23008_TIMEOUT_MS = 100;
static const char* TAG = "doc_mcp23008";
// DOC FIM — registradores MCP23008
}

// DOC INÍCIO — ciclo de vida do dispositivo MCP23008
DocMcp23008::~DocMcp23008() {
    Deinitialize();
}

esp_err_t DocMcp23008::Initialize(i2c_master_bus_handle_t bus, uint8_t address) {
    if (bus == nullptr || address < MCP23008_ADDRESS_MIN ||
        address > MCP23008_ADDRESS_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    // Permite repetir Initialize com os mesmos parâmetros sem registrar
    // dispositivos duplicados. Para trocar de barramento/endereço, encerra antes.
    if (device_ != nullptr) {
        if (bus_ == bus && address_ == address) {
            return ESP_OK;
        }
        return ESP_ERR_INVALID_STATE;
    }

    i2c_device_config_t config = {};
    config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    config.device_address = address;
    config.scl_speed_hz = MCP23008_I2C_SPEED_HZ;
    config.scl_wait_us = 0;
    config.flags.disable_ack_check = 0;

    i2c_master_dev_handle_t new_device = nullptr;
    const esp_err_t err = i2c_master_bus_add_device(bus, &config, &new_device);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao registrar MCP23008 em 0x%02X: %s",
                 address, esp_err_to_name(err));
        return err;
    }

    bus_ = bus;
    device_ = new_device;
    address_ = address;
    ESP_LOGI(TAG, "Dispositivo MCP23008 registrado em 0x%02X", address_);
    return ESP_OK;
}
// DOC FIM — ciclo de vida do dispositivo MCP23008

// DOC INÍCIO — acesso a registradores com retorno explícito de erro
esp_err_t DocMcp23008::ReadRegister(uint8_t reg, uint8_t* value) {
    // DOC INÍCIO — validação separada de argumentos e estado
    // Um ponteiro de saída nulo é erro de argumento; dispositivo ausente
    // representa estado inválido do driver.
    if (value == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    // DOC FIM — validação separada de argumentos e estado

    return i2c_master_transmit_receive(
        device_, &reg, sizeof(reg), value, sizeof(*value), MCP23008_TIMEOUT_MS);
}

esp_err_t DocMcp23008::WriteRegister(uint8_t reg, uint8_t value) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    const uint8_t data[] = {reg, value};
    return i2c_master_transmit(device_, data, sizeof(data), MCP23008_TIMEOUT_MS);
}
// DOC FIM — acesso a registradores

// DOC INÍCIO — configuração e leitura de entrada
esp_err_t DocMcp23008::ConfigureInput(uint8_t pin, bool pullup_enabled) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (pin > 7) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t direction = 0;
    esp_err_t err = ReadRegister(MCP23008_REG_IODIR, &direction);
    if (err != ESP_OK) {
        return err;
    }

    // Bit 1 no IODIR configura entrada. Preserva a configuração dos outros pinos.
    direction = static_cast<uint8_t>(direction | (1U << pin));
    err = WriteRegister(MCP23008_REG_IODIR, direction);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t pullups = 0;
    err = ReadRegister(MCP23008_REG_GPPU, &pullups);
    if (err != ESP_OK) {
        return err;
    }

    if (pullup_enabled) {
        pullups = static_cast<uint8_t>(pullups | (1U << pin));
    } else {
        pullups = static_cast<uint8_t>(pullups & ~(1U << pin));
    }
    return WriteRegister(MCP23008_REG_GPPU, pullups);
}

esp_err_t DocMcp23008::ReadPin(uint8_t pin, bool* level) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (pin > 7 || level == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t gpio = 0;
    const esp_err_t err = ReadRegister(MCP23008_REG_GPIO, &gpio);
    if (err != ESP_OK) {
        return err;
    }

    *level = (gpio & (1U << pin)) != 0;
    return ESP_OK;
}
// DOC FIM — configuração e leitura de entrada

// DOC INÍCIO — encerramento sem remover o barramento compartilhado
void DocMcp23008::Deinitialize() {
    if (device_ != nullptr) {
        const esp_err_t err = i2c_master_bus_rm_device(device_);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Falha ao remover MCP23008: %s", esp_err_to_name(err));
        }
    }

    device_ = nullptr;
    bus_ = nullptr;
    address_ = 0;
}
// DOC FIM — encerramento do dispositivo
