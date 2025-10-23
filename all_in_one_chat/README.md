# RYLR998 LoRa Communication Between Two Arduino Unos

This project enables bidirectional message communication between two Arduino Uno boards using RYLR998 LoRa modules.

## Hardware Requirements

- 2x Arduino Uno boards
- 2x RYLR998 LoRa modules (433MHz or 868MHz/915MHz depending on your region)
- Jumper wires
- 2x USB cables for programming and serial monitoring

## Wiring Connections

Connect each RYLR998 module to its Arduino Uno as follows:

| RYLR998 Pin | Arduino Uno Pin |
|-------------|-----------------|
| TX          | Pin 2 (RX)      |
| RX          | Pin 3 (TX)      |
| VCC         | 5V              |
| GND         | GND             |

**Important:** The RYLR998 modules operate at 3.3V logic but can tolerate 5V. If you experience issues, add a voltage divider (2kΩ and 1kΩ resistors) on the Arduino TX (Pin 3) line.

## Installation

1. **Upload the sketches:**
   - Upload `arduino_uno_sender.ino` to the first Arduino Uno (Device 1 - Address 1)
   - Upload `arduino_uno_receiver.ino` to the second Arduino Uno (Device 2 - Address 2)

2. **Open Serial Monitors:**
   - Open the Serial Monitor for each Arduino at 9600 baud
   - You should see initialization messages confirming the module configuration

## Usage

1. After uploading and powering both Arduinos, each will display initialization info
2. Type a message in either Serial Monitor and press Enter to send
3. The message will be transmitted via LoRa and appear on the other device
4. Received messages show the sender's address and signal strength (RSSI/SNR)

### Example Communication

**Device 1:**
```
=== RYLR998 Arduino Uno - Device 1 ===
Initializing LoRa module...
Ready to send/receive messages!
Type your message and press Enter to send.
----------------------------------------
Hello from Device 1!
[SENT to Device 2] Hello from Device 1!
[RECEIVED from Device 2] Hi back from Device 2! (RSSI: -45 dBm, SNR: 9 dB)
```

**Device 2:**
```
=== RYLR998 Arduino Uno - Device 2 ===
Initializing LoRa module...
Ready to send/receive messages!
Type your message and press Enter to send.
----------------------------------------
[RECEIVED from Device 1] Hello from Device 1! (RSSI: -47 dBm, SNR: 8 dB)
Hi back from Device 2!
[SENT to Device 1] Hi back from Device 2!
```

## Configuration

Both sketches are pre-configured with:
- **Network ID:** 6 (both devices must use the same network ID)
- **Device 1 Address:** 1
- **Device 2 Address:** 2
- **Baud Rate:** 115200 (RYLR998), 9600 (Serial Monitor)

You can modify these values in the sketch headers if needed.

## RYLR998 AT Commands Reference

The sketches use these AT commands:

- `AT+ADDRESS=<addr>` - Set device address (0-65535)
- `AT+NETWORKID=<id>` - Set network ID (3-15)
- `AT+PARAMETER?` - Query current parameters
- `AT+SEND=<addr>,<len>,<data>` - Send message
- Response: `+RCV=<addr>,<len>,<data>,<RSSI>,<SNR>` - Received message

## Troubleshooting

1. **No response from module:**
   - Check wiring connections
   - Verify power supply (3.3V-5V)
   - Try swapping TX/RX connections if backwards

2. **Messages not received:**
   - Ensure both devices have the same NETWORK_ID
   - Check that addresses are different (1 and 2)
   - Verify antennas are properly connected
   - Test at closer range (start with 1-2 meters)

3. **Garbled messages:**
   - Confirm baud rate is 115200 for RYLR998
   - Check for loose connections
   - Try resetting both modules

## Range

Typical range depends on:
- **Indoor:** 200-500 meters
- **Outdoor (line of sight):** 1-3 km
- **Optimal conditions:** Up to 5 km

## License

Free to use and modify for your projects.
