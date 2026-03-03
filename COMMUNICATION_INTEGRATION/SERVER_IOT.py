from flask import Flask, request, jsonify, render_template_string, redirect, url_for, session

app = Flask(__name__)
app.secret_key = "super_secret_key"

USERNAME = "admin"
PASSWORD = "iot123"

# Store data separately for each node
latest_data = {
    1: {"edge": None, "temperature": None},
    3: {"edge": None, "temperature": None}
}

# ---------------- RECEIVE DATA ----------------
@app.route('/data', methods=['POST'])
def receive_data():
    data = request.get_json()

    if not data:
        return jsonify({"status": "No JSON received"}), 400

    node = data.get("node")
    if node in latest_data:
        latest_data[node] = {
            "edge": data.get("edge"),
            "temperature": data.get("temperature")
        }

    print("Received:", data)
    return jsonify({"status": "success"}), 200


# ---------------- LOGIN ----------------
@app.route('/', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        if request.form.get("username") == USERNAME and request.form.get("password") == PASSWORD:
            session['logged_in'] = True
            return redirect(url_for('select_node'))
        return render_template_string(LOGIN_HTML, error="Invalid Credentials")

    return render_template_string(LOGIN_HTML)


# ---------------- SELECT NODE PAGE ----------------
@app.route('/select')
def select_node():
    if not session.get('logged_in'):
        return redirect(url_for('login'))
    return render_template_string(SELECT_HTML)


# ---------------- NODE DASHBOARD ----------------
@app.route('/node/<int:node_id>')
def node_dashboard(node_id):
    if not session.get('logged_in'):
        return redirect(url_for('login'))
    return render_template_string(DASHBOARD_HTML, node_id=node_id)


# ---------------- GET NODE DATA ----------------
@app.route('/latest/<int:node_id>')
def get_latest(node_id):
    if not session.get('logged_in'):
        return jsonify({"error": "Unauthorized"}), 403
    return jsonify(latest_data.get(node_id, {}))


# ---------------- LOGOUT ----------------
@app.route('/logout')
def logout():
    session.clear()
    return redirect(url_for('login'))


# ---------------- HTML TEMPLATES ----------------

LOGIN_HTML = """
<!DOCTYPE html>
<html>
<body style="background:#1e3c72;display:flex;justify-content:center;align-items:center;height:100vh;">
<form method="POST" style="background:white;padding:30px;border-radius:10px;">
<h2>Login</h2>
<input name="username" placeholder="Username" required><br><br>
<input name="password" type="password" placeholder="Password" required><br><br>
<button type="submit">Login</button>
{% if error %}<p style="color:red;">{{error}}</p>{% endif %}
</form>
</body>
</html>
"""

SELECT_HTML = """
<!DOCTYPE html>
<html>
<body style="background:#2a5298;color:white;text-align:center;padding-top:100px;">
<h1>Select Node Dashboard</h1>
<a href="/node/1" style="font-size:25px;color:#00e676;">Open NODE 1</a><br><br>
<a href="/node/3" style="font-size:25px;color:#ffeb3b;">Open NODE 3</a><br><br>
<a href="/logout" style="color:red;">Logout</a>
</body>
</html>
"""

DASHBOARD_HTML = """
<!DOCTYPE html>
<html>
<head>
<title>Node {{node_id}}</title>
<style>
body {
    background: linear-gradient(135deg,#1e3c72,#2a5298);
    display:flex;
    justify-content:center;
    align-items:center;
    height:100vh;
    font-family:Arial;
    color:white;
}
.card {
    background:rgba(255,255,255,0.1);
    padding:40px;
    border-radius:20px;
    text-align:center;
    width:350px;
}
.temp {
    font-size:60px;
    transition:all 0.5s ease;
}
.normal { color:#00e676; }
.warning { color:#ffeb3b; }
.danger { color:#ff1744; animation:pulse 1s infinite; }
@keyframes pulse {
    0%{transform:scale(1);}
    50%{transform:scale(1.1);}
    100%{transform:scale(1);}
}
</style>
</head>
<body>

<div class="card">
<h2>Node {{node_id}} Temperature</h2>
<div id="temp" class="temp">-- °C</div>
<p>Edge: <span id="edge">--</span></p>
<a href="/select" style="color:white;">Back</a>
</div>

<script>
function updateUI(data){
    const t=document.getElementById("temp");
    const e=document.getElementById("edge");

    if(data.temperature==null) return;

    let temp=parseFloat(data.temperature).toFixed(1);
    t.innerText=temp+" °C";
    e.innerText=data.edge;

    t.classList.remove("normal","warning","danger");

    if(temp<28)
        t.classList.add("normal");
    else if(temp<35)
        t.classList.add("warning");
    else
        t.classList.add("danger");
}

function fetchData(){
    fetch('/latest/{{node_id}}')
    .then(r=>r.json())
    .then(d=>updateUI(d));
}

setInterval(fetchData,2000);
fetchData();
</script>

</body>
</html>
"""

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)