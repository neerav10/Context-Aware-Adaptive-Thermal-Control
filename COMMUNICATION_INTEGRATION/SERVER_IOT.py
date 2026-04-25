# from flask import Flask, request, jsonify, render_template_string, redirect, url_for, session
# import sqlite3
# from datetime import datetime
# import os

# app = Flask(__name__)
# app.secret_key = "supersecretkey"   # 🔐 required for login session

# # ---------- CONFIG ----------
# USERNAME = "admin"
# PASSWORD = "1234"

# # ---------- DATABASE ----------
# def init_db():
#     conn = sqlite3.connect("iot.db")
#     c = conn.cursor()
#     c.execute("""
#         CREATE TABLE IF NOT EXISTS data (
#             id INTEGER PRIMARY KEY AUTOINCREMENT,
#             node INTEGER,
#             temperature REAL,
#             state INTEGER,
#             timestamp TEXT
#         )
#     """)
#     conn.commit()
#     conn.close()

# init_db()

# def insert_data(node, temp, state):
#     conn = sqlite3.connect("iot.db")
#     c = conn.cursor()
#     c.execute("INSERT INTO data (node, temperature, state, timestamp) VALUES (?, ?, ?, ?)",
#               (node, temp, state, datetime.now().strftime("%H:%M:%S")))
#     conn.commit()
#     conn.close()

# def get_latest():
#     conn = sqlite3.connect("iot.db")
#     c = conn.cursor()
#     c.execute("SELECT node, temperature, state FROM data ORDER BY id DESC LIMIT 10")
#     rows = c.fetchall()
#     conn.close()

#     result = {1: ("--", "--"), 3: ("--", "--")}
#     for row in rows:
#         node, temp, state = row
#         if node in result and result[node][0] == "--":
#             result[node] = (temp, state)
#     return result

# # ---------- DEBUG ROUTES ----------
# @app.route('/checkdb')
# def check_db():
#     return os.path.abspath("iot.db")

# @app.route('/show')
# def show_db():
#     conn = sqlite3.connect("iot.db")
#     c = conn.cursor()
#     c.execute("SELECT * FROM data ORDER BY id DESC")
#     rows = c.fetchall()
#     conn.close()
#     return str(rows)

# # ---------- LOGIN ----------
# @app.route('/login', methods=['GET', 'POST'])
# def login():
#     if request.method == 'POST':
#         user = request.form.get("username")
#         pwd = request.form.get("password")

#         if user == USERNAME and pwd == PASSWORD:
#             session['logged_in'] = True
#             return redirect(url_for('dashboard'))
#         else:
#             return "Invalid credentials"

#     return """
#     <html>
#     <head>
#         <title>Login - Edge IoT</title>
#         <style>
#             body { background: #0f2027; color: white; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }
#             .login-box { background: rgba(255,255,255,0.05); padding: 40px; border-radius: 20px; backdrop-filter: blur(15px); box-shadow: 0 8px 32px rgba(0,0,0,0.5); border: 1px solid rgba(255,255,255,0.1); text-align: center; }
#             input { width: 100%; padding: 10px; margin: 10px 0; border-radius: 8px; border: none; outline: none; background: rgba(255,255,255,0.1); color: white; }
#             button { background: #00ffcc; color: #111; border: none; padding: 10px 20px; font-weight: bold; border-radius: 8px; cursor: pointer; transition: 0.3s; width: 100%; margin-top: 15px;}
#             button:hover { background: #00ccaa; box-shadow: 0 0 15px rgba(0,255,204,0.5); }
#         </style>
#     </head>
#     <body>
#         <div class="login-box">
#             <h2>🔒 System Login</h2>
#             <form method="POST">
#                 <input name="username" placeholder="Username" required>
#                 <input name="password" type="password" placeholder="Password" required>
#                 <button type="submit">Access Dashboard</button>
#             </form>
#         </div>
#     </body>
#     </html>
#     """

# # ---------- API ----------
# @app.route('/data', methods=['POST'])
# def receive_data():
#     data = request.get_json()
#     node = data.get("node")
#     temp = data.get("temperature")
#     state = data.get("state")
#     insert_data(node, temp, state)
#     print("Received:", data)
#     return jsonify({"status": "ok"})

# @app.route('/latest')
# def latest():
#     data = get_latest()
#     return jsonify({
#         "node1_temp": data[1][0], "node1_state": data[1][1],
#         "node3_temp": data[3][0], "node3_state": data[3][1],
#     })

