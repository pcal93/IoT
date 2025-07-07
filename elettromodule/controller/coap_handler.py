from coapthon.server.coap import CoAP
from coapthon.resources.resource import Resource
from coapthon.client.helperclient import HelperClient
from coapthon.utils import parse_uri
from database import Database
import datetime




class ResExample(Resource):

    coapAddrs = []

    def __init__(self, name="ResExample"):
        super(ResExample, self).__init__(name)

        self.payload = "Basic Resource"


    def render_GET(self, request):

        if request.payload == "rele":
            ResExample.coapAddrs.append(request.source[0])

        return self


class Put_charging_state():

    def put_charging_state(state, db_state):

        if not ResExample.coapAddrs:
            return
        
        manual = "0"
        state = str(state)
        db_state = db_state

        if state == "1":
            mode = "on"
            db_state = "1"
        elif state == "0":
            mode = "off"
            db_state = "0"
        else:
            mode = state
            manual = "1"

        for x in ResExample.coapAddrs:

            client = HelperClient(server=(x, 5683))
            response = client.put("rele", "mode="+mode)
            timestamp = datetime.datetime.now()
            client.stop()

            db = Database()
            connection = db.connect_db()

            with connection.cursor() as cursor:
                sql = "INSERT INTO `rele_data` (`rele_id`, `timestamp`, `state`, `manual`) VALUES (%s, %s, %s, %s)"
                cursor.execute(sql, (x, timestamp, db_state, manual))
            connection.commit()  