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

#include "ui_rolling_code.hpp"
#include "baseband_api.hpp"
#include "string_format.hpp"
#include "file_path.hpp"
#include "ch.h"

using namespace portapack;

namespace ui::external_app::rolling_code {

RollingCodeView::RollingCodeView(NavigationView& nav)
    : nav_{nav} {
    
    baseband::run_image(portapack::spi_flash::image_tag_capture);

    add_children({
        &labels,
        &field_frequency,
        &text_status,
        &text_code_count,
        &text_current_name,
        &button_freq_preset,
        &button_auto,
        &button_capture,
        &button_save,
        &button_load,
        &button_replay,
        &button_prev,
        &button_next,
        &button_clear,
        &button_exit
    });

    // Set up frequency field
    field_frequency.set_value(radio_state_.frequency);
    field_frequency.on_change = [this](rf::Frequency f) {
        radio_state_.frequency = f;
        settings_.save("frequency", &f);
    };
    field_frequency.on_edit = [this, &nav]() {
        // TODO: Implement frequency edit dialog if needed
    };

    // Button event handlers
    button_freq_preset.on_select = [this](Button&) {
        // Cycle through common gate remote frequencies
        current_freq_index = (current_freq_index + 1) % common_gate_frequencies.size();
        radio_state_.frequency = common_gate_frequencies[current_freq_index];
        field_frequency.set_value(radio_state_.frequency);
        update_freq_preset_text();
    };

    button_auto.on_select = [this](Button&) {
        this->on_auto_scan();
    };
    button_capture.on_select = [this](Button&) {
        this->on_capture();
        // Reset button text after a delay
        chThdSleepMilliseconds(1000);
        button_capture.set_text("Capture");
    };

    button_save.on_select = [this](Button&) {
        this->on_save();
    };

    button_load.on_select = [this](Button&) {
        this->on_load();
    };

    button_replay.on_select = [this](Button&) {
        this->on_replay();
    };

    button_prev.on_select = [this](Button&) {
        this->on_prev();
    };

    button_next.on_select = [this](Button&) {
        this->on_next();
    };

    button_clear.on_select = [this](Button&) {
        this->on_clear();
    };

    button_exit.on_select = [this](Button&) {
        nav_.pop();
    };

    // Load settings
    settings_.load("frequency", &radio_state_.frequency);
    field_frequency.set_value(radio_state_.frequency);
    
    // Update frequency preset button text
    update_freq_preset_text();
    
    update_display();
}

RollingCodeView::~RollingCodeView() {
    radio_state_.save();
    baseband::shutdown();
}

void RollingCodeView::focus() {
    button_capture.focus();
}

void RollingCodeView::on_capture() {
    if (is_capturing_) {
        // Stop capture
        is_capturing_ = false;
        button_capture.set_text("Capture");
        text_status.set("Ready");
        return;
    }

    // For now, simulate capture with dummy data
    simulate_capture();
}

void RollingCodeView::simulate_capture() {
    // Generate a more realistic rolling code simulation
    // Different gate remote protocols typically use:
    // - 12, 16, 20, or 24-bit rolling codes  
    // - Common frequencies: 315MHz, 433.92MHz, 868MHz
    // - Manchester, PWM or ASK/OOK modulation
    
    static uint32_t rolling_counter = 0x1234; // Simulate rolling counter
    
    // Generate different types of rolling codes to simulate real remotes
    std::string payload;
    std::string name;
    uint32_t frequency = radio_state_.frequency;
    
    switch (captured_codes_.size() % 4) {
        case 0: {
            // HCS301 style - 16 bit rolling code + 16 bit fixed
            uint16_t fixed_code = 0x5A3C;
            uint16_t rolling_code = rolling_counter & 0xFFFF;
            
            // Convert to Manchester encoded payload (simplified)
            payload = "";
            for (int i = 15; i >= 0; i--) {
                payload += ((fixed_code >> i) & 1) ? "10" : "01"; // Manchester encoding
            }
            for (int i = 15; i >= 0; i--) {
                payload += ((rolling_code >> i) & 1) ? "10" : "01";
            }
            name = "HCS301_" + to_string_hex(rolling_code, 4);
            break;
        }
        case 1: {
            // KeeLoq style - 28 bit rolling
            uint32_t keeloq_code = rolling_counter;
            payload = "";
            for (int i = 27; i >= 0; i--) {
                payload += ((keeloq_code >> i) & 1) ? "1110" : "1000"; // PWM encoding
            }
            name = "KeeLoq_" + to_string_hex(keeloq_code & 0xFFFFFFF, 7);
            break;
        }
        case 2: {
            // Linear style - 20 bit code
            uint32_t linear_code = 0x50000 | (rolling_counter & 0x0FFFF);
            payload = "";
            for (int i = 19; i >= 0; i--) {
                payload += ((linear_code >> i) & 1) ? "110" : "100"; // Simple OOK
            }
            name = "Linear_" + to_string_hex(linear_code, 5);
            break;
        }
        case 3: {
            // Custom style - 24 bit with preamble
            uint32_t custom_code = rolling_counter & 0xFFFFFF;
            payload = "1010101010101010"; // Preamble
            for (int i = 23; i >= 0; i--) {
                payload += ((custom_code >> i) & 1) ? "11" : "10";
            }
            name = "Custom_" + to_string_hex(custom_code, 6);
            break;
        }
    }
    
    rolling_counter += 0x123; // Increment counter to simulate rolling
    
    RollingCodeEntry entry(frequency, 2000000, payload, name);
    captured_codes_.push_back(entry);
    
    current_code_index_ = captured_codes_.size() - 1;
    text_status.set("Captured rolling code!");
    update_display();
    
    // Simulate a brief capture indication
    button_capture.set_text("Got it!");
}

void RollingCodeView::on_save() {
    if (captured_codes_.empty()) {
        text_status.set("No codes to save");
        return;
    }

    // Simple filename for now
    auto path = fs::path("ROLLS/rolling_codes.TXT");
    
    // Create directory if needed
    auto dir_result = make_new_directory("ROLLS");
    
    // Save rolling codes to file
    File file;
    auto result = file.create(path);
    if (result.code() != 0) {
        text_status.set("Save failed!");
        return;
    }

    // Write header
    file.write_line("# Rolling Code File v1.0");
    file.write_line("# Format: Frequency SampleRate Payload Name");
    
    // Write each code
    for (const auto& code : captured_codes_) {
        std::string line = to_string_dec_uint(code.frequency) + " " +
                          to_string_dec_uint(code.sample_rate) + " " +
                          code.payload + " " + 
                          code.name;
        file.write_line(line);
    }

    text_status.set("Saved " + to_string_dec_uint(captured_codes_.size()) + " codes");
}

void RollingCodeView::on_load() {
    // For now, use a simple fixed filename
    auto path = fs::path("ROLLS/rolling_codes.TXT");
    
    File file;
    auto result = file.open(path);
    if (result.code() != 0) {
        text_status.set("Load failed! No file");
        return;
    }

    captured_codes_.clear();
    std::string line;
    char line_buffer[256];
    
    while (file.read_line(line_buffer, sizeof(line_buffer))) {
        line = std::string(line_buffer);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        // Parse line: Frequency SampleRate Payload Name
        size_t pos1 = line.find(' ');
        size_t pos2 = line.find(' ', pos1 + 1);
        size_t pos3 = line.find(' ', pos2 + 1);
        
        if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
            uint32_t frequency = std::stoul(line.substr(0, pos1));
            uint32_t sample_rate = std::stoul(line.substr(pos1 + 1, pos2 - pos1 - 1));
            std::string payload = line.substr(pos2 + 1, pos3 - pos2 - 1);
            std::string name = line.substr(pos3 + 1);
            
            captured_codes_.emplace_back(frequency, sample_rate, payload, name);
        }
    }
    
