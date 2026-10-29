#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include <driver/i2c_master.h>
#include <stdint.h>

class I2cDevice {
public:
    // DOC INÍCIO — velocidade configurável por dispositivo
    // O padrão de 400 kHz preserva os chamadores existentes.
    // Use 100000 para periféricos que precisem acompanhar o barramento do DOC.
    I2cDevice(i2c_master_bus_handle_t i2c_bus, uint8_t addr,
              uint32_t scl_speed_hz = 400000);
    // DOC FIM — velocidade configurável por dispositivo

protected:
    i2c_master_bus_handle_t i2c_bus_;
    i2c_master_dev_handle_t i2c_device_;
    uint8_t device_address_;

    void WriteReg(uint8_t reg, uint8_t value);
    void WriteRegs(uint8_t reg, const uint8_t* buffer, size_t length);
    uint8_t ReadReg(uint8_t reg);
    void ReadRegs(uint8_t reg, uint8_t* buffer, size_t length);
    esp_err_t ResetBus(const char* reason);
};

#endif  // I2C_DEVICE_H
