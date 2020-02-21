# Horus Receive Firmware

This firmware is currently configured to run on an Arduino Uno, but since it's a PlatformIO project, it'll be easy to adapt for any MCU with Arduino Core support.

At the moment, the firmware is intended to handle each message it receives in a stack fashion, at which point it will print to its serial port. A python script should be listening to that port to handle the messages and upload it to Firebase accordingly.

## Building

To build and upload, use the CLI command:
```bash
pio run -t upload
```

Or use the PlatformIO IDE to upload. 

## Upload Hook

Once the Arduino (or whatever MCU) is connected to a computer, navigate to `scripts/` to set up the upload hook. This part assumes you have Python 3.7+ and pip installed and a `serviceAccount.json` file generated in the directory.  

Within the `scripts/` directory, run `pip install -r requirements.txt`. This will install the script dependencies. 

Once installed, the `receive.py` script takes 3 positional arguments. The first argument is the COM/serial port of the MCU, the second argument is the baud rate of the MCU, and the last argument is the sensor ID to upload to. So for example, if the COM port for the MCU is `COM3`, our baud rate is `115200`, and we want to upload to sensor ID `14fa`, run the script like so:
```bash
python receive.py COM3 115200 14fa
```
Or, if your MCU is at `/dev/cu.modem115` with a baud rate of `9600` and sensor ID of `a`, run like so:
```bash
python receive.py /dev/cu.modem115 9600 a
```
And so on.


## `serviceAccount.json`

**DO NOT COMMIT THIS FILE.** It contains credentials that grants admin access to the Firebase instance. 

You can generate one within the Firebase console. Put it in the `scripts/` directory. 

## Note about running in the background

If you're using a Linux machine, you can run the script in the background with a `&` postfix to the command. But if you want to also log the script output, you'll have to set a flag to unbuffer the script output. The full command, using the settings from the second example above, would be:
```bash
python -u receive.py /dev/cu.modem115 9600 a >> output.log &
```
You can then grab the job PID by running `jobs -l` and kill it with `kill $PID`. 
