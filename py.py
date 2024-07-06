import serial
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

# Inisialisasi Firebase
cred = credentials.Certificate('sisdig-arduino-uno.json')
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://sisdig-arduino-uno-default-rtdb.asia-southeast1.firebasedatabase.app'
})

# Menghubungkan ke Serial
ser = serial.Serial('COM4', 9600)  # Ganti 'COM3' dengan 'COM4'

while True:
    try:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)

            # Memisahkan data suhu dan intensitas cahaya
            parts = line.split(',')
            temperature = parts[0].split(':')[1].strip().replace(' C', '')
            light_intensity = parts[1].split(':')[1].strip().replace(' %', '')

            # Membuat referensi ke database Firebase
            ref = db.reference('sensorData/singleEntry')  # Menggunakan kunci tetap 'singleEntry'
            ref.set({
                'temperature': temperature,
                'light_intensity': light_intensity
            })

    except KeyboardInterrupt:
        print("Program dihentikan")
        break
    except Exception as e:
        print(f"Error: {e}")
