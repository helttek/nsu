import psycopg2
import tkinter as tk
from tkinter import ttk

from src.controller.controller import Controller

# def fetch_data():
#     try:
#         with psycopg2.connect(
#                 host="localhost",
#                 database="university_library",
#                 user="admin",
#                 password="admin_password"
#         ) as conn:
#             with conn.cursor() as cur:
#                 cur.execute("SELECT * FROM reader;")
#                 columns = [desc[0] for desc in cur.description]
#                 data = cur.fetchall()
#                 return columns, data
#     except Exception as e:
#         print(f"Error: {e}")
#         return [], []
#
#
# def show_table():
#     # Clear previous contents
#     for widget in frame.winfo_children():
#         widget.destroy()
#
#     columns, data = fetch_data()
#
#     if not columns:
#         return
#
#     tree = ttk.Treeview(frame, columns=columns, show="headings")
#     for col in columns:
#         tree.heading(col, text=col)
#         tree.column(col, anchor='center')
#
#     for row in data:
#         tree.insert("", tk.END, values=row)
#
#     tree.pack(fill=tk.BOTH, expand=True)


if __name__ == "__main__":
    controller = Controller()
    controller.start()
