import time
import paho.mqtt.client as mqtt
from send_email import *
import signal
import sys

from test_image import test

capture: bool = False
client = mqtt.Client()


# disconnect if ctrl-c
def signal_handler(sig, frame):
    print('Disconnecting...')
    client.disconnect()
    print('Disconnected')
    sys.exit(0)
# END def signal_handler()


signal.signal(signal.SIGINT, signal_handler)


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("image")
    client.subscribe("cmd")
# END def on_connect()


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    if msg.topic == "cmd":
        global capture

        print("Command received")
        print(msg.payload)
        command = msg.payload.decode("utf-8")
        print(command)

        if command == "picture":
            capture = False
        elif command == "capture":
            capture = True
        # END if

        print(capture)

    elif msg.topic == "image":
        print("Image received")
        temp = time.time()
        path = f'{temp:.10f}'.replace(".", "")
        with open(f"picture/image_{path}.jpg", "wb") as f:
            f.write(msg.payload)
        print(f"Image saved at picture/image_{path}.jpg")

        if capture:
            return

        # test image
        pred = test(f"picture/image_{path}.jpg")

        # send email
        pred = pred.split()
        road_type, road_quality = pred[0], pred[1]
        body = f"Road Type: {road_type}\nRoad Quality: {road_quality}"

        if road_quality == "Good":
            body += "\n\nNo need for repair"
        elif road_quality == "Regular":
            body += "\n\nRepair soon"
        elif road_quality == "Bad":
            body += "\n\nRepair immediately"
        # END if

        send_email(to=RECV, subject="UbiComp Prediction", message=body)
        print('Email Sent')

    # END if
# END def on_message()


client.on_connect = on_connect
client.on_message = on_message

client.connect("utsav-msi-lappy")

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
