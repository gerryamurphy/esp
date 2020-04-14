import ujson as json
import os
import urandom as random
import urequests as requests
import utime as time
from dht import DHT11
from machine import Pin

ALTO_SERVER = 'http://10.36.41.175:5000/'
DEBUG_GET_FAKE_DATA = True
DEBUG_GET_FAKE_MAC = True
SLEEP_TIME = 5

class AltoNode(object):
    def __init__(self):
        # self.session = requests.Session()
        self.unique_id = self.get_unique_id()
        self.sensors = {}
        self.sensor = DHT11(Pin(5))


    def upload_data(self):
        #print ("upload_data")
        new_data = self.get_new_data()
        #print (new_data)
        #post_data = ujson.dumps({ 'account': 'user_account', 'password': 'password'})

        post_data=json.dumps({'data': new_data, 'node_id': self.unique_id})
        print(post_data)
        url = ALTO_SERVER + 'api/upload'
        #r = requests.post(ALTO_SERVER+'api/upload', jdata)
        res = requests.post(url, headers = {'content-type': 'application/json'}, data = post_data).json()
        # header_data = ''
        # sleep_time = json.loads(r.text)['sleep_time']
        return SLEEP_TIME

    def get_unique_id(self):
        #print ("get_unique_id")
        if DEBUG_GET_FAKE_MAC:
            return 'unknown_{:04}'.format(random.getrandbits(16))

    def get_new_data(self):
        #print("get_new_data")
        now = int(time.time())
        self.sensor.measure()
        temp = float(self.sensor.temperature())
        temp_f = temp * (9/5) + 32.0
        data = {"28-000005e2ffff": (now, temp_f)}
        return data

    def upload_loop(self):
        #print("upload_loop")
        while True:
            sleep_time = self.upload_data()
            time.sleep(sleep_time)

def run():
    node = AltoNode()

    # def upload_loop(self):
        #print("upload_loop")
    while True:
        sleep_time = node.upload_data()
        time.sleep(sleep_time)

# time.sleep(10)
# run()
    #node.upload_loop()