    if (!captured_codes_.empty()) {
        current_code_index_ = 0;
        text_status.set("Loaded " + to_string_dec_uint(captured_codes_.size()) + " codes");
    } else {
        text_status.set("No codes loaded");
    }
    update_display();
}

void RollingCodeView::on_replay() {
    if (captured_codes_.empty()) {
        text_status.set("No codes to replay");
        return;
    }

    // Get current code
    const auto& code = captured_codes_[current_code_index_];
    
    // Prepare for OOK transmission
    baseband::run_image(portapack::spi_flash::image_tag_ook);
    
    // Set frequency
    radio_state_.frequency = code.frequency;
    field_frequency.set_value(code.frequency);
    radio_state_.update_tuning_frequency();

    // Configure OOK transmission parameters
    uint32_t stream_length = code.payload.length();
    uint32_t samples_per_bit = code.sample_rate / 1000; // Default 1kHz symbol rate
    uint8_t repeat = 5; // Repeat 5 times  
    uint32_t pause_symbols = 10000; // 10ms pause between repeats
    
    // Set OOK data for baseband
    baseband::set_ook_data(stream_length, samples_per_bit, repeat, pause_symbols);
    
    text_status.set("Replaying: " + code.name);
    
    // The actual payload transmission would be handled by the baseband processor
    // For now this sets up the parameters for transmission
}

