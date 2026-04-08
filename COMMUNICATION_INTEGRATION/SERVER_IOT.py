from flask import Flask, request, jsonify, render_template_string

app = Flask(__name__)

data_store = {
    "node1_temp": None,
    "node3_temp": None,
    "node1_state": None,
    "node3_state": None
}

@app.route('/data', methods=['POST'])
def receive_data():

    data = request.get_json()

    node = data.get("node")
    temp = data.get("temperature")
    state = data.get("state")

    if node == 1:
        data_store["node1_temp"] = temp
        data_store["node1_state"] = state

    elif node == 3:
        data_store["node3_temp"] = temp
        data_store["node3_state"] = state

    print("Received:", data)

    return jsonify({"status":"ok"})


@app.route('/')
def dashboard():

    html = """
    <html>
    <head>
        <title>Edge IoT Monitor</title>
        <meta http-equiv="refresh" content="2">
        <style>
        body{
            font-family:Arial;
            background:#111;
            color:white;
            text-align:center;
        }

        .container{
            display:flex;
            justify-content:center;
            gap:50px;
            margin-top:60px;
        }

        .card{
            background:#222;
            padding:30px;
            border-radius:10px;
            width:200px;
        }

        .temp{
            font-size:40px;
        }

        .state{
            margin-top:10px;
            font-size:20px;
        }
        </style>
    </head>

    <body>

        <h1>Edge IoT Temperature Dashboard</h1>

        <div class="container">

            <div class="card">
                <h2>Node 1</h2>
                <div class="temp">{{node1_temp}}</div>
                <div class="state">State: {{node1_state}}</div>
            </div>

            <div class="card">
                <h2>Node 3</h2>
                <div class="temp">{{node3_temp}}</div>
                <div class="state">State: {{node3_state}}</div>
            </div>

        </div>

    </body>
    </html>
    """

    return render_template_string(
        html,
        node1_temp=data_store["node1_temp"],
        node3_temp=data_store["node3_temp"],
        node1_state=data_store["node1_state"],
        node3_state=data_store["node3_state"]
    )


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)