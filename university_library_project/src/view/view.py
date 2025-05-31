import tkinter as tk
from tkinter import messagebox
from tkinter import ttk


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

        self.username_login_entry = None
        self.password_login_entry = None
        self.insert_entries = {}
        self.update_entries = {}
        self.where_entry = None

        # Handle window close event
        self.__window.protocol("WM_DELETE_WINDOW", self.__on_window_close)

    def __on_window_close(self):
        """Handles the event when the main window is closed."""
        if messagebox.askokcancel("Quit", "Do you want to quit?"):
            self.__controller.on_window_close()  # Notify controller
            self.__window.destroy()

    def show_start_frame(self):
        self.__display_auth_frame()
        # Start the Tkinter event loop here, only once
        if not hasattr(self, '_mainloop_started') or not self._mainloop_started:
            self._mainloop_started = True
            self.__window.mainloop()

    def __display_auth_frame(self):
        if self.__current_frame:
            self.__current_frame.destroy()
        self.__current_frame = tk.Frame(self.__window)

        username_label = tk.Label(self.__current_frame, text="Username:")
        self.username_login_entry = tk.Entry(self.__current_frame)
        username_label.pack(anchor="center", pady=(100, 2))
        self.username_login_entry.pack(anchor="center", pady=2)
        # self.username_login_entry.insert(0, "admin") # <--- REMOVE OR COMMENT OUT THIS LINE

        password_label = tk.Label(self.__current_frame, text="Password:")
        self.password_login_entry = tk.Entry(self.__current_frame, show="*")
        password_label.pack(anchor="center", pady=2)
        self.password_login_entry.pack(anchor="center", pady=2)
        # self.password_login_entry.insert(0, "admin_password") # <--- REMOVE OR COMMENT OUT THIS LINE

        login_button = tk.Button(self.__current_frame, text="Login", command=self.__on_login_click)
        login_button.pack(anchor="center", pady=10)

        self.__current_frame.pack(expand=True, fill=tk.BOTH)
        # Removed self.__window.mainloop() from here

    def __on_login_click(self):
        username = self.username_login_entry.get()
        password = self.password_login_entry.get()
        if self.__controller.authenticate_user(username, password):
            self.__display_menu_frame()
        else:
            messagebox.showerror("Authentication Error", "Invalid username or password.")

    def __clear_current_frame(self):
        if self.__current_frame:
            self.__current_frame.destroy()
        self.__current_frame = tk.Frame(self.__window)
        self.__current_frame.pack(expand=True, fill=tk.BOTH)

    def __display_menu_frame(self):
        self.__clear_current_frame()
        self.__window.title(self.__app_name + ": Menu")

        insert_form_button = tk.Button(self.__current_frame, text="Insert Form", command=self.__on_insert_from_click,
                                       width=20, height=2)
        insert_form_button.pack(anchor="center", pady=10)

        update_form_button = tk.Button(self.__current_frame, text="Update Form", command=self.__on_update_from_click,
                                       width=20, height=2)
        update_form_button.pack(anchor="center", pady=10)

        search_form_button = tk.Button(self.__current_frame, text="Search Form (Queries)",
                                       command=self.__on_search_from_click, width=20, height=2)
        search_form_button.pack(anchor="center", pady=10)

        # logout_button = tk.Button(self.__current_frame, text="Logout", command=self.__on_logout_click, width=20,
        #                           height=2)
        # logout_button.pack(anchor="s", pady=20)  # Внизу

    # def __on_logout_click(self):
    #     # Здесь можно добавить логику очистки сессии в контроллере/модели, если нужно
    #     print("Logging out...")
    #     self.__display_auth_frame()

    def __on_insert_from_click(self):
        self.__display_insert_form_frame()

    def __on_insert_table_selected(self, event):
        for widget in self.insert_fields_frame.winfo_children():
            widget.destroy()
        self.insert_placeholder.pack_forget()  # Убираем плейсхолдер

        table_name = self.insert_table_var.get()
        if not table_name:
            self.insert_placeholder.pack(pady=50)
            return

        try:
            columns = self.__controller.get_table_columns(table_name)
            if not columns:
                tk.Label(self.insert_fields_frame,
                         text=f"No columns found or error for table '{table_name}'. Check console.").pack(pady=20)
                return

            header_frame = tk.Frame(self.insert_fields_frame)
            header_frame.pack(fill=tk.X, padx=10, pady=5)
            tk.Label(header_frame, text="Column", font=('Arial', 10, 'bold'), width=20, anchor="w").grid(row=0,
                                                                                                         column=0,
                                                                                                         padx=5,
                                                                                                         sticky="w")
            tk.Label(header_frame, text="Type", font=('Arial', 10, 'bold'), width=15, anchor="w").grid(row=0, column=1,
                                                                                                       padx=5,
                                                                                                       sticky="w")
            tk.Label(header_frame, text="Nullable", font=('Arial', 10, 'bold'), width=10, anchor="w").grid(row=0,
                                                                                                           column=2,
                                                                                                           padx=5,
                                                                                                           sticky="w")
            tk.Label(header_frame, text="Value", font=('Arial', 10, 'bold'), width=30, anchor="w").grid(row=0, column=3,
                                                                                                        padx=5,
                                                                                                        sticky="w")

            self.insert_entries = {}
            for i, (column, data_type, is_nullable) in enumerate(columns):
                row_frame = tk.Frame(self.insert_fields_frame)
                row_frame.pack(fill=tk.X, padx=10, pady=2)
                tk.Label(row_frame, text=column, width=20, anchor="w").grid(row=0, column=0, padx=5, sticky="w")
                tk.Label(row_frame, text=data_type, width=15, anchor="w").grid(row=0, column=1, padx=5, sticky="w")
                tk.Label(row_frame, text=is_nullable, width=10, anchor="w").grid(row=0, column=2, padx=5, sticky="w")
                entry = tk.Entry(row_frame, width=33)
                entry.grid(row=0, column=3, padx=5, sticky="ew")
                self.insert_entries[column] = entry
        except Exception as e:
            messagebox.showerror("Error", f"Could not load table columns: {str(e)}")
            tk.Label(self.insert_fields_frame, text=f"Error loading columns: {str(e)}").pack(pady=20)

    def __on_insert_submit(self):
        table_name = self.insert_table_var.get()
        if not table_name:
            messagebox.showerror("Error", "Please select a table first.")
            return

        columns_and_values = {}
        try:
            # Получаем информацию о столбцах, чтобы знать их типы и nullable status
            table_columns_info_raw = self.__controller.get_table_columns(table_name)
            table_columns_info = {col[0]: {'type': col[1], 'nullable': col[2]} for col in table_columns_info_raw}

            for column, entry_widget in self.insert_entries.items():
                value_str = entry_widget.get().strip()
                col_info = table_columns_info.get(column)

                if value_str:  # Если что-то введено
                    # Попытка преобразования типов (очень базовая)
                    if "int" in col_info['type'] or "serial" in col_info['type']:
                        try:
                            columns_and_values[column] = int(value_str)
                        except ValueError:
                            # Если это serial (автоинкремент) и поле пустое, оно само заполнится
                            if "serial" in col_info['type'] and not value_str:
                                continue  # Пропускаем, БД сама сгенерирует
                            messagebox.showerror("Type Error", f"Invalid integer value for {column}: {value_str}")
                            return
                    elif "numeric" in col_info['type'] or "money" in col_info['type'] or "double" in col_info['type']:
                        try:
                            columns_and_values[column] = float(
                                value_str.replace(',', '.'))  # Замена запятой на точку для float
                        except ValueError:
                            messagebox.showerror("Type Error", f"Invalid numeric value for {column}: {value_str}")
                            return
                    elif "date" == col_info['type']:
                        # Здесь можно добавить валидацию формата даты, если нужно
                        columns_and_values[column] = value_str
                    elif "boolean" == col_info['type']:
                        if value_str.lower() in ['true', '1', 'yes', 'y']:
                            columns_and_values[column] = True
                        elif value_str.lower() in ['false', '0', 'no', 'n']:
                            columns_and_values[column] = False
                        else:
                            messagebox.showerror("Type Error",
                                                 f"Invalid boolean value for {column}: {value_str} (use true/false)")
                            return
                    else:  # Для varchar, char, text и т.д.
                        columns_and_values[column] = value_str
                elif col_info['nullable'] == 'NO' and "serial" not in col_info[
                    'type']:  # Если поле не пустое и не автоинкремент
                    messagebox.showerror("Validation Error", f"Field '{column}' cannot be empty.")
                    return
                # Если поле пустое и nullable, то ничего не добавляем (будет NULL)

            if not columns_and_values:
                messagebox.showwarning("No Data", "Please fill in at least one field to insert.")
                return

            self.__controller.insert_data(table_name, columns_and_values)
            messagebox.showinfo("Success", "Record inserted successfully!")
            for entry_widget in self.insert_entries.values():
                entry_widget.delete(0, tk.END)
        except Exception as e:
            messagebox.showerror("Insertion Error", f"Failed to insert record: {str(e)}")

    def __on_update_from_click(self):
        self.__display_update_form_frame()

    def __on_update_table_selected(self, event):
        for widget in self.update_fields_frame.winfo_children():
            widget.destroy()
        self.update_placeholder.pack_forget()

        table_name = self.update_table_var.get()
        if not table_name:
            self.update_placeholder.pack(pady=50)
            return
        try:
            columns = self.__controller.get_table_columns(table_name)
            if not columns:
                tk.Label(self.update_fields_frame, text=f"No columns found for table '{table_name}'.").pack(pady=20)
                return

            header_frame = tk.Frame(self.update_fields_frame)
            header_frame.pack(fill=tk.X, padx=10, pady=5)
            tk.Label(header_frame, text="Column", font=('Arial', 10, 'bold'), width=20, anchor="w").grid(row=0,
                                                                                                         column=0,
                                                                                                         padx=5,
                                                                                                         sticky="w")
            tk.Label(header_frame, text="Type", font=('Arial', 10, 'bold'), width=15, anchor="w").grid(row=0, column=1,
                                                                                                       padx=5,
                                                                                                       sticky="w")
            tk.Label(header_frame, text="New Value", font=('Arial', 10, 'bold'), width=30, anchor="w").grid(row=0,
                                                                                                            column=2,
                                                                                                            padx=5,
                                                                                                            sticky="w")

            self.update_entries = {}
            for i, (column, data_type, is_nullable) in enumerate(columns):
                row_frame = tk.Frame(self.update_fields_frame)
                row_frame.pack(fill=tk.X, padx=10, pady=2)
                tk.Label(row_frame, text=column, width=20, anchor="w").grid(row=0, column=0, padx=5, sticky="w")
                tk.Label(row_frame, text=data_type, width=15, anchor="w").grid(row=0, column=1, padx=5, sticky="w")
                entry = tk.Entry(row_frame, width=33)
                entry.grid(row=0, column=2, padx=5, sticky="ew")
                self.update_entries[column] = entry
        except Exception as e:
            messagebox.showerror("Error", f"Could not load table columns: {str(e)}")
            tk.Label(self.update_fields_frame, text=f"Error: {str(e)}").pack(pady=20)

    def __on_update_submit(self):
        table_name = self.update_table_var.get()
        if not table_name:
            messagebox.showerror("Error", "Please select a table first.")
            return

        where_clause_template = self.where_entry.get().strip()  # Например, "id = %s AND name ILIKE %s"
        if not where_clause_template:
            messagebox.showerror("Error", "WHERE clause template is required for update operations.")
            return

        # Запросим значения для плейсхолдеров в WHERE
        num_where_params = where_clause_template.count('%s')
        where_params_values = []
        if num_where_params > 0:
            # Простой диалог для ввода параметров WHERE. Для сложного UI нужен другой подход.
            params_str = tk.simpledialog.askstring("WHERE Parameters",
                                                   f"Enter {num_where_params} value(s) for WHERE clause, separated by commas:",
                                                   parent=self.__window)
            if params_str is None:  # Пользователь нажал Cancel
                return
            where_params_values = [p.strip() for p in params_str.split(',')]
            if len(where_params_values) != num_where_params:
                messagebox.showerror("Error",
                                     f"Expected {num_where_params} parameters for WHERE, got {len(where_params_values)}.")
                return

        columns_to_update = {}
        table_columns_info_raw = self.__controller.get_table_columns(table_name)
        table_columns_info = {col[0]: {'type': col[1], 'nullable': col[2]} for col in table_columns_info_raw}

        for column, entry_widget in self.update_entries.items():
            value_str = entry_widget.get().strip()
            if value_str:  # Обновляем только если введено новое значение
                col_info = table_columns_info.get(column)
                # Попытка преобразования типов (аналогично insert)
                if "int" in col_info['type']:
                    columns_to_update[column] = int(value_str)
                elif "numeric" in col_info['type'] or "money" in col_info['type']:
                    columns_to_update[column] = float(value_str.replace(',', '.'))
                elif "boolean" == col_info['type']:
                    columns_to_update[column] = value_str.lower() in ['true', '1', 'yes', 'y']
                else:
                    columns_to_update[column] = value_str

        if not columns_to_update:
            messagebox.showwarning("No Data", "Please fill in at least one field to update.")
            return

        try:
            # where_params_values должны быть в правильном порядке для %s
            self.__controller.update_data(table_name, columns_to_update, where_clause_template,
                                          tuple(where_params_values))
            messagebox.showinfo("Success", "Record(s) updated successfully!")
            for entry_widget in self.update_entries.values(): entry_widget.delete(0, tk.END)
            self.where_entry.delete(0, tk.END)
        except Exception as e:
            messagebox.showerror("Update Error", f"Failed to update record(s): {str(e)}")

    def __on_search_from_click(self):
        self.__display_search_form_frame()

    def __on_back_to_menu_click(self):
        self.__display_menu_frame()

    def __display_form_frame_template(self, title_suffix, on_table_selected_callback, on_submit_callback,
                                      include_where_clause=False):
        self.__clear_current_frame()
        self.__window.title(self.__app_name + title_suffix)

        # Frame for table selection and raw query
        top_controls_frame = tk.Frame(self.__current_frame)
        top_controls_frame.pack(pady=10, padx=10, fill=tk.X)

        table_frame = tk.Frame(top_controls_frame)
        table_frame.pack(pady=5, fill=tk.X)
        tk.Label(table_frame, text="Select table:").pack(side=tk.LEFT, padx=(0, 5))

        table_var = tk.StringVar()
        table_combobox = ttk.Combobox(table_frame, textvariable=table_var, width=30, state="readonly")
        try:
            tables = self.__controller.get_tables()
            table_combobox['values'] = tables
        except Exception as e:
            messagebox.showerror("Database Error", f"Error fetching tables: {str(e)}")
        table_combobox.pack(side=tk.LEFT)
        table_combobox.bind('<<ComboboxSelected>>', on_table_selected_callback)

        # Raw Query Section
        raw_frame = tk.Frame(top_controls_frame)
        raw_frame.pack(pady=5, fill=tk.X)
        raw_label_text = "Or enter raw SQL query (INSERT/UPDATE/DELETE):" if "Insert" in title_suffix or "Update" in title_suffix else "Or enter raw SQL query:"
        tk.Label(raw_frame, text=raw_label_text).pack(anchor="w")

        raw_statement_entry = tk.Entry(raw_frame, width=75)
        raw_statement_entry.pack(fill=tk.X, pady=(0, 5))

        raw_button_command = self.__on_raw_non_select_click if "Insert" in title_suffix or "Update" in title_suffix else self.__on_raw_select_click
        if "Insert" in title_suffix:
            self.raw_insert_statement_entry = raw_statement_entry
        elif "Update" in title_suffix:
            self.raw_update_statement_entry = raw_statement_entry
        else:
            self.raw_select_statement_entry = raw_statement_entry  # For search form

        tk.Button(raw_frame, text="Execute Raw Query",
                  command=lambda: raw_button_command(raw_statement_entry.get())).pack(anchor="e")

        ttk.Separator(self.__current_frame, orient='horizontal').pack(fill=tk.X, pady=10, padx=10)

        # WHERE clause for UPDATE
        if include_where_clause:
            where_frame = tk.Frame(self.__current_frame)
            where_frame.pack(pady=(5, 10), padx=10, fill=tk.X)
            tk.Label(where_frame, text="WHERE condition (e.g., id = %s):").pack(side=tk.LEFT, padx=(0, 5))
            self.where_entry = tk.Entry(where_frame, width=60)
            self.where_entry.pack(side=tk.LEFT, fill=tk.X, expand=True)

        # Fields container with scrollbar
        fields_container_outer = tk.Frame(self.__current_frame)
        fields_container_outer.pack(pady=10, padx=10, fill=tk.BOTH, expand=True)

        canvas = tk.Canvas(fields_container_outer)
        scrollbar = tk.Scrollbar(fields_container_outer, orient=tk.VERTICAL, command=canvas.yview)
        fields_frame_inner = tk.Frame(canvas)
        fields_frame_inner.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))
        canvas.create_window((0, 0), window=fields_frame_inner, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)

        canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        placeholder_text = "Please select a table to view its fields"
        placeholder_label = tk.Label(fields_frame_inner, text=placeholder_text)
        placeholder_label.pack(pady=50)

        # Buttons at the bottom
        button_frame = tk.Frame(self.__current_frame)
        button_frame.pack(pady=10, padx=10, fill=tk.X, side=tk.BOTTOM)
        tk.Button(button_frame, text="Submit", command=on_submit_callback).pack(side=tk.LEFT)
        tk.Button(button_frame, text="Back to Menu", command=self.__on_back_to_menu_click).pack(side=tk.RIGHT)

        # Assign dynamic parts to instance variables if they need to be accessed later
        if "Insert" in title_suffix:
            self.insert_table_var = table_var
            self.insert_table_combobox = table_combobox
            self.insert_fields_frame = fields_frame_inner
            self.insert_placeholder = placeholder_label
        elif "Update" in title_suffix:
            self.update_table_var = table_var
            self.update_table_combobox = table_combobox
            self.update_fields_frame = fields_frame_inner
            self.update_placeholder = placeholder_label

        return fields_frame_inner  # Return the frame where dynamic fields will be added

    def __display_insert_form_frame(self):
        self.__display_form_frame_template(
            title_suffix=": Insert Form",
            on_table_selected_callback=self.__on_insert_table_selected,
            on_submit_callback=self.__on_insert_submit
        )

    def __display_update_form_frame(self):
        self.__display_form_frame_template(
            title_suffix=": Update Form",
            on_table_selected_callback=self.__on_update_table_selected,
            on_submit_callback=self.__on_update_submit,
            include_where_clause=True  # Для формы обновления добавляем поле WHERE
        )

    def __on_raw_non_select_click(self, query):  # Для INSERT, UPDATE, DELETE
        if not query:
            messagebox.showerror("Error", "Please enter a query.")
            return
        try:
            # Для не-SELECT запросов, execute_query вернет (True, None) при успехе
            success, _ = self.__controller.send_query(query)  # Параметры не передаем для сырого запроса здесь
            if success:
                messagebox.showinfo("Success", "Raw query executed successfully!")
                # Очистка поля ввода (если нужно, нужно передать сам entry widget)
            else:  # Этого не должно быть, если нет исключения
                messagebox.showwarning("Result", "Query executed, but returned False (check logs).")
        except Exception as e:
            messagebox.showerror("Query Error", f"Raw query execution failed: {str(e)}")

    def __on_raw_select_click(self, query):  # Для SELECT
        if not query:
            messagebox.showerror("Error", "Please enter a query.")
            return
        try:
            # execute_query теперь возвращает (results, column_names)
            results, column_names = self.__controller.send_query(query)

            if results is None and column_names is None:  # Могло быть исключение внутри send_query
                messagebox.showerror("Query Error", "Failed to execute query. Check console.")
                return

            if isinstance(results, list):
                if not results:  # Пустой результат SELECT
                    messagebox.showinfo("Query Results", "Query executed successfully, but no records were returned.")
                else:
                    self.__display_results_in_treeview(results, column_names, "Raw Query Results")
            else:  # Если вернулось не (list, list), а что-то другое (например, True для не-SELECT по ошибке)
                messagebox.showinfo("Success", "Query executed (non-SELECT or unexpected result type). Check console.")
            # Очистка поля ввода (если нужно)
            if hasattr(self, 'raw_select_statement_entry') and self.raw_select_statement_entry:
                self.raw_select_statement_entry.delete(0, tk.END)
        except Exception as e:
            messagebox.showerror("Query Error", f"Raw query execution failed: {str(e)}")

    def __display_search_form_frame(self):
        self.__clear_current_frame()
        self.__window.title(self.__app_name + ": Search/Select Form")

        # Raw SELECT query input
        raw_query_frame = tk.Frame(self.__current_frame)
        raw_query_frame.pack(pady=10, padx=10, fill=tk.X)
        tk.Label(raw_query_frame, text="Enter SQL SELECT query:").pack(side=tk.LEFT)
        self.raw_select_statement_entry = tk.Entry(raw_query_frame, width=60)
        self.raw_select_statement_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
        tk.Button(raw_query_frame, text="Execute",
                  command=lambda: self.__on_raw_select_click(self.raw_select_statement_entry.get())).pack(side=tk.LEFT)

        ttk.Separator(self.__current_frame, orient='horizontal').pack(fill=tk.X, pady=10, padx=10)

        # Ready-made queries Listbox
        listbox_frame = tk.Frame(self.__current_frame)
        listbox_frame.pack(pady=10, padx=10, fill=tk.BOTH, expand=True)
        tk.Label(listbox_frame, text="Or choose a ready-made query:").pack(anchor="w")

        listbox_inner_frame = tk.Frame(listbox_frame)
        listbox_inner_frame.pack(fill=tk.BOTH, expand=True)

        hor_scrollbar = tk.Scrollbar(listbox_inner_frame, orient=tk.HORIZONTAL)
        vert_scrollbar = tk.Scrollbar(listbox_inner_frame, orient=tk.VERTICAL)

        self.__listbox = tk.Listbox(listbox_inner_frame, xscrollcommand=hor_scrollbar.set,
                                    yscrollcommand=vert_scrollbar.set, height=10,
                                    exportselection=False)  # exportselection=False

        hor_scrollbar.config(command=self.__listbox.xview)
        hor_scrollbar.pack(side=tk.BOTTOM, fill=tk.X)
        vert_scrollbar.config(command=self.__listbox.yview)
        vert_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.__listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        try:
            ready_queries = self.__controller.get_ready_queries_descriptions()
            for query_desc in ready_queries:
                self.__listbox.insert(tk.END, query_desc)
        except Exception as e:
            messagebox.showerror("Error", f"Could not load ready-made queries: {str(e)}")
            self.__listbox.insert(tk.END, "Error loading queries.")

        # Buttons at the bottom
        button_frame = tk.Frame(self.__current_frame)
        button_frame.pack(pady=10, padx=10, fill=tk.X, side=tk.BOTTOM)
        tk.Button(button_frame, text="Execute Selected Ready Query",
                  command=self.__on_get_ready_query_result_click).pack(side=tk.LEFT)
        tk.Button(button_frame, text="Back to Menu", command=self.__on_back_to_menu_click).pack(side=tk.RIGHT)

    def __show_parameter_dialog(self, param_config_list, query_description):
        dialog = tk.Toplevel(self.__window)
        dialog.title(f"Parameters for: {query_description[:50]}...")
        dialog.geometry("450x" + str(100 + len(param_config_list) * 30))
        dialog.transient(self.__window)  # Делает диалог модальным относительно главного окна
        dialog.grab_set()  # Перехватывает все события, пока диалог открыт
        dialog.resizable(False, False)

        entries = {}
        for pc in param_config_list:
            frame = tk.Frame(dialog)
            frame.pack(fill=tk.X, padx=10, pady=5)
            label_text = pc['prompt']
            if pc.get('optional'):
                label_text += " (optional)"
            tk.Label(frame, text=label_text, width=35, anchor="w").pack(side=tk.LEFT)
            entry = tk.Entry(frame, width=20)
            entry.pack(side=tk.LEFT, fill=tk.X, expand=True)
            if pc.get('default'):
                entry.insert(0, str(pc['default']))
            entries[pc['name']] = (entry, pc.get('type', 'str'), pc.get('optional', False), pc.get('is_pattern', False))

        user_params = {}  # Словарь для хранения введенных параметров

        def on_ok():
            nonlocal user_params
            try:
                for name, (entry_widget, param_type, is_optional, is_pattern) in entries.items():
                    value_str = entry_widget.get().strip()
                    if not value_str:
                        if is_optional:
                            user_params[name] = None  # Передаем None для опциональных пустых полей
                            continue
                        else:  # Если не опционально и пусто
                            # Посмотрим, есть ли значение по-умолчанию, которое не было вставлено в Entry, но определено
                            # В текущей логике default уже вставлен, так что это для обязательных без default
                            default_val_for_param = next(
                                (item.get('default') for item in param_config_list if item['name'] == name), None)
                            if default_val_for_param is not None:
                                user_params[
                                    name] = default_val_for_param  # Используем default если пусто и не опционально
                                continue
                            else:
                                messagebox.showerror("Input Error", f"Parameter '{name}' is required.", parent=dialog)
                                user_params = None  # Сигнал ошибки
                                return

                    if param_type == 'int':
                        user_params[name] = int(value_str)
                    elif param_type == 'float':
                        user_params[name] = float(value_str.replace(',', '.'))
                    # is_pattern будет обработан в модели при формировании %value%
                    else:  # str
                        user_params[name] = value_str
                dialog.destroy()
            except ValueError as ve:
                messagebox.showerror("Input Error", f"Invalid value for a parameter: {ve}", parent=dialog)
                user_params = None  # Сигнал ошибки
                # Не закрываем диалог, даем исправить
            except Exception as ex:
                messagebox.showerror("Error", f"An unexpected error occurred: {ex}", parent=dialog)
                user_params = None
                dialog.destroy()

        def on_cancel():
            nonlocal user_params
            user_params = None  # Сигнал отмены
            dialog.destroy()

        button_frame = tk.Frame(dialog)
        button_frame.pack(fill=tk.X, pady=10)
        tk.Button(button_frame, text="OK", command=on_ok, width=10).pack(side=tk.LEFT, padx=20)
        tk.Button(button_frame, text="Cancel", command=on_cancel, width=10).pack(side=tk.RIGHT, padx=20)

        dialog.protocol("WM_DELETE_WINDOW", on_cancel)  # Обработка закрытия окна крестиком
        dialog.wait_window()  # Ожидаем закрытия диалога (делает его модальным)
        return user_params

    def __on_get_ready_query_result_click(self):
        picked_indices = self.__listbox.curselection()
        if not picked_indices:
            messagebox.showwarning("No Selection", "Please select a query from the list.")
            return

        query_description = self.__listbox.get(picked_indices[0])
        print(f"Selected ready query: {query_description}")

        param_config = self.__controller.get_query_params_config(query_description)
        user_entered_params = {}

        if param_config:
            user_entered_params = self.__show_parameter_dialog(param_config, query_description)
            if user_entered_params is None:  # Диалог был отменен или ошибка ввода
                print("Parameter input cancelled or failed.")
                return

        try:
            # get_ready_queries_results теперь возвращает (results, column_names)
            results, column_names = self.__controller.get_ready_queries_results(query_description, user_entered_params)

            if results is None and column_names is None:  # Ошибка при выполнении
                messagebox.showerror("Query Error", "Failed to execute ready query. Check console.")
                return

            if isinstance(results, list):
                if not results:
                    messagebox.showinfo("Query Results", "Query executed successfully, no records returned.")
                else:
                    self.__display_results_in_treeview(results, column_names, f"Results: {query_description[:60]}")
            else:  # Неожиданный результат
                messagebox.showwarning("Query Warning",
                                       "Query executed but returned an unexpected result type. Check console.")

        except Exception as e:
            messagebox.showerror("Execution Error", f"Error executing ready query '{query_description}': {str(e)}")
            import traceback
            traceback.print_exc()

    def __display_results_in_treeview(self, results, column_names, window_title):
        if not isinstance(results, list):
            messagebox.showerror("Display Error", "Cannot display results: Invalid data format.")
            return

        result_window = tk.Toplevel(self.__window)
        result_window.title(window_title)
        result_window.geometry("900x600")

        if not column_names and results:  # Если имена столбцов не пришли, но есть данные
            if isinstance(results[0], dict):
                column_names = list(results[0].keys())
            else:  # Просто нумеруем, если это список кортежей/списков без имен
                column_names = [f"Column {i + 1}" for i in range(len(results[0]))]

        if not column_names and not results:  # Нет ни данных, ни имен
            tk.Label(result_window, text="No data or column names to display.").pack(padx=20, pady=20)
            return
        if not results:  # Есть имена, но нет данных
            tk.Label(result_window, text="Query returned no data.").pack(padx=20, pady=20)
            # Можно отобразить пустую таблицу с заголовками
            # return # или продолжить, чтобы показать пустую таблицу

        tree_frame = tk.Frame(result_window)
        tree_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        tree = ttk.Treeview(tree_frame, columns=column_names, show='headings')

        for col in column_names:
            tree.heading(col, text=col)
            tree.column(col, width=120, minwidth=50, anchor=tk.W)  # tk.W для выравнивания по левому краю

        for row_data in results:
            if isinstance(row_data, dict):
                values = [row_data.get(col, "") for col in column_names]
            elif isinstance(row_data, (tuple, list)):
                values = row_data
            else:  # Пропускаем непонятные строки
                continue
            tree.insert('', tk.END, values=values)

        vsb = ttk.Scrollbar(tree_frame, orient="vertical", command=tree.yview)
        hsb = ttk.Scrollbar(tree_frame, orient="horizontal", command=tree.xview)
        tree.configure(yscrollcommand=vsb.set, xscrollcommand=hsb.set)

        vsb.pack(side=tk.RIGHT, fill=tk.Y)
        hsb.pack(side=tk.BOTTOM, fill=tk.X)
        tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
