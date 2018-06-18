#!/bin/python

# Copyright 2018 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import flask
from flask import request
from google.cloud import pubsub_v1

# From server example
import argparse
import base64
import json
import os
import sys
from threading import Lock
import time

from googleapiclient import discovery
from googleapiclient.errors import HttpError
from oauth2client.service_account import ServiceAccountCredentials

app = flask.Flask(__name__, static_url_path='')

messages = []
subscription_name = 'your-subscription-name'
REGION_ID = 'us-central1'
REGISTRY_ID = 'your-registry-name'
PROJECT_ID = 'your-project-id'


API_SCOPES = ['https://www.googleapis.com/auth/cloud-platform']
API_VERSION = 'v1'
DISCOVERY_API = 'https://cloudiot.googleapis.com/$discovery/rest'
SERVICE_NAME = 'cloudiot'

credentials = ServiceAccountCredentials.from_json_keyfile_name(
        'creds.json', API_SCOPES)
if not credentials:
    sys.exit('Could not load service account credential '
             'from {}'.format(service_account_json))

discovery_url = '{}?version={}'.format(DISCOVERY_API, API_VERSION)

service = discovery.build(
    SERVICE_NAME,
    API_VERSION,
    discoveryServiceUrl=discovery_url,
    credentials=credentials,
    cache_discovery=False)

update_config_mutex = Lock()

def updateConfig(project_id, region, registry_id, device_id, data):
    """Push the data to the given device as configuration."""
    config_data_json = json.dumps(data)
    body = {
        'version_to_update': 0,
        'binary_data': base64.b64encode(
                config_data_json.encode('utf-8')).decode('ascii')
    }

    device_name = ('projects/{}/locations/{}/registries/{}/'
                   'devices/{}'.format(
                       project_id,
                       region,
                       registry_id,
                       device_id))

    request = service.projects().locations().registries().devices(
    ).modifyCloudToDeviceConfig(name=device_name, body=body)

    update_config_mutex.acquire()
    try:
        request.execute()
    except HttpError as e:
        print('Error executing ModifyCloudToDeviceConfig: {}'.format(e))
    finally:
        update_config_mutex.release()

def pubsub_callback(message):
    messages.append(message.data)
    message.ack()
    app.logger.info(message.data)

@app.route('/')
def root():
    return flask.send_from_directory('','index.html')

@app.route('/js/<path:path>')
def send_js(path):
    return flask.send_from_directory('js', path)

@app.route('/messages')
def list_messages():
    return flask.jsonify(messages)

@app.route('/clear')
def clear_messages():
    global messages
    del messages[:]
    messages = []
    return flask.jsonify(messages)

@app.route('/command')
def process_command():
    deviceId = request.values.get('deviceId');
    command = request.values.get('cmd')
    updateConfig(PROJECT_ID, REGION_ID, REGISTRY_ID, deviceId, int(command))
    return flask.jsonify([deviceId, command])

@app.route('/hcf')
def process_command_all():
    devices = ['inky', 'blinky', 'clyde', 'pinky'] # TODO devices list?
    command = request.values.get('cmd')
    for deviceId in devices:
        updateConfig(PROJECT_ID, REGION_ID, REGISTRY_ID, deviceId, int(command))
    return flask.jsonify([devices, command])

@app.before_request
def before_first_request():
    subscriber = pubsub_v1.SubscriberClient()
    subscription_path = subscriber.subscription_path(PROJECT_ID, subscription_name)
    #subscriber.subscribe_experimental(subscription_name, pubsub_callback)
    app.logger.info(subscription_path)
    app.logger.info(dir(app))
    subscriber.subscribe(subscription_path, pubsub_callback)

if __name__ == '__main__':
    app.serve('0.0.0.0', 8080)
