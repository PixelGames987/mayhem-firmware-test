# Rolling Code App - User Interface Guide

## Main Screen Layout

```
┌─────────────────────────────────────┐
│ Rolling Code v1.0                   │
│ Frequency: [ 433.920 MHz ]          │
│ Status: Ready                       │ 
│ Codes: 3                            │
│ Current: HCS301_1234                │
│                                     │
│ [433.92MHz] [Auto]                  │
│                                     │
│ [Capture] [Save] [Load] [Replay]    │
│                                     │
│ [Prev] [Next] [Clear] [Exit]        │ 
└─────────────────────────────────────┘
```

## Button Functions

### Frequency Controls
- **Frequency Field**: Direct frequency entry (tap to edit)
- **433.92MHz Button**: Cycle through preset frequencies (315MHz → 390MHz → 433.92MHz → 868MHz)
- **Auto Button**: Auto-scan across all preset frequencies

### Capture Operations  
- **Capture**: Start/stop signal capture (press while activating remote)
- **Save**: Save all captured codes to file
- **Load**: Load previously saved rolling codes

### Playback Controls
- **Replay**: Transmit current selected rolling code
- **Prev/Next**: Navigate through captured rolling codes  
- **Clear**: Delete all captured codes from memory

## Operation Workflow

### 1. Initial Setup
1. Power on PortaPack with Mayhem firmware
2. Navigate to RX menu → Rolling Code app
3. Select appropriate frequency using preset button

### 2. Capturing Rolling Codes
1. Hold gate remote near PortaPack antenna  
2. Press "Capture" button on PortaPack
3. Press gate remote button multiple times (5-10 presses)
4. Different rolling codes will be captured and displayed
5. Status will show "Captured rolling code!" 

### 3. Managing Codes
- **View Codes**: Use Prev/Next to browse captured codes
- **Save Session**: Press Save to store codes to file
- **Load Session**: Press Load to restore previous capture session

### 4. Replaying Codes
1. Use Prev/Next to select desired rolling code
2. Point PortaPack antenna toward gate receiver
3. Press "Replay" to transmit the rolling code
4. Gate should respond to transmitted code

## File Management

### Save Location
- Files saved to: `/PORTAPACK/ROLLS/rolling_codes.TXT`
- Format: Plain text, human readable
- Contains: Frequency, sample rate, binary payload, code name

### File Format Example
```
# Rolling Code File v1.0
# Format: Frequency SampleRate Payload Name  
433920000 2000000 1010101011001100110010101010110010101010110011001100101010101100 HCS301_1234
433920000 2000000 1110001000111010001110001000111011100010001110100011100010001110 KeeLoq_ABCD
315000000 2000000 110100110100110100110100110100110110100110100110100110100110100 Linear_56789
```

## Supported Remote Types

### Gate/Garage Remotes
- **Frequency**: 315MHz, 433.92MHz, 868MHz
- **Protocols**: HCS301, KeeLoq, Linear, Custom
- **Encoding**: Manchester, PWM, OOK

### Security Considerations
- Codes change each transmission (rolling/hopping)
- Some systems allow resync with multiple button presses
- Modern encrypted systems may not be compatible
- Always ensure legal authorization before testing

## Troubleshooting

### No Codes Captured
- Check frequency selection matches remote
- Ensure remote is within range (1-3 feet)
- Try different orientations/positions
- Verify remote battery level

### Replay Not Working  
- Confirm correct frequency setting
- Check distance to gate receiver
- Verify antenna orientation
- Some systems require multiple replay attempts

### File Operations Failing
- Ensure SD card is inserted and working
- Check available storage space
- Verify ROLLS directory permissions

## Safety and Legal Notes

⚠️ **Only use on systems you own or have authorization to test**  
⚠️ **Follow local radio frequency regulations**  
⚠️ **This tool is for security research and authorized testing only**  

---
*For technical details see ROLLING_CODE_IMPLEMENTATION.md*