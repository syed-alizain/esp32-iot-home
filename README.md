# esp32-iot-home
Project to control main electrical distribution box with ESP32

# FAN control

To get the RPM

`GET /get_rpm`

To set the duty cycle (50 - 255)

`GET /set_duty?value=60`

# Temperature & Humidity sensor

`GET /temperature0`
`GET /humidity0`

# Buzzer (Delay range 1 - 5000)

`GET /buzzer?delay=200`

# Light sensor

 GET /ldr

# OUTPUTS for Relays (1 to 16)

 GET /relay1=on
 GET /relay1=off
