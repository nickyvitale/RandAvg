from random import randrange
from time import sleep
import paho.mqtt.client as mqtt
from time import time

if __name__ == "__main__":
    server = "127.0.0.1"
    client = mqtt.Client(str(time()))
    client.connect(server)
    while (1):
        randnum = randrange(11)
        client.publish("random", randnum)
        print("Published: ", randnum)
        sleep(randrange(30))