void RollingCodeView::on_prev() {
    if (captured_codes_.empty()) return;
    
    if (current_code_index_ > 0) {
        current_code_index_--;
    } else {
        current_code_index_ = captured_codes_.size() - 1;
    }
    update_display();
}

void RollingCodeView::on_next() {
    if (captured_codes_.empty()) return;
    
    current_code_index_ = (current_code_index_ + 1) % captured_codes_.size();
    update_display();
}

void RollingCodeView::on_clear() {
    captured_codes_.clear();
    current_code_index_ = 0;
    text_status.set("Cleared all codes");
    update_display();
}

void RollingCodeView::update_freq_preset_text() {
    // Update button text to show current frequency  
    if (radio_state_.frequency == 315000000) {
        button_freq_preset.set_text("315MHz");
    } else if (radio_state_.frequency == 390000000) {
        button_freq_preset.set_text("390MHz");
    } else if (radio_state_.frequency == 433920000) {
        button_freq_preset.set_text("433.92MHz");
    } else if (radio_state_.frequency == 868000000) {
        button_freq_preset.set_text("868MHz");
    } else {
        button_freq_preset.set_text(to_string_short_freq(radio_state_.frequency));
    }
}

void RollingCodeView::on_auto_scan() {
    if (is_auto_scanning_) {
        is_auto_scanning_ = false;
        button_auto.set_text("Auto");
        text_status.set("Auto scan stopped");
        return;
    }
    
    is_auto_scanning_ = true;
    button_auto.set_text("Stop");
    text_status.set("Auto scanning...");
    
    // In a real implementation, this would start scanning across
    // the common frequencies looking for signals
    // For now, simulate finding codes on different frequencies
    for (size_t i = 0; i < common_gate_frequencies.size() && is_auto_scanning_; i++) {
        radio_state_.frequency = common_gate_frequencies[i];
        field_frequency.set_value(radio_state_.frequency);
        update_freq_preset_text();
        
        // Simulate a brief pause on each frequency
        chThdSleepMilliseconds(500);
        
        // Simulate occasionally finding a signal (25% chance per frequency)
        if ((i + captured_codes_.size()) % 4 == 0) {
            simulate_capture();
            text_status.set("Found signal!");
            chThdSleepMilliseconds(1000);
        }
    }
    
    is_auto_scanning_ = false;
    button_auto.set_text("Auto");
    text_status.set("Auto scan complete");
}

void RollingCodeView::update_display() {
    text_code_count.set(to_string_dec_uint(captured_codes_.size()));
    
    if (captured_codes_.empty()) {
        text_current_name.set("-");
    } else {
        const auto& current = captured_codes_[current_code_index_];
        text_current_name.set(current.name);
    }
}

} // namespace ui::external_app::rolling_code