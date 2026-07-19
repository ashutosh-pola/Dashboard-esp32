import subprocess
import time
import json
import os
from datetime import datetime
from flask import Flask, jsonify, request
from flask_cors import CORS
import threading

FILE_NAME = "usage.json"

app = Flask(__name__)
CORS(app) 

current_media = {"playing": False, "title": "No Media", "artist": "Idle", "progress": 0}

def today():
    return datetime.now().strftime("%Y-%m-%d")

if os.path.exists(FILE_NAME):
    with open(FILE_NAME, "r") as file:
        data = json.load(file)
else:
    data = {"date": today(), "apps": {}}

if data["date"] != today():
    data = {"date": today(), "apps": {}}

def save():
    with open(FILE_NAME, "w") as file:
        json.dump(data, file, indent=4)

def get_active_app():
    script = 'tell application "System Events" to name of first application process whose frontmost is true'
    try:
        app_name = subprocess.check_output(["osascript", "-e", script], timeout=1)
        return app_name.decode().strip()
    except:
        return "Unknown"

@app.route("/update_media", methods=['POST'])
def update_media():
    global current_media
    current_media = request.json
    return jsonify({"status": "success"})

@app.route("/media")
def media():
    return jsonify(current_media)

@app.route("/usage")
def usage():
    return jsonify(data["apps"])

def server():
    app.run(host="0.0.0.0", port=5000)

threading.Thread(target=server, daemon=True).start()

while True:
    if data["date"] != today():
        data["date"] = today()
        data["apps"] = {}
    current = get_active_app()
    if current not in data["apps"]:
        data["apps"][current] = 0
    data["apps"][current] += 2
    save()
    time.sleep(2)