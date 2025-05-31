# <filename>controller.py</filename>
from src.model.model import Model
from src.view.view import View


class Controller:
    def __init__(self):
        print("Initializing Controller...")
        self.model = Model()
        self.view = View(self)

    def start(self):
        self.view.show_start_frame()

    def send_query(self, query, params=None):
        return self.model.execute_query(query, params)

    def authenticate_user(self, username, password):
        try:
            return self.model.authenticate_user(username, password)
        except Exception as e:
            print(f"Controller: Authentication error - {e}")
            return False

    def get_ready_queries_descriptions(self):
        return self.model.get_ready_queries()

    def get_ready_queries_results(self, query_description, user_params=None):
        return self.model.get_ready_queries_results(query_description, user_params)

    def get_query_params_config(self, query_description):
        return self.model.get_query_params_config_by_description(query_description)

    def get_tables(self):
        return self.model.get_tables()

    def get_table_columns(self, table_name):
        return self.model.get_table_columns(table_name)

    def insert_data(self, table_name, columns_and_values):
        return self.model.insert_record(table_name, columns_and_values)

    def update_data(self, table_name, columns_to_update, where_clause_template, where_params):
        return self.model.update_record(table_name, columns_to_update, where_clause_template, where_params)

    def on_window_close(self):
        print("Controller: Window close requested. Closing database connection.")
        self.model.close_connection()