# # NEW ENDPOINT: Fetch last 5 readings for the graph
# @app.route('/history')
# def history():
#     conn = sqlite3.connect("iot.db")
#     c = conn.cursor()
    
#     # Subquery gets latest 5, outer query orders them chronologically (oldest to newest for the graph left-to-right)
#     c.execute("SELECT temperature, timestamp FROM (SELECT * FROM data WHERE node=1 ORDER BY id DESC LIMIT 5) ORDER BY id ASC")
#     n1 = c.fetchall()
    
#     c.execute("SELECT temperature, timestamp FROM (SELECT * FROM data WHERE node=3 ORDER BY id DESC LIMIT 5) ORDER BY id ASC")
#     n3 = c.fetchall()
#     conn.close()

#     # Use the labels from whichever node has more data points (usually they will be equal)
#     labels = [row[1] for row in n1] if len(n1) > len(n3) else [row[1] for row in n3]

#     return jsonify({
#         "labels": labels,
#         "n1_temps": [row[0] for row in n1],
#         "n3_temps": [row[0] for row in n3]
#     })

# # ---------- DASHBOARD ----------
# @app.route('/')
# def dashboard():
#     if not session.get('logged_in'):
#         return redirect(url_for('login'))

#     html = """
#     <!DOCTYPE html>
#     <html>
#     <head>
#         <title>Edge IoT Dashboard</title>
#         <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
#         <style>
#         body {
#             font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
#             background: linear-gradient(135deg, #0f2027, #203a43, #2c5364);
#             color: white;
#             text-align: center;
#             margin: 0;
#             padding: 20px;
#             min-height: 100vh;
#         }

#         h1 { margin-top: 10px; font-weight: 300; letter-spacing: 2px; }

#         .dashboard-wrapper {
#             max-width: 900px;
#             margin: 0 auto;
#             display: flex;
#             flex-direction: column;
#             gap: 30px;
#         }

#         .cards-container {
#             display: flex;
#             justify-content: center;
#             gap: 40px;
#             flex-wrap: wrap;
#         }

#         .card {
#             background: rgba(255, 255, 255, 0.05);
#             padding: 30px;
#             border-radius: 20px;
#             width: 250px;
#             backdrop-filter: blur(15px);
#             border: 1px solid rgba(255, 255, 255, 0.1);
#             box-shadow: 0 8px 32px rgba(0,0,0,0.3);
#             transition: transform 0.3s ease;
#         }

#         .card:hover { transform: translateY(-5px); }
#         .card h2 { margin: 0; font-size: 20px; color: #aaa; text-transform: uppercase; letter-spacing: 1px;}
#         .temp { font-size: 55px; font-weight: bold; margin: 15px 0; text-shadow: 0 0 20px rgba(255,255,255,0.2); }
#         .state { font-size: 18px; padding: 5px 15px; border-radius: 20px; display: inline-block; font-weight: bold; }

#         .chart-container {
#             background: rgba(255, 255, 255, 0.05);
#             padding: 20px;
#             border-radius: 20px;
#             backdrop-filter: blur(15px);
#             border: 1px solid rgba(255, 255, 255, 0.1);
#             box-shadow: 0 8px 32px rgba(0,0,0,0.3);
#             height: 350px;
#             position: relative;
#         }

#         .status-low { background: rgba(0, 255, 204, 0.2); color: #00ffcc; }
#         .status-mid { background: rgba(255, 204, 0, 0.2); color: #ffcc00; }
#         .status-high { background: rgba(255, 68, 68, 0.2); color: #ff4444; }

#         .top-bar {
#             display: flex;
#             justify-content: space-between;
#             align-items: center;
#             max-width: 900px;
#             margin: 0 auto 30px auto;
#         }

#         .logout {
#             background: rgba(255,255,255,0.1);
#             color: white;
#             text-decoration: none;
#             padding: 8px 20px;
#             border-radius: 20px;
#             transition: 0.3s;
#             border: 1px solid rgba(255,255,255,0.2);
#         }
#         .logout:hover { background: rgba(255,68,68,0.8); }

#         </style>
#     </head>
#     <body>

#         <div class="top-bar">
#             <h1>🌐 EDGE IOT SYSTEM</h1>
#             <a class="logout" href="/logout">Logout</a>
#         </div>

#         <div class="dashboard-wrapper">
#             <div class="cards-container">
#                 <div class="card">
#                     <h2>Node 1</h2>
#                     <div id="n1_temp" class="temp">--</div>
#                     <div id="n1_state" class="state">--</div>
#                 </div>

