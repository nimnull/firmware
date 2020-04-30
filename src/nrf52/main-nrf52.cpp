#include "NRF52Bluetooth.h"
#include "configuration.h"
#include <assert.h>
#include <ble_gap.h>
#include <memory.h>
#include <nrf52840.h>

// #define USE_SOFTDEVICE

static inline void debugger_break(void)
{
    __asm volatile("bkpt #0x01\n\t"
                   "mov pc, lr\n\t");
}

// handle standard gcc assert failures
void __attribute__((noreturn)) __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    DEBUG_MSG("assert failed %s: %d, %s, test=%s\n", file, line, func, failedexpr);
    // debugger_break(); FIXME doesn't work, possibly not for segger
    while (1)
        ; // FIXME, reboot!
}

void getMacAddr(uint8_t *dmac)
{
    ble_gap_addr_t addr;

#ifdef USE_SOFTDEVICE
    uint32_t res = sd_ble_gap_addr_get(&addr);
    assert(res == NRF_SUCCESS);
    memcpy(dmac, addr.addr, 6);
#else
    const uint8_t *src = (const uint8_t *)NRF_FICR->DEVICEADDR;
    dmac[5] = src[0];
    dmac[4] = src[1];
    dmac[3] = src[2];
    dmac[2] = src[3];
    dmac[1] = src[4];
    dmac[0] = src[5] | 0xc0; // MSB high two bits get set elsewhere in the bluetooth stack
#endif
}

NRF52Bluetooth *nrf52Bluetooth;

static bool bleOn = false;
void setBluetoothEnable(bool on)
{
    if (on != bleOn) {
        if (on) {
            if (!nrf52Bluetooth) {
                nrf52Bluetooth = new NRF52Bluetooth();
                nrf52Bluetooth->setup();
            }
        } else {
            DEBUG_MSG("FIXME: implement BLE disable\n");
        }
        bleOn = on;
    }
}

#include "PmuBQ25703A.h"

PmuBQ25703A pmu;

void nrf52Setup()
{
    // Not yet on board
    // pmu.init();
}