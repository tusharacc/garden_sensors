import paho.mqtt.client as mqtt
import save_data as sd
import json
from datetime import datetime
import time,re

pattern = re.compile("sensor:(?P<sensor>.*),attr:(?P<attr>.*),unit:(?P<unit>.*),value:(?P<value>.*)")
def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    msg_str = str(message.payload.decode("utf-8"))
    regex_srch = pattern.search(msg_str)
    sensor_id = regex_srch['sensor']
    unit = regex_srch['unit']
    attr = regex_srch['attr']
    value = regex_srch['value']
    print (f"The value is {sensor_id}")
    try:
        ret = sd.persist_data({"sensor_id":sensor_id,
                   "readin_dt":datetime.now().strftime("%Y-%m-%d"),
                   "readin_tm":int(time.time()),
                   "sensor_data":f'{{"attribute":"{attr}","value":{value},"unit":"{unit}"}}'
                   })
        if ret['success']:
            print("Data Inserted")
        else:
            print (ret['message'])
            raise ValueError()
    except Exception as ex:
        print (f"The error is {ex}");
    
client = mqtt.Client("P1")
client.connect("127.0.0.1")

client.on_message=on_message
client.subscribe("sensor")
client.loop_forever()
