import tkinter as tk
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
        insert_form_button.pack(anchor="center")

        update_form_button = tk.Button(self.__current_frame, text="Update Form", command=self.__on_update_from_click)
        update_form_button.pack(anchor="center")

        search_form_button = tk.Button(self.__current_frame, text="Search Form", command=self.__on_search_from_click)
        search_form_button.pack(anchor="center")

        self.__previous_frame.pack_forget()
        self.__window.title(self.__app_name + ": Menu")
        self.__current_frame.pack(expand=True)

    def __on_insert_from_click(self):
        self.__display_insert_form_frame()

    def __display_insert_form_frame(self):
        self.__previous_frame = self.__current_frame
        self.__current_frame = tk.Frame(self.__window)

        raw_insert_statement_label = tk.Label(self.__current_frame, text="SQL insert query:")
        raw_insert_statement_label.pack(anchor="center")
        self.raw_insert_statement_entry = tk.Entry(self.__current_frame, width=75)
        self.raw_insert_statement_entry.pack(anchor="center")

        enter_query_button = tk.Button(self.__current_frame, text="Enter", command=self.__on_enter_query_click)
        enter_query_button.pack(anchor="center")

        back_to_menu_button = tk.Button(self.__current_frame, text="Back", command=self.__on_back_to_menu_click)
        back_to_menu_button.pack(anchor="center")

        self.__previous_frame.pack_forget()
        self.__window.title(self.__app_name + ": Insert Form")
        self.__current_frame.pack(expand=True)

    def __get_ready_query_result(self):
        picked_indices = self.__listbox.curselection()
        if len(picked_indices) != 0:
            choice = self.__listbox.get(picked_indices)
            print(choice)
            self.__controller.get_ready_queries_results(choice)
        else:
            messagebox.showerror("Error", "No queries selected.")

    def __on_update_from_click(self):
        self.__display_update_form_frame()

    def __display_update_form_frame(self):
        self.__previous_frame = self.__current_frame
        self.__current_frame = tk.Frame(self.__window)

        raw_update_statement_label = tk.Label(self.__current_frame, text="SQL update query:")
        raw_update_statement_label.pack(anchor="center")
        self.raw_update_statement_entry = tk.Entry(self.__current_frame, width=75)
        self.raw_update_statement_entry.pack(anchor="center")

        enter_query_button = tk.Button(self.__current_frame, text="Enter", command=self.__on_enter_query_click)
        enter_query_button.pack(anchor="center")

        back_to_menu_button = tk.Button(self.__current_frame, text="Back", command=self.__on_back_to_menu_click)
        back_to_menu_button.pack(anchor="center")

        self.__previous_frame.pack_forget()
        self.__window.title(self.__app_name + ": Update Form")
        self.__current_frame.pack(expand=True)

    def __on_search_from_click(self):
        self.__display_search_form_frame()

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

    def __on_back_to_menu_click(self):
        self.__display_menu_frame()

    def __on_enter_query_click(self):
        query = self.raw_insert_statement_entry.get()
        self.__controller.send_query(query)
