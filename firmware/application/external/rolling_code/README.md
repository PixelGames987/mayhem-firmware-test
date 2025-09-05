# Rolling Code App for PortaPack

This app allows you to capture and replay rolling code signals from gate remotes, car key fobs, and similar devices.

## Features

- **Multi-frequency support**: Supports common gate remote frequencies (315MHz, 390MHz, 433.92MHz, 868MHz, etc.)
- **Rolling code capture**: Simulates capture of different rolling code formats (HCS301, KeeLoq, Linear, etc.)
- **Signal replay**: Transmits captured codes using OOK modulation
- **File save/load**: Save captured codes to files for later use
- **Multi-code storage**: Store multiple rolling codes and cycle through them

## How Rolling Codes Work

Rolling codes (also called hopping codes) are security mechanisms used in modern remotes:
- Each time you press the remote, a different code is transmitted
- The receiver expects the next code in the sequence
- This prevents replay attacks from simple code grabbers
- However, some systems allow "resync" by pressing the button multiple times

## Usage Instructions

1. **Set Frequency**: Use the frequency preset button to cycle through common gate remote frequencies, or manually set with the frequency field

2. **Capture Codes**: 
   - Point the gate remote at your PortaPack
   - Press "Capture" on the PortaPack  
   - Press the remote button (gate up/unlock) 
   - Repeat 3-5 times to capture multiple rolling codes
   - The app will show different codes being captured

3. **Save Codes**: Press "Save" to store all captured codes to a file in the ROLLS directory

4. **Load Codes**: Press "Load" to restore previously saved rolling codes

5. **Replay Codes**: 
   - Use "Prev"/"Next" to select which code to transmit
   - Press "Replay" to transmit the selected rolling code
   - The system will cycle through available codes for multiple attempts

## File Format

Rolling codes are saved in text files with this format:
```
# Rolling Code File v1.0
# Format: Frequency SampleRate Payload Name
433920000 2000000 10101010110011001100101010101100 HCS301_1234
```

## Common Gate Remote Frequencies

- **315 MHz**: Common in North America
- **390 MHz**: Some older systems  
- **433.92 MHz**: Very common in Europe and Asia
- **868 MHz**: European frequency for newer systems

## Legal Notice

**This tool is for educational and legitimate use only**. Only use this app on:
- Your own gate remotes and systems
- Systems you have explicit permission to test
- Educational purposes with proper authorization

Do not use this app to gain unauthorized access to any systems. The user is responsible for compliance with local laws and regulations.

## Technical Details

- Uses OOK (On-Off Keying) modulation for transmission
- Supports Manchester and PWM encoding schemes
- Sample rates up to 20 MSPS for wideband capture
- Compatible with HackRF One + PortaPack H2

## Building

This app is built as part of the PortaPack Mayhem firmware. It's automatically included when building the firmware.