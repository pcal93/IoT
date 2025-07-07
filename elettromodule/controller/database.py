import pymysql.cursors
import tabulate

class Database:
    connection = None

    def __init__(self):
        self

    def connect_db(self):
        if self.connection is not None:
            return self.connection
        else:
            self.connection = pymysql.connect(host="localhost",
                                              user='root',
                                              password="",
                                              database="electromodule",
                                              cursorclass=pymysql.cursors.DictCursor)
            return self.connection
        
    def show_battery_data():
        db = Database()
        connection = db.connect_db()

        with connection.cursor() as new_cursor:
            sql = "SELECT * FROM ( SELECT * FROM battery_data bd ORDER BY id_battery_data DESC LIMIT 10 )battery_data ORDER BY id_battery_data ASC;"
            new_cursor.execute(sql)
            print("----------- BATTERY DATA --------")

            if new_cursor.rowcount == 0:
                print("NO DATA")
                return
            
            results = new_cursor.fetchall()
            header = results[0].keys()
            rows = [x.values() for x in results]
            print(tabulate.tabulate(rows, header, tablefmt='grid'))
    def show_rele_data():
        db = Database()
        connection = db.connect_db()

        with connection.cursor() as new_cursor:
            sql = "SELECT * FROM ( SELECT * FROM rele_data rd ORDER BY id_rele_data DESC LIMIT 10 )rele_data ORDER BY id_rele_data ASC;"
            new_cursor.execute(sql)
            print("----------- RELE' DATA --------")            

            if new_cursor.rowcount == 0:
                print("NO DATA")
                return
            
            results = new_cursor.fetchall()
            header = results[0].keys()
            rows = [x.values() for x in results]
            print(tabulate.tabulate(rows, header, tablefmt='grid'))