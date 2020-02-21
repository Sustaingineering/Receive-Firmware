import firebase_admin
from firebase_admin import db
from firebase_admin import credentials
import serial
import math
import sys
from time import time as t

ORDER = [
    "time-stamp",
    "voltage",
    "current",
    "power",
    # "op-temp",
    "surface-temperature",
    "water-breaker",
]


class GSMSerialHandler:
    def __init__(self, port, baud):
        self.ser = serial.Serial(port=port, baudrate=baud)  # Maybe add a timeout

    def get_line_from_serial(self):
        try:
            return self.ser.readline().decode("utf-8")
        except UnicodeDecodeError:
            return ""


class FirebaseHandler:
    def __init__(self, creds):
        self.fb = firebase_admin.initialize_app(
            creds,
            options={"databaseURL": "https://sustaingineering-horus.firebaseio.com"},
        )

    def send_data(self, dest, data):
        db.reference(path=dest).child(str(int(data["time-stamp"]))).set(data)


# Expects a valid CSV string, does not do any validation
# Also expects the json to contain
def parse_csv(string, order):
    shape = {
        "power": 0,
        "surface-temperature": 0,
        "op-temp": 0,
        "current": 0,
        "water-breaker": 0,
        "time-stamp": 0,
        "voltage": 0,
    }.copy()
    for index, value in enumerate(string.split(",")):
        if order[index] == "time-stamp":
            shape[order[index]] = int(value)
        else:
            shape[order[index]] = float(value)
    return shape


# Validates that the string is a CSV in a super lazy way
def validate_valid_csv(string, item_count):
    c = string.count(",")
    return c == item_count - 1


if __name__ == "__main__":
    serial_handler = GSMSerialHandler(sys.argv[1], sys.argv[2])
    firebase_handler = FirebaseHandler(credentials.Certificate("serviceAccount.json"))

    while True:
        line = serial_handler.get_line_from_serial()
        if line != "\n":
            print(line)
        if validate_valid_csv(line, len(ORDER)):
            json = parse_csv(line, ORDER)
            firebase_handler.send_data(sys.argv[3], json)