#                 <div class="card">
#                     <h2>Node 3</h2>
#                     <div id="n3_temp" class="temp">--</div>
#                     <div id="n3_state" class="state">--</div>
#                 </div>
#             </div>

#             <div class="chart-container">
#                 <canvas id="tempChart"></canvas>
#             </div>
#         </div>

#         <script>
#         // --- 1. SETUP UI HELPERS ---
#         function getStateText(state){
#             if(state == 0) return "LOW";
#             if(state == 1) return "MEDIUM";
#             if(state == 2) return "HIGH";
#             return "STANDBY";
#         }

#         function getClass(state){
#             if(state == 0) return "status-low";
#             if(state == 1) return "status-mid";
#             if(state == 2) return "status-high";
#             return "";
#         }

#         // --- 2. SETUP CHART.JS ---
#         const ctx = document.getElementById('tempChart').getContext('2d');
#         const myChart = new Chart(ctx, {
#             type: 'line',
#             data: {
#                 labels: [],
#                 datasets: [
#                     {
#                         label: 'Node 1 Temp (°C)',
#                         borderColor: '#00ffcc',
#                         backgroundColor: 'rgba(0, 255, 204, 0.1)',
#                         data: [],
#                         tension: 0.4, // smooth curves
#                         fill: true
#                     },
#                     {
#                         label: 'Node 3 Temp (°C)',
#                         borderColor: '#ff4444',
#                         backgroundColor: 'rgba(255, 68, 68, 0.1)',
#                         data: [],
#                         tension: 0.4,
#                         fill: true
#                     }
#                 ]
#             },
#             options: {
#                 responsive: true,
#                 maintainAspectRatio: false,
#                 plugins: {
#                     legend: { labels: { color: 'white', font: { family: 'Segoe UI' } } }
#                 },
#                 scales: {
#                     y: { 
#                         beginAtZero: false, 
#                         grid: { color: 'rgba(255,255,255,0.1)' },
#                         ticks: { color: 'white' }
#                     },
#                     x: { 
#                         grid: { display: false },
#                         ticks: { color: 'white' }
#                     }
#                 }
#             }
#         });

#         // --- 3. DATA FETCHING LOOP ---
#         function updateData(){
#             // Update Number Cards
#             fetch('/latest')
#             .then(res => res.json())
#             .then(data => {
#                 document.getElementById("n1_temp").innerHTML = data.node1_temp + "°";
#                 document.getElementById("n3_temp").innerHTML = data.node3_temp + "°";

#                 let n1s = document.getElementById("n1_state");
#                 let n3s = document.getElementById("n3_state");

#                 n1s.innerHTML = getStateText(data.node1_state);
#                 n3s.innerHTML = getStateText(data.node3_state);

#                 n1s.className = "state " + getClass(data.node1_state);
#                 n3s.className = "state " + getClass(data.node3_state);
#             });

#             // Update Graph
#             fetch('/history')
#             .then(res => res.json())
#             .then(data => {
#                 myChart.data.labels = data.labels;
#                 myChart.data.datasets[0].data = data.n1_temps;
#                 myChart.data.datasets[1].data = data.n3_temps;
#                 myChart.update(); // smoothly redraws the chart
#             });
#         }

#         // Run immediately, then every 2 seconds
#         updateData();
#         setInterval(updateData, 500);
#         </script>

#     </body>
#     </html>
#     """

#     return render_template_string(html)

# @app.route('/logout')
# def logout():
#     session.clear()
#     return redirect(url_for('login'))

# # ---------- RUN ----------
# if __name__ == "__main__":
#     print("DB PATH:", os.path.abspath("iot.db"))
#     app.run(host="0.0.0.0", port=5000, debug=True)




















from flask import Flask, request, jsonify, render_template_string, redirect, url_for, session
import sqlite3
from datetime import datetime
import os

app = Flask(__name__)
app.secret_key = "supersecretkey"

# ---------- CONFIG ----------
USERNAME = "admin"
PASSWORD = "1234"

# Force absolute path so we never accidentally create ghost databases
BASE_DIR = os.path.abspath(os.path.dirname(__file__))
DB_PATH = os.path.join(BASE_DIR, "iot.db")

# ---------- DATABASE ----------
def init_db():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("""
        CREATE TABLE IF NOT EXISTS data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            node INTEGER,
            temperature REAL,
            state INTEGER,
            timestamp TEXT
        )
    """)
    conn.commit()
    conn.close()

