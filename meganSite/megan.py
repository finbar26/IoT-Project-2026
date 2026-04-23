from flask import Flask, render_template, request, jsonify
import random
import logging




app = Flask(__name__)


ButtonPressed = 0
currentPattern= 'None'

@app.route('/', methods=['GET', 'POST'])
def index():
    global currentPattern
    returnMessage = ''
    if request.method == 'POST':
        value = request.form.get('button')
        if value == '1':
            rainbowPattern()
            returnMessage = 'Rainbow Pattern Activated'
            currentPattern = 'Rainbow'
        elif value == '2':
            blinkPattern()
            returnMessage = 'Blink Pattern Activated'
            currentPattern = 'Blink'
        elif value == '3':
            chasePattern()
            returnMessage = 'Chase Pattern Activated'
            currentPattern = 'Chase'
        else:
            returnMessage = 'No valid button pressed'
    return render_template('site.html', message=returnMessage)

data = {'temperature': [], 'useTempControl': False, 'delayTime': 200} # Example data#


@app.route('/pattern', methods=['GET'])
def send_pattern():
    return currentPattern

@app.route('/data', methods=['POST'])
def receive_data():
    try: 
        content = request.get_json()
        data['temperature'].append(content['temperature'])
        if(content['useTempControl'] == 1):
            data['useTempControl'] = True
        else:
            data['useTempControl'] = False
        data['delayTime'] = content['delayTime']
        print(f'Received temperature: {content["temperature"]}')
        print(f'Temperature Control Mode: {content["useTempControl"]}')
        print(f'Delay Time: {content["delayTime"]}ms')
        return jsonify({'status': True})
    except Exception as e:
        return jsonify({'status': False})

@app.route('/get_data')
def get_data():
    return jsonify(data)


def rainbowPattern():
    print('Rainbow Pattern Activated')

def blinkPattern():
    print('Blink Pattern Activated')

def chasePattern():
    print('Chase Pattern Activated')



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
