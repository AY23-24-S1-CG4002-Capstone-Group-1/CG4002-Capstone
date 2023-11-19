# CG4002-Capstone

# Hardware-Internal & External Comms Integration

This portion of the code contains the fully integrated code of hardware sensor, internal communications and external communications.

This repo has separated the code into different segments:

- Arduino Code - Where Hardware Sensor integrates with Internal Communications
- Python Code - Where Internal Communications integrates with External Communications

---

## Arduino Code

This section will provide greater depth to the instructions to execute the code.

### Prerequsite:

To allow the Arduino code to compile successfully, libraries would need to be installed. The libraries required can be installed under Tools -> Manage Libraries:

- CRC8 Library by Rob Tillart
- Adafruit MPU6050 by Adafruit

In the context of a single-player gameplay, 3 Bluno Beetles are required:

- Glove
- Vest
- Gun

Hence, the configuration of Beetles are not required. However, in the context of a two-player game, the number of devices is increased by 2 times.

### Configuration of Two Players:

Configuration of two players are also required in a two-player gameplay settings. This configuration includes the setting of the NeoPixels and the frequency of IR.

This settings is done on the Vest and Gun.

#### Gun Code Changes:

The only changes required in the Bluno Gun code is as follows:

```
int player = 1; // Change the player accordingly
```

#### Vest Code Changes:

The changes required in vest is as follows:

```
#define player 1
//#define player 2

// Changes to the player should be made accordingly
```

With the changes made, all 3 components will work in unison.

---

## Python Code

For the python program to work, the operating system should be in Ubuntu.

### Libraries Required

Most of the libaries used in the code are pre-installed in Python. the libaries that are not pre-installed and require to `pip install` includes:

- bluepy -> `pip install bluepy`
- paho-mqtt -> `pip install paho-mqtt`
- crc8 -> `pip install crc8`

### Running the Code:

The code has been separated into two separated source file for segregation purposes.

Player 1: `bluno_data_transfer_v2_p1.py`
Player 2: `bluno_data_Transfer_V2_p2.py`

To run the code, enter the following command:

```
python bluno_data_transfer_v2_<p1 or p2>.py
```

or

```
python3 bluno_data_transfer_v2_<p1 or p2>.py
```

After you run the code, you will be prompted to input the player number. Enter `1` for the player 1 code and enter `2` for player 2 code.

If the "connected" message is shown, it implies that the connection to external comms is working.

However, do note that for this code to be fully functional, it requires the code in the extcomms to be working as well.

### Data Collection

To collect data, only the Glove needs to be ran. An approach adopted by the team is to comment out the processes that are unused. An example is as shown below:

```
blunoGlove = BlunoGlove()
blunoGun = BlunoGun()
blunoVest = BlunoVest()

# Declare Thread
t1 = Process(target=blunoGlove.run)
t2 = Process(target=blunoGun.run)
t3 = Process(target=blunoVest.run)

relay_client = RelayClient(sn)
relay = Process(target=relay_client.run)

mqtt_client = MQTTClient(sn)
mqtt_thread = Process(target=mqtt_client.run)

t1.start()
# t2.start()
# t3.start()

# relay.start()
# mqtt_thread.start()
```

Note that t2, t3, relay and mqtt_thread has been commented out.

Glove would need to be powered up to begin the data collection. The code will be ran with the command:

```
python bluno_data_transfer_v2_<p1 or p2>.py
Enter the player: <1 or 2>
```

Once entered, data collection will commence. To terminate, enter `CTRL+C` in the keyboard.

Proceed to copy all the data collected in the terminal using Right Click and Copy.

Warning: Please do not use the CTRL+C to copy.

Paste it on a text file and save it.

---
