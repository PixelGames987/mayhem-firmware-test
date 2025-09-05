/*
 * Copyright (C) 2024 
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#pragma once

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_freq_field.hpp"
#include "app_settings.hpp"
#include "radio_state.hpp"
#include "ook_file.hpp"
#include "file.hpp"
#include "baseband_api.hpp"

#include <vector>
#include <string>
#include <memory>

namespace ui::external_app::rolling_code {

struct RollingCodeEntry {
    uint32_t frequency;
    uint32_t sample_rate;
    std::string payload;  
    std::string name;
    
    RollingCodeEntry(uint32_t freq = 433920000, uint32_t sr = 2000000, const std::string& p = "", const std::string& n = "")
        : frequency(freq), sample_rate(sr), payload(p), name(n) {}
};

class RollingCodeView : public View {
   public:
    RollingCodeView(NavigationView& nav);
    ~RollingCodeView();

    void focus() override;
    
    std::string title() const override { return "Rolling Code"; };

   private:
    NavigationView& nav_;
    
    RxRadioState radio_state_{
        433920000 /* frequency */,
        2000000 /* sample_rate */,
        2000000 /* baseband_bandwidth */
    };

    app_settings::SettingsManager settings_{
        "rx_rolling_code", app_settings::Mode::RX
    };

    const std::vector<rf::Frequency> common_gate_frequencies = {
        315000000,   // 315 MHz (US)
        390000000,   // 390 MHz
        433920000,   // 433.92 MHz (EU/Asia) 
        868000000,   // 868 MHz (EU)
        315550000,   // 315.55 MHz  
        433050000,   // 433.05 MHz
        433230000    // 433.23 MHz
    };

    size_t current_freq_index = 2; // Default to 433.92MHz

    void on_capture();
    void on_save();
    void on_load();
    void on_replay();
    void on_prev();
    void on_next(); 
    void on_clear();
    void on_auto_scan();
    void update_display();
    void simulate_capture();  // For testing
    void update_freq_preset_text();
    
    std::vector<RollingCodeEntry> captured_codes_;
    size_t current_code_index_ = 0;
    bool is_capturing_ = false;
    bool is_auto_scanning_ = false;

    Labels labels{
        {{0 * 8, 0 * 16}, "Rolling Code v1.0", Color::light_grey()},
        {{0 * 8, 2 * 16}, "Frequency:", Color::light_grey()},
        {{0 * 8, 4 * 16}, "Status:", Color::light_grey()},
        {{0 * 8, 6 * 16}, "Codes:", Color::light_grey()},
        {{0 * 8, 8 * 16}, "Current:", Color::light_grey()},
    };

    FreqField field_frequency{
        {10 * 8, 2 * 16}
    };

    Text text_status{
        {7 * 8, 4 * 16, 20 * 8, 1 * 16},
        "Ready"
    };

    Text text_code_count{
        {6 * 8, 6 * 16, 10 * 8, 1 * 16},
        "0"
    };

    Text text_current_name{
        {8 * 8, 8 * 16, 20 * 8, 1 * 16},
        "-"
    };

    Button button_capture{
        {0 * 8, 11 * 16, 7 * 8, 2 * 16},
        "Capture"
    };

    Button button_save{
        {8 * 8, 11 * 16, 5 * 8, 2 * 16},
        "Save"
    };

    Button button_load{
        {14 * 8, 11 * 16, 5 * 8, 2 * 16},
        "Load" 
    };

    Button button_replay{
        {20 * 8, 11 * 16, 6 * 8, 2 * 16},
        "Replay"
    };

    Button button_prev{
        {0 * 8, 14 * 16, 5 * 8, 2 * 16},
        "Prev"
    };

    Button button_next{
        {6 * 8, 14 * 16, 5 * 8, 2 * 16},
        "Next"
    };

    Button button_clear{
        {12 * 8, 14 * 16, 5 * 8, 2 * 16},
        "Clear"
    };

    Button button_auto{
        {11 * 8, 10 * 16, 6 * 8, 1 * 16},
        "Auto"
    };

    Button button_freq_preset{
        {0 * 8, 10 * 16, 10 * 8, 1 * 16},
        "433.92MHz"
    };

    Button button_exit{
        {18 * 8, 14 * 16, 4 * 8, 2 * 16},
        "Exit"
    };
};

} // namespace ui::external_app::rolling_code