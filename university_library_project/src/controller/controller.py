from src.model.model import Model
from src.view.view import View


class Controller:
    def __init__(self):
        print("Initializing Controller...")
        self.model = Model()
        self.view = View(self)

    def start(self):
        self.view.show_start_frame()

    def send_query(self, query):
        self.model.execute_query(query)

    def authenticate_user(self, username, password):
        try:
            self.model.authenticate_user(username, password)
            return True
        except:
            return False

    def get_ready_queries_descriptions(self):
        return self.model.get_ready_queries()

    def get_ready_queries_results(self, query):
        return self.model.get_ready_queries_results(query)
