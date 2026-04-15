from flask import Flask, request, jsonify, render_template_string, redirect, url_for, session
from pymongo import MongoClient
from datetime import datetime

app = Flask(__name__)
app.secret_key = "supersecretkey"

# ---------- CONFIG ----------
USERNAME = "admin"
PASSWORD = "1234"

# ---------- MONGODB ----------
client = MongoClient("mongodb://localhost:27017/")
db = client["iot_db"]
collection = db["data"]

# ---------- DATABASE FUNCTIONS ----------
def insert_data(node, temp, state):
    collection.insert_one({
        "node": node,
        "temperature": temp,
        "state": state,
        "timestamp": datetime.now()
    })

def get_latest():
    rows = list(collection.find().sort("_id", -1).limit(10))

    result = {1: ("--", "--"), 3: ("--", "--")}

    for row in rows:
        node = row.get("node")
        temp = row.get("temperature")
        state = row.get("state")

        if node in result and result[node][0] == "--":
            result[node] = (temp, state)

    return result

# ---------- DEBUG ROUTES ----------
@app.route('/show')
def show_db():
    data = list(collection.find().sort("_id", -1))
    return str(data)

# ---------- LOGIN ----------
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        user = request.form.get("username")
        pwd = request.form.get("password")

        if user == USERNAME and pwd == PASSWORD:
            session['logged_in'] = True
            return redirect(url_for('dashboard'))
        else:
            return "Invalid credentials"

    return """
    <html>
    <body style="background:#111;color:white;text-align:center;margin-top:100px;">
        <h2>Login</h2>
        <form method="POST">
            <input name="username" placeholder="Username"><br><br>
            <input name="password" type="password" placeholder="Password"><br><br>
            <button type="submit">Login</button>
        </form>
    </body>
    </html>
    """

# ---------- API ----------
@app.route('/data', methods=['POST'])
def receive_data():
    data = request.get_json()

    node = data.get("node")
    temp = data.get("temperature")
    state = data.get("state")

    insert_data(node, temp, state)

    print("Received:", data)

    return jsonify({"status": "ok"})

@app.route('/latest')
def latest():
    data = get_latest()

    return jsonify({
        "node1_temp": data[1][0],
        "node1_state": data[1][1],
        "node3_temp": data[3][0],
        "node3_state": data[3][1],
    })

# ---------- DASHBOARD ----------
@app.route('/')
def dashboard():
    if not session.get('logged_in'):
        return redirect(url_for('login'))

    html = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Edge IoT Dashboard</title>

        <style>
        body {
            font-family: Arial;
            background: linear-gradient(135deg,#0f2027,#203a43,#2c5364);
            color: white;
            text-align: center;
        }

        h1 { margin-top: 20px; }

        .container {
            display: flex;
            justify-content: center;
            gap: 40px;
            margin-top: 50px;
        }

        .card {
            background: rgba(255,255,255,0.1);
            padding: 30px;
            border-radius: 15px;
            width: 220px;
            backdrop-filter: blur(10px);
            box-shadow: 0 0 20px rgba(0,0,0,0.5);
        }

        .temp { font-size: 45px; margin-top: 10px; }
        .state { margin-top: 10px; font-size: 20px; }

        .status-low { color: #00ffcc; }
        .status-mid { color: #ffcc00; }
        .status-high { color: #ff4444; }

        .logout {
            position: absolute;
            right: 20px;
            top: 20px;
        }

        </style>
    </head>

    <body>

        <a class="logout" href="/logout">Logout</a>

        <h1>🌐 Edge IoT Monitor</h1>

        <div class="container">

            <div class="card">
                <h2>Node 1</h2>
                <div id="n1_temp" class="temp">--</div>
                <div id="n1_state" class="state">--</div>
            </div>

            <div class="card">
                <h2>Node 3</h2>
                <div id="n3_temp" class="temp">--</div>
                <div id="n3_state" class="state">--</div>
            </div>

        </div>

        <script>
        function getStateText(state){
            if(state == 0) return "LOW";
            if(state == 1) return "MEDIUM";
            if(state == 2) return "HIGH";
            return "--";
        }

        function getClass(state){
            if(state == 0) return "status-low";
            if(state == 1) return "status-mid";
            if(state == 2) return "status-high";
            return "";
        }

        function updateData(){
            fetch('/latest')
            .then(res => res.json())
            .then(data => {

                document.getElementById("n1_temp").innerHTML = data.node1_temp + " °C";
                document.getElementById("n3_temp").innerHTML = data.node3_temp + " °C";

                let s1 = getStateText(data.node1_state);
                let s3 = getStateText(data.node3_state);

                let c1 = getClass(data.node1_state);
                let c3 = getClass(data.node3_state);

                let n1s = document.getElementById("n1_state");
                let n3s = document.getElementById("n3_state");

                n1s.innerHTML = "State: " + s1;
                n3s.innerHTML = "State: " + s3;

                n1s.className = "state " + c1;
                n3s.className = "state " + c3;
            });
        }

        setInterval(updateData, 2000);
        updateData();
        </script>

    </body>
    </html>
    """

    return render_template_string(html)

@app.route('/logout')
def logout():
    session.clear()
    return redirect(url_for('login'))

# ---------- RUN ----------
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)