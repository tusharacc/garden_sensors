import sqlite3,time

DATABASE_ADDRESS = 'sensors.db'

queries = {
    'insert_reading':"INSERT INTO sensor_readings(sensor_id,readin_dt,readin_tm,sensor_data) VALUES (?,?,?,?)",
    'get_reading':"SELECT sensor_id,readin_tm,sensor_data FROM sensor_readings where readin_tm >= ?"
}

conn = sqlite3.connect(DATABASE_ADDRESS)

def conn_db():
    try:
        return sqlite3.connect(DATABASE_ADDRESS)
    except Exception as ex:
        raise EnvironmentError(f'Cannot Connect to {DATABASE_ADDRESS}, Reason {ex}')

def create_cursor(conn):
    return conn.cursor()
    
def close_conn(conn):
    conn.close()

def insert_data(conn,c,query):
    try:
        c.execute(*query)
        conn.commit()
        return {'success': True, 'message': None}
    except Exception as ex:
        return {'success': False, 'message': ex}
    

def select_data(conn,c,query):
    data = []
    try:
        c.execute(*query)
        records = c.fetchall()
        for record in records:
            data.append([item for item in record])
        return {'data':data,'success': True, 'message': None}
    except Exception as ex:
        return {'data':data,'success': False, 'message': ex}


def get_data():
    current_time = int(time.time())
    query_time = current_time - 60*60
    query = (queries['get_reading'],(query_time,))
    conn = conn_db()
    c = create_cursor(conn)
    result = select_data(conn,c,query)
    close_conn(conn)
    return result
    
def persist_data(data):
    conn = conn_db()
    c = create_cursor(conn)
    print (f"Save_Date: {data}")
    query = (queries['insert_reading'],(data['sensor_id'],data['readin_dt'],data['readin_tm'],data['sensor_data'],),)
    result = insert_data(conn,c,query)
    close_conn(conn)
    return result

if __name__ == '__main__':
    import time
    from datetime import datetime
    ret = persist_data({"sensor_id":"pot1",
               "readin_dt":datetime.now().strftime("%Y-%m-%d"),
               "readin_tm":int(time.time()),
               "sensor_data":'{"attribute":"temperature","value":"25","unit":"c"}'
               }
              )
    if ret['success']:
        print("Data Inserted")
    else:
        print (ret['message'])
        raise ValueError()
    