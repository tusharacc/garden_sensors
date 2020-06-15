import paho.mqtt.client as mqtt
import save_data as sd
import json
from datetime import datetime
import time

def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message typr " ,type(message.payload))
    j = json.loads(str(message.payload.decode("utf-8")))
    ret = sd.save_data({"sensor_id":j['sensor'],
               "readin_dt":datetime.now().strftime("%Y-%m-%d"),
               "readin_tm":int(time.time()),
               "sensor_data":f'{"attribute":{j["sensing"]},"value":{j["value"]},"unit":{j["unit"]}}'
               })
    if ret['success']:
        print("Data Inserted")
    else:
        print (ret['message'])
        raise ValueError()
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)
    
client = mqtt.Client("P1")
client.connect("127.0.0.1")

client.on_message=on_message
client.subscribe("sensors")
client.loop_forever()
