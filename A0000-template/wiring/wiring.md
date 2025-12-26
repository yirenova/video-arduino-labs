# Wiring Notes

## Pin Mapping

| Module Pin | Board Pin | Notes |
|-----------|-----------|------|
| VCC | 3V3 | |
| GND | GND | |
| SCL | GPIOxx | I2C SCL |
| SDA | GPIOxx | I2C SDA |

## Notes
- Keep wires short if the signal is unstable.
- If using a 5V module, verify level shifting requirements.