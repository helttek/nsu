import sqlite3
import os
from flask import (
    Flask,
    request,
    render_template_string,
    g,
    session,
    redirect,
    url_for,
    flash,
)
import requests  # For SSRF example
from flask_wtf import CSRFProtect  # Enable CSRF protection

app = Flask(__name__)
CSRFProtect(app)

# Use a strong secret key in production (e.g. from env)
app.secret_key = os.environ.get("FLASK_SECRET_KEY", "a_very_insecure_secret_key")
DATABASE = "vulnerable_users.db"


# --- Database Setup ---
def init_db():
    if not os.path.exists(DATABASE):
        conn = sqlite3.connect(DATABASE)
        cursor = conn.cursor()
        # VERY INSECURE: Storing passwords in plain text (A02: Cryptographic Failures)
        cursor.execute(
            """
            CREATE TABLE users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT UNIQUE NOT NULL,
                password TEXT NOT NULL,
                is_admin INTEGER DEFAULT 0
            )
        """
        )
        cursor.execute(
            "INSERT INTO users (username, password, is_admin) VALUES (?, ?, ?)",
            ("admin", "password123", 1),
        )
        cursor.execute(
            "INSERT INTO users (username, password) VALUES (?, ?)",
            ("user", "qwerty"),
        )

        cursor.execute(
            """
            CREATE TABLE items (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                owner_id INTEGER,
                FOREIGN KEY(owner_id) REFERENCES users(id)
            )
        """
        )
        cursor.execute(
            "INSERT INTO items (name, owner_id) VALUES (?, ?)",
            ("Admin Item", 1),
        )
        cursor.execute(
            "INSERT INTO items (name, owner_id) VALUES (?, ?)",
            ("User Item 1", 2),
        )
        cursor.execute(
            "INSERT INTO items (name, owner_id) VALUES (?, ?)",
            ("User Item 2", 2),
        )
        conn.commit()
        conn.close()
        print(f"Database {DATABASE} created and initialized.")
    else:
        print(f"Database {DATABASE} already exists.")


def get_db():
    db = getattr(g, "_database", None)
    if db is None:
        db = g._database = sqlite3.connect(DATABASE)
        db.row_factory = sqlite3.Row
    return db


@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, "_database", None)
    if db is not None:
        db.close()


# --- Routes ---


# Home Page
@app.route("/")
def index():
    username = session.get("username")
    return render_template_string(
        """
        <h1>Welcome!</h1>
        {% if username %}
            <p>Logged in as: {{ username }}</p>
            <p><a href="/logout">Logout</a></p>
            <p><a href="/search">Search Items</a></p>
            <p><a href="/fetch_url_page">Fetch URL Content</a></p>
            <p><a href="/admin">Admin Area (Try accessing directly!)</a></p>
        {% else %}
            <p><a href="/login">Login</a></p>
        {% endif %}
        <hr>
        <p>This is a demo vulnerable application.</p>
    """,
        username=username,
    )


# Login: GET form
@app.route("/login", methods=["GET"])
def login_form():
    return render_template_string(
        """
        <h1>Login</h1>
        <form method="post" action="{{ url_for('login_submit') }}">
            {{ csrf_token() }}
            Username: <input type="text" name="username"><br>
            Password: <input type="password" name="password"><br>
            <input type="submit" value="Login">
        </form>
        <p>Try: admin / password123 or user / qwerty</p>
    """
    )


# Login: POST submission
@app.route("/login", methods=["POST"], endpoint="login_submit")
def login_submit():
    username = request.form.get("username")
    password = request.form.get("password")
    db = get_db()
    cursor = db.cursor()

    # VERY INSECURE: Plain text password check
    cursor.execute(
        "SELECT * FROM users WHERE username = ? AND password = ?",
        (username, password),
    )
    user = cursor.fetchone()

    if user:
        session["user_id"] = user["id"]
        session["username"] = user["username"]
        session["is_admin"] = user["is_admin"]
        flash("Login successful!", "success")
        return redirect(url_for("index"))
    else:
        flash("Invalid credentials.", "danger")
        return redirect(url_for("login_form"))


