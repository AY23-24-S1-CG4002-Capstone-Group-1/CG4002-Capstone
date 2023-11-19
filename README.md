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
# External Comms

## Directories
- ```html```: Eval server frontend code provided by mod
- ```server```: Eval server backend code provided by mod
- ```model_wk13_5```: The code used to run the final wk13 eval

## Files
- ```brokertest.py```: Test file used to test the latency of brokers
- ```mqttbrokertimings.txt```: Txt file containing the broker latencies
- ```engine.py```: Python file containing game logic
- ```predict.py```: Python file containing the HWAI function
- ```standardisations.py```: Extension of HWAI stuff used for preprocessing
- ```relay-int.py```: Mock relay used to test remotely without hardware
- ```ultra96FS.py```: The main program that runs on the Ultra96. Functionally identical to the one in ```model_wk13_5```, but with more documentation.

## Processes Overview
![image](https://github.com/ChewyChair/cg4002extcomms/assets/69298017/ad957d0d-c08a-42fa-b491-a8814c4caa96)
The Python ```multiprocessing``` library is used to start 6-7 processes. (The ```EvalClient```  is not started in freeplay.) The processes are as follows:
- ```RelayClient```, the client that will communicate via TCP with the relay nodes. There are two of these processes, one for each player. Each process will send messages from the gun and vests to the GunLogic process and send motion data to their respective ```Classification``` process.
- ```Classification```, for which there are two processes, each of which will receive sensor data forwarded from its ```RelayClient```. When receiving sensor data, it writes it into a buffer. 
Default timeout is none but when it receives packets timeout is set to the **SENSOR_WINDOW**. After an action has been identified the timeout is reset back to none.
If it receives 30 packets it proceeds with classification
On timeout, if it received more than 28 packets it proceeds with classification, otherwise it discards it.
After an action has been classified, if it is within the **DOUBLE_ACTION_WINDOW** of the previous action, it will be discarded since it is likely to have been a misfire. This window is extended after identifying a hammer action as well, since it even has a tendency to triple fire. Otherwise it will be sent via MQTT to the visualisers by publishing to *lasertag/vizgamestate*, and the buffer will be reset. All messages published from here have the type query. The visualisers will check if the opponent is visible on screen and reply if it is a hit or miss via *lasertag/vizhit*.
- ```GunLogic```, which takes in gun and vest messages from both players and determines if the guns hit or miss. This process reads from the gun queue. By default it has no timeout. This implementation is based on queues and timeouts. The in depth explanation can be found in ```ultra96FS.py```.
The result will be sent to the engine via the to_engine_queue.
- ```GameEngine```, the game engine itself. This will subscribe to *lasertag/vizhit* and put all replies from the visualiser on the to_engine_queue, which is also the same queue accessed by the ```GunLogic``` process. The engine reads from this queue and updates the game state based on hits and misses. 
If it is not in freeplay mode, it then sends the updated game state to the ```EvalClient``` for verification and waits for the reply. If it has been desynced from the evaluation server, it will overwrite its current game state. 
Finally, it will publish to *lasertag/vizgamestate* with the type update. The visualisers will draw the animations and update the UI based on these messages. The game engine also has several guards in place during evaluation, which can be found in section 6.6.
- ```EvalClient```, the client that communicates via TCP with the evaluation server. It receives the JSON containing the player_id, action and game_state from the ```GameEngine```, sends it to the eval server, waits for the reply containing the correct game_state, then sends it back to the ```GameEngine```. Since the ```GameEngine``` blocks while waiting for the reply, the ```EvalClient``` effectively runs sequentially with the ```GameEngine``` by design. The ```EvalClient``` is not started in freeplay.


## Design choices
This program was written using multiprocessing and uses no polling, only blocking queues and timeouts to minimise CPU load.
```ultra96FS.py``` is pretty well documented, see it for more information.
