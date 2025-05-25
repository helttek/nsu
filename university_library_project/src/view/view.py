import tkinter as tk
from tkinter import ttk
from tkinter import messagebox


class View:
    def __init__(self, controller):
        self.__current_frame = None
        self.__previous_frame = None
        self.__controller = controller
        self.__auth_frame = None
        self.__app_name = "University Library Database"
        self.__window_res = "1000x700"
        self.__window = tk.Tk()
        self.__window.title(self.__app_name + ": Authentication")
        self.__window.geometry(self.__window_res)

        # create authentication frame variables
        self.username_login_entry = None
        self.password_login_entry = None

    def show_start_frame(self):
        self.__display_auth_frame()

    def __display_auth_frame(self):
        self.__current_frame = tk.Frame(self.__window)

        username_label = tk.Label(self.__current_frame, text="Username:")
        self.username_login_entry = tk.Entry(self.__current_frame)
        username_label.pack(anchor="center")
        self.username_login_entry.pack(anchor="center")

        password_label = tk.Label(self.__current_frame, text="Password:")
        self.password_login_entry = tk.Entry(self.__current_frame, show="*")
        password_label.pack(anchor="center")
        self.password_login_entry.pack(anchor="center")

        login_button = tk.Button(self.__current_frame, text="Login", command=self.__on_login_click)
        login_button.pack(anchor="center")

        self.__current_frame.pack(expand=True)
        self.__window.mainloop()

    def __on_login_click(self):
        username = self.username_login_entry.get()
        password = self.password_login_entry.get()
        if self.__controller.authenticate_user(username, password):
            self.__display_menu_frame()
        else:
            messagebox.showerror("Error", "Invalid username or password.")

    def __display_menu_frame(self):
        self.__previous_frame = self.__current_frame
        self.__current_frame = tk.Frame(self.__window)

        insert_form_button = tk.Button(self.__current_frame, text="Insert Form", command=self.__on_insert_from_click)
        insert_form_button.pack(anchor="center", pady=10)

        update_form_button = tk.Button(self.__current_frame, text="Update Form", command=self.__on_update_from_click)
        update_form_button.pack(anchor="center", pady=10)

        search_form_button = tk.Button(self.__current_frame, text="Search Form", command=self.__on_search_from_click)
        search_form_button.pack(anchor="center", pady=10)

        self.__previous_frame.pack_forget()
        self.__window.title(self.__app_name + ": Menu")
        self.__current_frame.pack(expand=True)

    def __on_insert_from_click(self):
        self.__display_insert_form_frame()

    def __on_insert_table_selected(self, event):
        # Clear previous fields
        for widget in self.insert_fields_frame.winfo_children():
            widget.destroy()

        table_name = self.insert_table_var.get()
        if not table_name:
            return

        # Get columns for the selected table
        try:
            columns = self.__controller.model.get_table_columns(table_name)

            # Create header
            header_frame = tk.Frame(self.insert_fields_frame)
            header_frame.pack(fill=tk.X, padx=10, pady=5)

            tk.Label(header_frame, text="Column", font=('Arial', 10, 'bold'), width=20).grid(row=0, column=0, padx=5)
            tk.Label(header_frame, text="Type", font=('Arial', 10, 'bold'), width=15).grid(row=0, column=1, padx=5)
            tk.Label(header_frame, text="Required", font=('Arial', 10, 'bold'), width=10).grid(row=0, column=2, padx=5)
            tk.Label(header_frame, text="Value", font=('Arial', 10, 'bold'), width=30).grid(row=0, column=3, padx=5)

            # Create entry fields for each column
            self.insert_entries = {}
            for i, (column, data_type, is_nullable) in enumerate(columns):
                row_frame = tk.Frame(self.insert_fields_frame)
                row_frame.pack(fill=tk.X, padx=10, pady=2)

                # Column name
                tk.Label(row_frame, text=column, width=20).grid(row=0, column=0, padx=5)

                # Data type
                tk.Label(row_frame, text=data_type, width=15).grid(row=0, column=1, padx=5)

                # Required indicator
                required = "Yes" if is_nullable == "NO" else "No"
                tk.Label(row_frame, text=required, width=10).grid(row=0, column=2, padx=5)

                # Value entry
                entry = tk.Entry(row_frame, width=30)
                entry.grid(row=0, column=3, padx=5)
                self.insert_entries[column] = entry

        except Exception as e:
            messagebox.showerror("Error", f"Could not load table columns: {str(e)}")
            tk.Label(self.insert_fields_frame, text=f"Error: {str(e)}").pack(pady=20)

    def __on_insert_submit(self):
        table_name = self.insert_table_var.get()
        if not table_name:
            messagebox.showerror("Error", "Please select a table first")
            return

        columns = []
        values = []

        # Collect values from entry fields
        for column, entry in self.insert_entries.items():
            value = entry.get().strip()
            if value:
                columns.append(column)
                values.append(value)

        if not columns:
            messagebox.showerror("Error", "Please fill in at least one field")
            return

        try:
            # Create placeholders for the query
            placeholders = ", ".join(["%s"] * len(values))
            column_str = ", ".join(columns)

            query = f"INSERT INTO {table_name} ({column_str}) VALUES ({placeholders})"
            self.__controller.model.execute_query(query, values)

            messagebox.showinfo("Success", "Record inserted successfully!")

            # Clear fields after successful insert
            for entry in self.insert_entries.values():
                entry.delete(0, tk.END)

        except Exception as e:
            messagebox.showerror("Error", f"Failed to insert record: {str(e)}")

    def __on_update_from_click(self):
        self.__display_update_form_frame()

    def __on_update_table_selected(self, event):
        # Clear previous fields
        for widget in self.update_fields_frame.winfo_children():
            widget.destroy()

        table_name = self.update_table_var.get()
        if not table_name:
            return

        # Get columns for the selected table
        try:
            columns = self.__controller.model.get_table_columns(table_name)

            # Create header
            header_frame = tk.Frame(self.update_fields_frame)
            header_frame.pack(fill=tk.X, padx=10, pady=5)

            tk.Label(header_frame, text="Column", font=('Arial', 10, 'bold'), width=20).grid(row=0, column=0, padx=5)
            tk.Label(header_frame, text="Type", font=('Arial', 10, 'bold'), width=15).grid(row=0, column=1, padx=5)
            tk.Label(header_frame, text="Update?", font=('Arial', 10, 'bold'), width=10).grid(row=0, column=2, padx=5)
            tk.Label(header_frame, text="New Value", font=('Arial', 10, 'bold'), width=30).grid(row=0, column=3, padx=5)

            # Create entry fields and checkboxes for each column
            self.update_entries = {}
            self.update_checkboxes = {}

            for i, (column, data_type, is_nullable) in enumerate(columns):
                row_frame = tk.Frame(self.update_fields_frame)
                row_frame.pack(fill=tk.X, padx=10, pady=2)

                # Column name
                tk.Label(row_frame, text=column, width=20).grid(row=0, column=0, padx=5)

                # Data type
                tk.Label(row_frame, text=data_type, width=15).grid(row=0, column=1, padx=5)

                # Checkbox to include in update
                var = tk.BooleanVar()
                checkbox = tk.Checkbutton(row_frame, variable=var)
                checkbox.grid(row=0, column=2, padx=5)
                self.update_checkboxes[column] = var

                # Value entry
                entry = tk.Entry(row_frame, width=30)
                entry.grid(row=0, column=3, padx=5)
                self.update_entries[column] = entry

            # Add hint for WHERE clause
            hint_label = tk.Label(self.update_fields_frame,
                                  text="Note: WHERE clause is required (e.g., 'id = 1' or 'name = 'John'')",
                                  font=('Arial', 9, 'italic'))
            hint_label.pack(pady=10)

        except Exception as e:
            messagebox.showerror("Error", f"Could not load table columns: {str(e)}")
            tk.Label(self.update_fields_frame, text=f"Error: {str(e)}").pack(pady=20)

    def __on_update_submit(self):
        table_name = self.update_table_var.get()
        if not table_name:
            messagebox.showerror("Error", "Please select a table first")
            return

        where_clause = self.where_entry.get().strip()
        if not where_clause:
            messagebox.showerror("Error", "WHERE clause is required for update operations")
            return

        set_clauses = []
        values = []

        # Collect values from checked fields
        for column, checkbox_var in self.update_checkboxes.items():
            if checkbox_var.get():
                value = self.update_entries[column].get().strip()
                set_clauses.append(f"{column} = %s")
                values.append(value)

        if not set_clauses:
            messagebox.showerror("Error", "Please select at least one field to update")
            return

        try:
            # Build and execute the query
            set_clause_str = ", ".join(set_clauses)
            query = f"UPDATE {table_name} SET {set_clause_str} WHERE {where_clause}"
            self.__controller.model.execute_query(query, values)

            messagebox.showinfo("Success", "Record(s) updated successfully!")

            # Clear checkboxes after successful update
            for checkbox_var in self.update_checkboxes.values():
                checkbox_var.set(False)

        except Exception as e:
            messagebox.showerror("Error", f"Failed to update record: {str(e)}")

    def __on_search_from_click(self):
        self.__display_search_form_frame()

    def __on_back_to_menu_click(self):
        self.__display_menu_frame()

    def __display_insert_form_frame(self):
        self.__previous_frame = self.__current_frame
        self.__current_frame = tk.Frame(self.__window)

        # Table selection section
        table_frame = tk.Frame(self.__current_frame)
        table_frame.pack(pady=10, fill=tk.X)

        table_label = tk.Label(table_frame, text="Select table:")
        table_label.pack(side=tk.LEFT, padx=10)

        self.insert_table_var = tk.StringVar()
        self.insert_table_combobox = ttk.Combobox(table_frame, textvariable=self.insert_table_var, width=30)

        try:
            tables = self.__controller.model.get_tables()
            self.insert_table_combobox['values'] = tables
        except Exception as e:
            messagebox.showerror("Database Error", f"Error connecting to database: {str(e)}")

        self.insert_table_combobox.pack(side=tk.LEFT, padx=10)
        self.insert_table_combobox.bind('<<ComboboxSelected>>', self.__on_insert_table_selected)

        # Raw SQL insert section
        raw_frame = tk.Frame(self.__current_frame)
        raw_frame.pack(pady=5, fill=tk.X, padx=10)

        raw_label = tk.Label(raw_frame, text="Or enter raw SQL insert query:")
        raw_label.pack(anchor="w")

        self.raw_insert_statement_entry = tk.Entry(raw_frame, width=75)
        self.raw_insert_statement_entry.pack(fill=tk.X, pady=5)

        raw_button = tk.Button(raw_frame, text="Execute Raw Query", command=self.__on_raw_insert_click)
        raw_button.pack(anchor="e", pady=5)

        separator = ttk.Separator(self.__current_frame, orient='horizontal')
        separator.pack(fill=tk.X, pady=10)

        # Create a frame for the dynamic form fields
        self.insert_fields_container = tk.Frame(self.__current_frame)
        self.insert_fields_container.pack(pady=10, fill=tk.BOTH, expand=True)

        # Scrollable canvas for fields
        self.insert_canvas = tk.Canvas(self.insert_fields_container)
        scrollbar = tk.Scrollbar(self.insert_fields_container, orient=tk.VERTICAL, command=self.insert_canvas.yview)
        self.insert_fields_frame = tk.Frame(self.insert_canvas)

        self.insert_fields_frame.bind(
            "<Configure>",
            lambda e: self.insert_canvas.configure(scrollregion=self.insert_canvas.bbox("all"))
        )

        self.insert_canvas.create_window((0, 0), window=self.insert_fields_frame, anchor="nw")
        self.insert_canvas.configure(yscrollcommand=scrollbar.set)

        self.insert_canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Initial message
        self.insert_placeholder = tk.Label(self.insert_fields_frame,
                                           text="Please select a table to view its fields")
        self.insert_placeholder.pack(pady=50)

        # Button frame
        button_frame = tk.Frame(self.__current_frame)
        button_frame.pack(pady=10, fill=tk.X)

        submit_button = tk.Button(button_frame, text="Submit Insert", command=self.__on_insert_submit)
        submit_button.pack(side=tk.LEFT, padx=10)

        back_button = tk.Button(button_frame, text="Back to Menu", command=self.__on_back_to_menu_click)
        back_button.pack(side=tk.RIGHT, padx=10)

        self.__previous_frame.pack_forget()
        self.__window.title(self.__app_name + ": Insert Form")
        self.__current_frame.pack(expand=True, fill=tk.BOTH)

    def __on_raw_insert_click(self):
        try:
            query = self.raw_insert_statement_entry.get()
            if not query:
                messagebox.showerror("Error", "Please enter a query")
                return

            self.__controller.send_query(query)
            messagebox.showinfo("Success", "Insert query executed successfully!")
            self.raw_insert_statement_entry.delete(0, tk.END)
        except Exception as e:
            messagebox.showerror("Error", f"Query execution failed: {str(e)}")

    def __display_update_form_frame(self):
        self.__previous_frame = self.__current_frame
        self.__current_frame = tk.Frame(self.__window)

        # Table selection section
        table_frame = tk.Frame(self.__current_frame)
        table_frame.pack(pady=10, fill=tk.X)

        table_label = tk.Label(table_frame, text="Select table:")
        table_label.pack(side=tk.LEFT, padx=10)

        self.update_table_var = tk.StringVar()
        self.update_table_combobox = ttk.Combobox(table_frame, textvariable=self.update_table_var, width=30)

        try:
            # Get tables from the database
            tables = self.__controller.model.get_tables()
            self.update_table_combobox['values'] = tables
        except Exception as e:
            messagebox.showerror("Database Error", f"Error connecting to database: {str(e)}")

        self.update_table_combobox.pack(side=tk.LEFT, padx=10)
        self.update_table_combobox.bind('<<ComboboxSelected>>', self.__on_update_table_selected)

        # Raw SQL update section
        raw_frame = tk.Frame(self.__current_frame)
        raw_frame.pack(pady=5, fill=tk.X, padx=10)

        raw_label = tk.Label(raw_frame, text="Or enter raw SQL update query:")
        raw_label.pack(anchor="w")

        self.raw_update_statement_entry = tk.Entry(raw_frame, width=75)
        self.raw_update_statement_entry.pack(fill=tk.X, pady=5)

        raw_button = tk.Button(raw_frame, text="Execute Raw Query", command=self.__on_raw_update_click)
        raw_button.pack(anchor="e", pady=5)

        separator = ttk.Separator(self.__current_frame, orient='horizontal')
        separator.pack(fill=tk.X, pady=10)

        # Create a frame for the dynamic form fields
        self.update_fields_container = tk.Frame(self.__current_frame)
        self.update_fields_container.pack(pady=10, fill=tk.BOTH, expand=True)

        # Scrollable canvas for fields
        self.update_canvas = tk.Canvas(self.update_fields_container)
        scrollbar = tk.Scrollbar(self.update_fields_container, orient=tk.VERTICAL, command=self.update_canvas.yview)
        self.update_fields_frame = tk.Frame(self.update_canvas)

        self.update_fields_frame.bind(
            "<Configure>",
            lambda e: self.update_canvas.configure(scrollregion=self.update_canvas.bbox("all"))
        )

        self.update_canvas.create_window((0, 0), window=self.update_fields_frame, anchor="nw")
        self.update_canvas.configure(yscrollcommand=scrollbar.set)

        self.update_canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Initial message
        self.update_placeholder = tk.Label(self.update_fields_frame,
                                           text="Please select a table to view its fields")
        self.update_placeholder.pack(pady=50)

        # WHERE condition frame
        where_frame = tk.Frame(self.__current_frame)
        where_frame.pack(pady=10, fill=tk.X, padx=10)

        where_label = tk.Label(where_frame, text="WHERE condition:")
        where_label.pack(side=tk.LEFT, padx=5)

        self.where_entry = tk.Entry(where_frame, width=60)
        self.where_entry.pack(side=tk.LEFT, padx=5, fill=tk.X, expand=True)

        # Button frame
        button_frame = tk.Frame(self.__current_frame)
        button_frame.pack(pady=10, fill=tk.X)

        submit_button = tk.Button(button_frame, text="Submit Update", command=self.__on_update_submit)
        submit_button.pack(side=tk.LEFT, padx=10)

        back_button = tk.Button(button_frame, text="Back to Menu", command=self.__on_back_to_menu_click)
        back_button.pack(side=tk.RIGHT, padx=10)

        self.__previous_frame.pack_forget()
        self.__window.title(self.__app_name + ": Update Form")
        self.__current_frame.pack(expand=True, fill=tk.BOTH)

    def __on_raw_update_click(self):
        try:
            query = self.raw_update_statement_entry.get()
            if not query:
                messagebox.showerror("Error", "Please enter a query")
                return

            self.__controller.send_query(query)
            messagebox.showinfo("Success", "Update query executed successfully!")
            self.raw_update_statement_entry.delete(0, tk.END)
        except Exception as e:
            messagebox.showerror("Error", f"Query execution failed: {str(e)}")

    def __display_search_form_frame(self):
        self.__previous_frame = self.__current_frame
        self.__current_frame = tk.Frame(self.__window)

        # raw SQL search elements
        tk.Label(self.__current_frame, text="SQL select query:").pack(anchor="center")
        self.raw_select_statement_entry = tk.Entry(self.__current_frame, width=75)
        self.raw_select_statement_entry.pack(anchor="center")
        tk.Button(self.__current_frame, text="Enter", command=self.__on_enter_query_click).pack(anchor="center")

        # create listbox and scrollbars for listbox
        hor_scrollbar = tk.Scrollbar(self.__current_frame, orient=tk.HORIZONTAL)
        vert_scrollbar = tk.Scrollbar(self.__current_frame, orient=tk.VERTICAL)
        ready_queries = self.__controller.get_ready_queries_descriptions()
        tk.Label(self.__current_frame, text="Choose a ready-made query:").pack(pady=(10, 0))
        self.__listbox = tk.Listbox(self.__current_frame, xscrollcommand=hor_scrollbar.set,
                                    yscrollcommand=vert_scrollbar.set, height=10, width=100)
        hor_scrollbar.config(command=self.__listbox.xview)
        vert_scrollbar.config(command=self.__listbox.yview)
        for query in ready_queries:
            self.__listbox.insert(tk.END, query)

        listbox_confirm_button = tk.Button(self.__current_frame, text="Confirm Selection",
                                           command=self.__get_ready_query_result)

        tk.Button(self.__current_frame, text="Back", command=self.__on_back_to_menu_click).pack(side=tk.BOTTOM)
        listbox_confirm_button.pack(side=tk.BOTTOM)

        hor_scrollbar.pack(side=tk.BOTTOM, fill=tk.X)
        self.__listbox.pack(anchor="center", side=tk.LEFT, fill=tk.BOTH)
        vert_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.__previous_frame.pack_forget()
        self.__window.title(self.__app_name + ": Select Form")
        self.__current_frame.pack(expand=True)

    def __get_ready_query_result(self):
        picked_indices = self.__listbox.curselection()
        if len(picked_indices) != 0:
            choice = self.__listbox.get(picked_indices)
            print(choice)

            # Get the query result
            result = self.__controller.get_ready_queries_results(choice)

            # Create a new window for results
            result_window = tk.Toplevel(self.__window)
            result_window.title("Query Results")
            result_window.geometry("800x600")

            # Get the actual column names for the query
            try:
                table_name = "reader"  # Currently all sample queries use the reader table - need to fix for general query
                column_info = self.__controller.get_table_columns(table_name)
                column_names = [col[0] for col in column_info]  # Extract column names

                # Ensure we have enough column names for the result
                if result and len(result) > 0:
                    # If we have more result columns than known column names, add generic ones
                    if len(result[0]) > len(column_names):
                        column_names.extend([f"Column {i + 1}" for i in range(len(column_names), len(result[0]))])
                    # If we have fewer columns in the result than column names, truncate
                    elif len(result[0]) < len(column_names):
                        column_names = column_names[:len(result[0])]
            except Exception as e:
                # Fall back to generic column names if there's an issue
                print(f"Error getting column names: {e}")
                if result and len(result) > 0:
                    column_names = [f"Column {i + 1}" for i in range(len(result[0]))]
                else:
                    column_names = []

            tree = ttk.Treeview(result_window, columns=column_names, show='headings')

            for col in column_names:
                tree.heading(col, text=col)
                tree.column(col, width=100)

            for row in result:
                tree.insert('', tk.END, values=row)

            vsb = ttk.Scrollbar(result_window, orient="vertical", command=tree.yview)
            hsb = ttk.Scrollbar(result_window, orient="horizontal", command=tree.xview)
            tree.configure(yscrollcommand=vsb.set, xscrollcommand=hsb.set)

            tree.pack(fill=tk.BOTH, expand=True)
            vsb.pack(side=tk.RIGHT, fill=tk.Y)
            hsb.pack(side=tk.BOTTOM, fill=tk.X)
        else:
            messagebox.showerror("Error", "No queries selected.")

    def __on_enter_query_click(self):
        try:
            query = self.raw_select_statement_entry.get()
            if not query:
                messagebox.showerror("Error", "Please enter a query")
                return

            result = self.__controller.send_query(query)

            if isinstance(result, list) and result:
                result_window = tk.Toplevel(self.__window)
                result_window.title("Query Results")
                result_window.geometry("800x600")

                # Try to extract the table name from the query to get column names
                import re
                table_name_match = re.search(r'FROM\s+(\w+)', query, re.IGNORECASE)

                column_names = []
                if table_name_match:
                    try:
                        table_name = table_name_match.group(1)
                        column_info = self.__controller.get_table_columns(table_name)
                        column_names = [col[0] for col in column_info]

                        # Ensure we have enough column names for the result
                        if len(result[0]) > len(column_names):
                            column_names.extend([f"Column {i + 1}" for i in range(len(column_names), len(result[0]))])
                        elif len(result[0]) < len(column_names):
                            column_names = column_names[:len(result[0])]
                    except Exception as e:
                        print(f"Error getting column names: {e}")
                        column_names = []

                # Fall back to generic column names if needed
                if not column_names and len(result[0]) > 0:
                    column_names = [f"Column {i + 1}" for i in range(len(result[0]))]

                tree = ttk.Treeview(result_window, columns=column_names, show='headings')

                for col in column_names:
                    tree.heading(col, text=col)
                    tree.column(col, width=100)

                for row in result:
                    tree.insert('', tk.END, values=row)

                vsb = ttk.Scrollbar(result_window, orient="vertical", command=tree.yview)
                hsb = ttk.Scrollbar(result_window, orient="horizontal", command=tree.xview)
                tree.configure(yscrollcommand=vsb.set, xscrollcommand=hsb.set)

                tree.pack(fill=tk.BOTH, expand=True)
                vsb.pack(side=tk.RIGHT, fill=tk.Y)
                hsb.pack(side=tk.BOTTOM, fill=tk.X)

            else:
                messagebox.showinfo("Success", "Query executed successfully!")

            self.raw_select_statement_entry.delete(0, tk.END)
        except Exception as e:
            messagebox.showerror("Error", f"Query execution failed: {str(e)}")