@app.route("/logout")
def logout():
    session.clear()
    flash("You have been logged out.", "info")
    return redirect(url_for("index"))


# Search: GET form
@app.route("/search", methods=["GET"])
def search_form():
    return render_template_string(
        """
        <h1>Search Items</h1>
        <form method="post" action="{{ url_for('search_results') }}">
            {{ csrf_token() }}
            Search Term: <input type="text" name="term"><br>
            <input type="submit" value="Search">
        </form>
         <p><a href="/">Back Home</a></p>
        """
    )


# Search: POST submission
@app.route("/search", methods=["POST"], endpoint="search_results")
def search_results():
    results = []
    search_term = request.form.get("term", "")
    db = get_db()
    cursor = db.cursor()
    try:
        # !! VULNERABILITY: SQL Injection -- for demo only
        query = f"SELECT name FROM items WHERE name LIKE '%{search_term}%'"
        print(f"Executing (vulnerable) query: {query}")
        cursor.execute(query)
        results = cursor.fetchall()
    except Exception as e:
        print(f"Database error: {e}")
        flash(f"An error occurred: {e}", "danger")

    return render_template_string(
        """
        <h1>Search Items</h1>
        <form method="post" action="{{ url_for('search_results') }}">
            {{ csrf_token() }}
            Search Term: <input type="text" name="term" value="{{ request.form.term }}"><br>
            <input type="submit" value="Search">
        </form>
        <h3>Results:</h3>
        {% if results %}
            <ul>
            {% for item in results %}
                <li>{{ item['name'] }}</li>
            {% endfor %}
            </ul>
        {% else %}
            <p>No results found.</p>
        {% endif %}
         <p><a href="/">Back Home</a></p>
    """,
        results=results,
    )


# Admin Page
@app.route("/admin")
def admin():
    db = get_db()
    cursor = db.cursor()
    cursor.execute("SELECT username, is_admin FROM users")
    users = cursor.fetchall()
    return render_template_string(
        """
        <h1>Admin Area (INSECURE)</h1>
        <p>This page should only be visible to administrators.</p>
        <h2>User List</h2>
        <ul>
        {% for user in users %}
            <li>{{ user['username'] }} (Admin: {{ 'Yes' if user['is_admin'] else 'No' }})</li>
        {% endfor %}
        </ul>
         <p><a href="/">Back Home</a></p>
    """,
        users=users,
    )


# Fetch URL Page
@app.route("/fetch_url_page")
def fetch_url_page():
    return render_template_string(
        """
        <h1>Fetch URL Content (INSECURE SSRF)</h1>
        <form action="/fetch_url" method="post">
            {{ csrf_token() }}
            Enter URL to fetch: <input type="text" name="url" size="50">
            <input type="submit" value="Fetch">
        </form>
        <p>Try fetching external URLs (e.g., https://example.com) or potentially internal ones if running locally.</p>
         <p><a href="/">Back Home</a></p>
    """
    )


# URL Fetching Endpoint
@app.route("/fetch_url", methods=["POST"])
def fetch_url():
    url = request.form.get("url")
    content = ""
    error = None
    if url:
        try:
            print(f"Fetching URL (vulnerable): {url}")
            response = requests.get(url, timeout=5)
            response.raise_for_status()
            content = response.text
        except requests.exceptions.RequestException as e:
            error = f"Error fetching URL: {e}"
            print(error)
        except Exception as e:
            error = f"An unexpected error occurred: {e}"
            print(error)

    return render_template_string(
        """
        <h1>URL Content</h1>
        {% if error %}
            <p style="color:red;">{{ error }}</p>
        {% else %}
            <p>Content fetched from: {{ url }}</p>
            <pre>{{ content }}</pre>
        {% endif %}
        <p><a href="/fetch_url_page">Fetch another URL</a></p>
        <p><a href="/">Back Home</a></p>
    """,
        url=url,
        content=content,
        error=error,
    )


if __name__ == "__main__":
    init_db()
    # Disabled debug mode for production
    app.run(debug=False)
