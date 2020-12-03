import paho.mqtt.client as mqtt
import threading
import time
import cv2.cv2 as cv2
import numpy as np
import binascii

mqttclient = mqtt.Client()
HOST = "192.168.1.25"
PORT = 1883


class GlobalValue:
    data = [""]


def on_client_connect(client, userdata, flags, rc):
    if rc == 0:
        print('connected!!!')
    elif rc == 3:
        print("Server can't be used")
    else:
        print('Other errors')


def connect():
    mqttclient.connect(HOST, PORT, 60)


def publish(topic, payload, qos):
    mqttclient.publish(topic, payload, qos)


def on_rec(client, userdata, message):
    if message.topic == "img":
        # print(str(message.payload.decode()))
        GlobalValue.data.append(str(message.payload.decode()))


def subscribe(topic, qos):
    mqttclient.subscribe(topic, qos)


def main():
    mqttclient.on_message = on_rec
    mqttclient.on_connect = on_client_connect
    #connect()
    mqttclient.connect(HOST, PORT, 60)
    subscribe('img', 0)
    subscribe('control', 0)
    publish('control', "1", 0)
    imgcount = 0
    # data2 = ""
    while True:
        mqttclient.loop_start()
        if (GlobalValue.data[len(GlobalValue.data) - 1]) == "1":
            # print("Reached EOF")
            data1 = "".join(GlobalValue.data[0:(len(GlobalValue.data) - 1)])
            GlobalValue.data = [""]
            data1 = binascii.a2b_hex(data1)
            # label = "img/image" + str(imgcount) + ".jpg"
            # with open(label, "wb") as image_file:
            #     image_file.write(data1)
            data1 = np.frombuffer(data1, dtype=np.uint8)
            img = cv2.imdecode(data1, 1)
            cv2.imshow("Door", img)
            cv2.waitKey(1)
            # if imgcount >= 40:
            #     exit(0)
            # else:
            imgcount += 1
            publish('CAMcontrol', "1", 0)


if __name__ == '__main__':
    main()