#pragma once

#include <driver/i2c_master.h>
#include <esp_err.h>
#include <stdint.h>

// DOC INÍCIO — driver configurável do expansor MCP23008
// O endereço depende dos pinos A0/A1/A2 da placa (0x20 a 0x27).
// Este driver não inicializa nem remove o barramento global compartilhado.
// O chamador fornece o handle obtido por DocI2cBusGet().
class DocMcp23008 {
public:
    DocMcp23008() = default;
    ~DocMcp23008();

    // Não copia handles I²C; evita remoção dupla do dispositivo.
    DocMcp23008(const DocMcp23008&) = delete;
    DocMcp23008& operator=(const DocMcp23008&) = delete;

    // Registra o MCP23008 no barramento compartilhado.
    // address deve estar entre 0x20 e 0x27.
    esp_err_t Initialize(i2c_master_bus_handle_t bus, uint8_t address);

    // Configura GPIOx como entrada e habilita/desabilita seu pull-up interno.
    esp_err_t ConfigureInput(uint8_t pin, bool pullup_enabled);

    // Lê o estado lógico do pino configurado.
    esp_err_t ReadPin(uint8_t pin, bool* level);

    // Remove somente o dispositivo MCP23008, nunca o barramento global.
    void Deinitialize();

private:
    esp_err_t ReadRegister(uint8_t reg, uint8_t* value);
    esp_err_t WriteRegister(uint8_t reg, uint8_t value);

    i2c_master_bus_handle_t bus_ = nullptr;
    i2c_master_dev_handle_t device_ = nullptr;
    uint8_t address_ = 0;
};
// DOC FIM — driver configurável do expansor MCP23008
