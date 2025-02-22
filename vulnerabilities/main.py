from flask import Flask, request, jsonify
import sqlite3
import hashlib
import os

app = Flask(__name__)
app.secret_key = "insecure-secret"  # Insecure hard-coded secret


@app.route("/")
def index():
    return "This site is vulnerable, try /admin, /crypto, /search, /process (POST), or /config."


# --- Broken Access Control ---
@app.route("/admin")
def admin_dashboard():
    return "Admin dashboard: Sensitive data"


# --- Cryptographic Failures ---
@app.route("/crypto")
def crypto():
    msg = request.args.get("msg", "")
    hashed = hashlib.md5(msg.encode()).hexdigest()
    return jsonify({"original": msg, "hashed": hashed})


# --- Injection ---
@app.route("/search")
def search():
    username = request.args.get("username", "")
    conn = sqlite3.connect(":memory:")
    cursor = conn.cursor()
    cursor.execute("CREATE TABLE users (id INTEGER PRIMARY KEY, username TEXT)")
    cursor.execute("INSERT INTO users (username) VALUES ('admin')")
    cursor.execute("INSERT INTO users (username) VALUES ('user')")
    conn.commit()
    query = f"SELECT * FROM users WHERE username = '{username}'"
    try:
        cursor.execute(query)
        results = cursor.fetchall()
    except Exception as e:
        return jsonify({"error": str(e)}), 500
    return jsonify({"query": query, "results": results})


# --- Insecure Design ---
@app.route("/process", methods=["POST"])
def process_data():
    data = request.get_json(force=True)
    return jsonify({"processed_data": data})


# --- Security Misconfiguration ---
@app.route("/config")
def config():
    config_info = {
        "debug": app.debug,
        "secret_key": app.secret_key,
        "environment": dict(os.environ),
    }
    return jsonify(config_info)


if __name__ == "__main__":
    app.run(debug=True)


# token:sqp_8f134664455343069692e7745ca066b6de70b15e
# run: sonar-scanner.bat -D"sonar.projectKey=vulnerabilities_server_test" -D"sonar.sources=." -D"sonar.host.url=http://localhost:9000" -D"sonar.token=sqp_8f134664455343069692e7745ca066b6de70b15e"
