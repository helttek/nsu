# vulnerable_app.py
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

app = Flask(__name__)
app.secret_key = "a_very_insecure_secret_key"  # Should be strong and secret
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
            "INSERT INTO users (username, password) VALUES (?, ?)", ("user", "qwerty")
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
            "INSERT INTO items (name, owner_id) VALUES (?, ?)", ("Admin Item", 1)
        )
        cursor.execute(
            "INSERT INTO items (name, owner_id) VALUES (?, ?)", ("User Item 1", 2)
        )
        cursor.execute(
            "INSERT INTO items (name, owner_id) VALUES (?, ?)", ("User Item 2", 2)
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
        db.row_factory = sqlite3.Row  # Return rows as dictionary-like objects
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


# Login Page
@app.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")
        db = get_db()
        cursor = db.cursor()

        # VERY INSECURE: Plain text password check (A02: Cryptographic Failures)
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
            # Generic error message (could be part of A05: Security Misconfiguration)
            return (
                render_template_string(
                    """
                <h1>Login</h1>
                {% with messages = get_flashed_messages(with_categories=true) %}
                  {% if messages %}
                    {% for category, message in messages %}
                      <p style="color: {{ 'red' if category == 'danger' else 'green' }}">{{ message }}</p>
                    {% endfor %}
                  {% endif %}
                {% endwith %}
                <form method="post">
                    Username: <input type="text" name="username"><br>
                    Password: <input type="password" name="password"><br>
                    <input type="submit" value="Login">
                </form>
            """
                ),
                401,
            )  # Unauthorized status
    return render_template_string(
        """
        <h1>Login</h1>
        <form method="post">
            Username: <input type="text" name="username"><br>
            Password: <input type="password" name="password"><br>
            <input type="submit" value="Login">
        </form>
        <p>Try: admin / password123 or user / qwerty</p>
    """
    )


@app.route("/logout")
def logout():
    session.clear()
    flash("You have been logged out.", "info")
    return redirect(url_for("index"))


# Search Page - Vulnerable to SQL Injection
@app.route("/search", methods=["GET", "POST"])
def search():
    results = []
    search_term = ""
    if request.method == "POST":
        search_term = request.form.get("term", "")
        db = get_db()
        cursor = db.cursor()
        try:
            # !! VULNERABILITY: SQL Injection (A03: Injection) !!
            # Example exploit: Enter `' OR '1'='1` into the search box
            query = f"SELECT name FROM items WHERE name LIKE '%{search_term}%'"
            print(
                f"Executing (vulnerable) query: {query}"
            )  # Logging query can also be risky
            cursor.execute(query)
            results = cursor.fetchall()
        except Exception as e:
            # Debug mode will show detailed errors (A05: Security Misconfiguration)
            print(f"Database error: {e}")  # Also potentially leaking info
            flash(f"An error occurred: {e}", "danger")  # Displaying raw errors

    return render_template_string(
        """
        <h1>Search Items</h1>
        <form method="post">
            Search Term: <input type="text" name="term" value="{{ search_term }}">
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
        search_term=search_term,
    )


# Admin Page - Vulnerable to Broken Access Control
@app.route("/admin")
def admin():
    # !! VULNERABILITY: Broken Access Control (A01) !!
    # No check here to see if the user is actually an admin.
    # Anyone knowing the URL can access it.
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


# Page to input URL for fetching
@app.route("/fetch_url_page")
def fetch_url_page():
    return render_template_string(
        """
        <h1>Fetch URL Content (INSECURE SSRF)</h1>
        <form action="/fetch_url" method="post">
            Enter URL to fetch: <input type="text" name="url" size="50">
            <input type="submit" value="Fetch">
        </form>
        <p>Try fetching external URLs (e.g., https://example.com) or potentially internal ones if running locally (e.g., http://127.0.0.1:5000/admin).</p>
         <p><a href="/">Back Home</a></p>
    """
    )


# URL Fetching Endpoint - Vulnerable to SSRF
@app.route("/fetch_url", methods=["POST"])
def fetch_url():
    url = request.form.get("url")
    content = ""
    error = None
    if url:
        try:
            # !! VULNERABILITY: Server-Side Request Forgery (A10: SSRF) !!
            # The application makes a request to a URL supplied by the user
            # without any validation. Attacker could provide internal IPs, file:// URIs, etc.
            print(f"Fetching URL (vulnerable): {url}")
            response = requests.get(url, timeout=5)  # Timeout to prevent hanging
            response.raise_for_status()  # Raise HTTPError for bad responses (4xx or 5xx)
            # Be careful displaying content - could lead to XSS if content is HTML/JS
            content = response.text
        except requests.exceptions.RequestException as e:
            error = f"Error fetching URL: {e}"
            # Debug mode will show detailed errors (A05: Security Misconfiguration)
            print(error)
        except Exception as e:
            error = f"An unexpected error occurred: {e}"
            # Debug mode will show detailed errors (A05: Security Misconfiguration)
            print(error)

    return render_template_string(
        """
        <h1>URL Content</h1>
        {% if error %}
            <p style="color:red;">{{ error }}</p>
        {% else %}
            <p>Content fetched from: {{ url }}</p>
            <pre>{{ content }}</pre> {# Using <pre> to somewhat mitigate raw HTML rendering #}
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
    # !! VULNERABILITY: Running with debug=True in production (A05: Security Misconfiguration) !!
    app.run(debug=True)
