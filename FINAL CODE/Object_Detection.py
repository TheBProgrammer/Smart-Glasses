from imageai.Detection import ObjectDetection
from pathlib import Path
import paho.mqtt.client as paho
import cv2
import os

broker = "192.168.1.25"
port = 1883

def on_publish(client,userdata,result):
	print("Signal Sent \n")
	pass

try:

	while True:
		y = False
		execution_path = os.getcwd()
		detector = ObjectDetection()
		detector.setModelTypeAsRetinaNet()
		detector.setModelPath( os.path.join(execution_path , "resnet50_coco_best_v2.0.1.h5"))
		detector.loadModel()

		custom_objects = detector.CustomObjects(person=True)

		client1 = paho.Client("control1")
		client1.on_publish = on_publish
		client1.connect(broker,port)

		my_file = Path(os.path.join(execution_path , "image_car.jpg"))
		if (my_file.is_file()):
			y = True

		while (y):
			detections = detector.detectCustomObjectsFromImage(input_image=os.path.join(execution_path , "image_car.jpg"), output_image_path=os.path.join(execution_path , "image_car_new.jpg"))
			for eachObject in detections:
				print(eachObject["name"] , " : " , eachObject["percentage_probability"] )
				if (eachObject["name"] == 'person'):
					h = 'Yes' 
					d = 'Y'
					print('1')
					break
				else:
					h = 'No'
					d = 'N'
					print('2')

			print("Person Detected : ",h)
			client1.publish("human/detected",d)
			y = False
			os.remove("image_car.jpg")

except KeyboardInterrupt:
    println('\ninterrupted!')
