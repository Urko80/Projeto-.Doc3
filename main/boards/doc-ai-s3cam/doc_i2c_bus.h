#pragma once

#include <driver/i2c_master.h>
#include <esp_err.h>

// DOC INÍCIO — API do barramento I²C compartilhado do Projeto .DOC
// O módulo da placa é o proprietário do barramento; consumidores registram e
// removem apenas seus próprios dispositivos, sem apagar o barramento global.
esp_err_t DocI2cBusInitialize();
esp_err_t DocI2cBusGet(i2c_master_bus_handle_t* bus);
// DOC FIM — API do barramento compartilhado