init_db()

def insert_data(node, temp, state):
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("INSERT INTO data (node, temperature, state, timestamp) VALUES (?, ?, ?, ?)",
              (node, temp, state, datetime.now().strftime("%H:%M:%S")))
    conn.commit()
    conn.close()

def get_latest():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("SELECT node, temperature, state FROM data ORDER BY id DESC LIMIT 10")
    rows = c.fetchall()
    conn.close()

    result = {1: ("--", "--"), 3: ("--", "--")}
    for row in rows:
        node, temp, state = row
        # Safely enforce integer checking
        node = int(node) if node is not None else -1 
        if node in result and result[node][0] == "--":
            result[node] = (temp, state)
    return result

# ---------- DEBUG ROUTES ----------
@app.route('/checkdb')
def check_db():
    return DB_PATH

@app.route('/show')
def show_db():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("SELECT * FROM data ORDER BY id DESC")
    rows = c.fetchall()
    conn.close()
    return str(rows)

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
    <head>
        <title>Login - Edge IoT</title>
        <style>
            body { background: #0f2027; color: white; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }
            .login-box { background: rgba(255,255,255,0.05); padding: 40px; border-radius: 20px; backdrop-filter: blur(15px); box-shadow: 0 8px 32px rgba(0,0,0,0.5); border: 1px solid rgba(255,255,255,0.1); text-align: center; }
            input { width: 100%; padding: 10px; margin: 10px 0; border-radius: 8px; border: none; outline: none; background: rgba(255,255,255,0.1); color: white; }
            button { background: #00ffcc; color: #111; border: none; padding: 10px 20px; font-weight: bold; border-radius: 8px; cursor: pointer; transition: 0.3s; width: 100%; margin-top: 15px;}
            button:hover { background: #00ccaa; box-shadow: 0 0 15px rgba(0,255,204,0.5); }
        </style>
    </head>
    <body>
        <div class="login-box">
            <h2>🔒 System Login</h2>
            <form method="POST">
                <input name="username" placeholder="Username" required>
                <input name="password" type="password" placeholder="Password" required>
                <button type="submit">Access Dashboard</button>
            </form>
        </div>
    </body>
    </html>
    """

# ---------- API ----------
@app.route('/data', methods=['POST'])
def receive_data():
    try:
        # force=True ignores missing headers, silent=True prevents crashes on bad JSON
        data = request.get_json(force=True, silent=True) or {}
        
        # Enforce strict type casting so database and UI match perfectly
        node = int(data.get("node", 0))
        temp = float(data.get("temperature", 0.0))
        state = int(data.get("state", 0))
        
        insert_data(node, temp, state)
        print(f"Data Successfully Logged - Node: {node}, Temp: {temp}, State: {state}")
        return jsonify({"status": "ok"})
    except Exception as e:
        print("Error processing incoming data:", e)
        return jsonify({"status": "error", "message": "Invalid Data Format"}), 400

@app.route('/latest')
def latest():
    data = get_latest()
    return jsonify({
        "node1_temp": data[1][0], "node1_state": data[1][1],
        "node3_temp": data[3][0], "node3_state": data[3][1],
    })

@app.route('/history')
def history():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    
    c.execute("SELECT temperature, timestamp FROM (SELECT * FROM data WHERE node=1 ORDER BY id DESC LIMIT 5) ORDER BY id ASC")
    n1 = c.fetchall()
    
    c.execute("SELECT temperature, timestamp FROM (SELECT * FROM data WHERE node=3 ORDER BY id DESC LIMIT 5) ORDER BY id ASC")
    n3 = c.fetchall()
    conn.close()

    labels = [row[1] for row in n1] if len(n1) > len(n3) else [row[1] for row in n3]

    return jsonify({
        "labels": labels,
        "n1_temps": [row[0] for row in n1],
        "n3_temps": [row[0] for row in n3]
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
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: linear-gradient(135deg, #0f2027, #203a43, #2c5364); color: white; text-align: center; margin: 0; padding: 20px; min-height: 100vh; }
        h1 { margin-top: 10px; font-weight: 300; letter-spacing: 2px; }
        .dashboard-wrapper { max-width: 900px; margin: 0 auto; display: flex; flex-direction: column; gap: 30px; }
        .cards-container { display: flex; justify-content: center; gap: 40px; flex-wrap: wrap; }
        .card { background: rgba(255, 255, 255, 0.05); padding: 30px; border-radius: 20px; width: 250px; backdrop-filter: blur(15px); border: 1px solid rgba(255, 255, 255, 0.1); box-shadow: 0 8px 32px rgba(0,0,0,0.3); transition: transform 0.3s ease; }
        .card:hover { transform: translateY(-5px); }
        .card h2 { margin: 0; font-size: 20px; color: #aaa; text-transform: uppercase; letter-spacing: 1px;}
        .temp { font-size: 55px; font-weight: bold; margin: 15px 0; text-shadow: 0 0 20px rgba(255,255,255,0.2); }
        .state { font-size: 18px; padding: 5px 15px; border-radius: 20px; display: inline-block; font-weight: bold; }
        .chart-container { background: rgba(255, 255, 255, 0.05); padding: 20px; border-radius: 20px; backdrop-filter: blur(15px); border: 1px solid rgba(255, 255, 255, 0.1); box-shadow: 0 8px 32px rgba(0,0,0,0.3); height: 350px; position: relative; }
        .status-low { background: rgba(0, 255, 204, 0.2); color: #00ffcc; }
        .status-mid { background: rgba(255, 204, 0, 0.2); color: #ffcc00; }
        .status-high { background: rgba(255, 68, 68, 0.2); color: #ff4444; }
        .top-bar { display: flex; justify-content: space-between; align-items: center; max-width: 900px; margin: 0 auto 30px auto; }
        .logout { background: rgba(255,255,255,0.1); color: white; text-decoration: none; padding: 8px 20px; border-radius: 20px; transition: 0.3s; border: 1px solid rgba(255,255,255,0.2); }
        .logout:hover { background: rgba(255,68,68,0.8); }
        </style>
    </head>
    <body>
        <div class="top-bar">
            <h1>🌐 EDGE IOT SYSTEM</h1>
            <a class="logout" href="/logout">Logout</a>
        </div>
        <div class="dashboard-wrapper">
            <div class="cards-container">
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
            <div class="chart-container">
                <canvas id="tempChart"></canvas>
            </div>
        </div>

        <script>
        function getStateText(state){
            if(state == 0) return "LOW";
            if(state == 1) return "MEDIUM";
            if(state == 2) return "HIGH";
            return "STANDBY";
        }

        function getClass(state){
            if(state == 0) return "status-low";
            if(state == 1) return "status-mid";
            if(state == 2) return "status-high";
            return "";
        }

        const ctx = document.getElementById('tempChart').getContext('2d');
        const myChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [
                    { label: 'Node 1 Temp (°C)', borderColor: '#00ffcc', backgroundColor: 'rgba(0, 255, 204, 0.1)', data: [], tension: 0.4, fill: true },
                    { label: 'Node 3 Temp (°C)', borderColor: '#ff4444', backgroundColor: 'rgba(255, 68, 68, 0.1)', data: [], tension: 0.4, fill: true }
                ]
            },
            options: {
                responsive: true, maintainAspectRatio: false,
                plugins: { legend: { labels: { color: 'white', font: { family: 'Segoe UI' } } } },
                scales: {
                    y: { beginAtZero: false, grid: { color: 'rgba(255,255,255,0.1)' }, ticks: { color: 'white' } },
                    x: { grid: { display: false }, ticks: { color: 'white' } }
                }
            }
        });

        function updateData(){
            fetch('/latest')
            .then(res => res.json())
            .then(data => {
                document.getElementById("n1_temp").innerHTML = data.node1_temp + "°";
                document.getElementById("n3_temp").innerHTML = data.node3_temp + "°";
                let n1s = document.getElementById("n1_state");
                let n3s = document.getElementById("n3_state");
                n1s.innerHTML = getStateText(data.node1_state);
                n3s.innerHTML = getStateText(data.node3_state);
                n1s.className = "state " + getClass(data.node1_state);
                n3s.className = "state " + getClass(data.node3_state);
            });

            fetch('/history')
            .then(res => res.json())
            .then(data => {
                myChart.data.labels = data.labels;
                myChart.data.datasets[0].data = data.n1_temps;
                myChart.data.datasets[1].data = data.n3_temps;
                myChart.update();
            });
        }

        updateData();
        setInterval(updateData, 500);
        </script>
    </body>
    </html>
    """
    return render_template_string(html)

@app.route('/logout')
def logout():
    session.clear()
    return redirect(url_for('login'))

if __name__ == "__main__":
    print("DB PATH:", DB_PATH)
    app.run(host="0.0.0.0", port=5000, debug=True)