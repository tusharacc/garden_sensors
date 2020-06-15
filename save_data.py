import sqlite3

DATABASE_ADDRESS = 'sensors.db'

queries = {
    'insert_reading':"INSERT INTO sensor_readings(sensor_id,readin_dt,readin_tm,sensor_data) VALUES (?,?,?,?)"    
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
        
def save_data(data):
    conn = conn_db()
    c = create_cursor(conn)
    query = (queries['insert_reading'],(data['sensor_id'],data['readin_dt'],data['readin_tm'],data['sensor_data'],),)
    result = insert_data(conn,c,query)
    
    return result

if __name__ == '__main__':
    import time
    from datetime import datetime
    ret = save_data({"sensor_id":"pot1",
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
    