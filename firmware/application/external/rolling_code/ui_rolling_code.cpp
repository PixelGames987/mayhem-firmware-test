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
    button_capture.on_select = [this](Button&) {
        this->on_capture();
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
    // Generate a simple simulated rolling code for testing
    std::string payload = "10101010110011001100101010101100";
    std::string name = "Code" + to_string_dec_uint(captured_codes_.size() + 1);
    
    RollingCodeEntry entry(radio_state_.frequency, 2000000, payload, name);
    captured_codes_.push_back(entry);
    
    current_code_index_ = captured_codes_.size() - 1;
    text_status.set("Captured!");
    update_display();
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
    
    // Create OOK file data structure for transmission
    ook_file_data ook_data;
    ook_data.frequency = code.frequency;
    ook_data.sample_rate = code.sample_rate;
    ook_data.symbol_rate = 1000;  // Default symbol rate
    ook_data.repeat = 5;          // Repeat 5 times
    ook_data.pause_symbol_duration = 10000;  // 10ms pause
    ook_data.payload = code.payload;

    // TODO: Implement actual OOK transmission using baseband API
    // For now, just show status
    text_status.set("Replaying: " + code.name);
    
    // Simulate replay completion
    // In real implementation, this would be done via message handler
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