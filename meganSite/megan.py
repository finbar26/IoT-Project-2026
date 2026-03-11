from flask import Flask, render_template, request, jsonify
import random


app = Flask(__name__)

ButtonPressed = 0
@app.route('/',methods=['GET', 'POST'])
def buttons():
    if request.method == 'POST':
        pressed = request.form.get("button")
        match int(pressed):
            case 1:
                rainbowPattern()
            case 2:
                firePattern()
            case 3:
                chasePattern()
        return render_template("site.html", ButtonPressed = pressed)
    return render_template("site.html", ButtonPressed = None)

@app.route('/_stuff')
def stuff():
    value = random.randint(0, 10)
    return jsonify(result=value)

def rainbowPattern():
    print("Rainbow Pattern Activated")

def firePattern():
    print("Fire Pattern Activated")

def chasePattern():
    print("Chase Pattern Activated")



if __name__ == '__main__':
    app.run(debug=True)