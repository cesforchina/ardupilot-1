#pragma once

#include "AP_BattMonitor.h"
#include "AP_BattMonitor_Backend.h"

#include <AP_UAVCAN/AP_UAVCAN.h>

#define AP_BATTMONITOR_UAVCAN_TIMEOUT_MICROS         5000000 // sensor becomes unhealthy if no successful readings for 5 seconds

class BattInfoCb;
class BattInfoAuxCb;

class AP_BattMonitor_UAVCAN : public AP_BattMonitor_Backend
{
public:
    enum BattMonitor_UAVCAN_Type {
        UAVCAN_BATTERY_INFO = 0
    };

    /// Constructor
    AP_BattMonitor_UAVCAN(AP_BattMonitor &mon, AP_BattMonitor::BattMonitor_State &mon_state, BattMonitor_UAVCAN_Type type, AP_BattMonitor_Params &params);

    void init() override {}

    /// Read the battery voltage and current.  Should be called at 10hz
    void read() override;

    /// capacity_remaining_pct - returns true if the percentage is valid and writes to percentage argument
    bool capacity_remaining_pct(uint8_t &percentage) const override;

    bool has_temperature() const override { return _has_temperature; }

    bool has_current() const override { return true; }

    bool has_consumed_energy() const override { return _has_consumed_energy; }

    bool has_time_remaining() const override { return _has_time_remaining; }

    bool has_cell_voltages() const override { return _has_cell_voltages; }

    bool get_cycle_count(uint16_t &cycles) const override;

    static void subscribe_msgs(AP_UAVCAN* ap_uavcan);
    static AP_BattMonitor_UAVCAN* get_uavcan_backend(AP_UAVCAN* ap_uavcan, uint8_t node_id, uint8_t battery_id);
    static void handle_battery_info_trampoline(AP_UAVCAN* ap_uavcan, uint8_t node_id, const BattInfoCb &cb);
    static void handle_battery_info_aux_trampoline(AP_UAVCAN* ap_uavcan, uint8_t node_id, const BattInfoAuxCb &cb);

private:
    void handle_battery_info(const BattInfoCb &cb);
    void handle_battery_info_aux(const BattInfoAuxCb &cb);

    static bool match_battery_id(uint8_t instance, uint8_t battery_id) {
        // when serial number is negative, all batteries are accepted. Else, it must match
        return (AP::battery().get_serial_number(instance) < 0) || (AP::battery().get_serial_number(instance) == (int32_t)battery_id);
    }

    AP_BattMonitor::BattMonitor_State _interim_state;
    BattMonitor_UAVCAN_Type _type;

    HAL_Semaphore _sem_battmon;

    AP_UAVCAN* _ap_uavcan;
    uint8_t _soc;
    uint8_t _node_id;
    uint16_t _cycle_count;
    float _remaining_capacity_wh;
    float _full_charge_capacity_wh;
    bool _has_temperature;
    bool _has_cell_voltages;
    bool _has_time_remaining;
    bool _has_consumed_energy;
    bool _has_battery_info_aux;
};
