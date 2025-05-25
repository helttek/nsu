import psycopg2
from psycopg2 import Error


class Model:
    def __init__(self):
        print("Model constructor")
        self.connection = None
        self.cursor = None
        self.current_user = None
        self.current_password = None

    def connect_to_db(self, username=None, password=None):
        try:
            # Use provided credentials if available, otherwise use stored credentials
            user = username or self.current_user or "admin"
            pwd = password or self.current_password or "admin_password"

            self.connection = psycopg2.connect(
                database="university_library",
                user=user,
                password=pwd,
                host="localhost"
            )
            self.cursor = self.connection.cursor()
            self.cursor.execute("SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL SERIALIZABLE")

            print(f"Connected to database as {user}")
            return True
        except Error as e:
            print(f"Error connecting to PostgreSQL: {e}")
            return False

    def execute_query(self, query, params=None):
        try:
            if not self.connection:
                if not self.connect_to_db():
                    raise Exception("Not connected to database")

            self.cursor.execute(query, params)
            if query.strip().upper().startswith(("SELECT", "SHOW")):
                result = self.cursor.fetchall()
                self.connection.commit()
                return result
            else:
                self.connection.commit()
                return True
        except Error as e:
            print(f"Error executing query: {e}")
            if self.connection:
                self.connection.rollback()
            raise e

    def get_tables(self):
        try:
            if not self.connection:
                if not self.connect_to_db():
                    raise Exception("Not connected to database")

            self.cursor.execute("""
                                SELECT table_name
                                FROM information_schema.tables
                                WHERE table_schema = 'public'
                                """)
            result = [table[0] for table in self.cursor.fetchall()]
            self.connection.commit()
            return result
        except Error as e:
            print(f"Error fetching tables: {e}")
            if self.connection:
                self.connection.rollback()
            return []

    def get_table_columns(self, table_name):
        try:
            if not self.connection:
                if not self.connect_to_db():
                    raise Exception("Not connected to database")

            # Use parameterized query to prevent SQL injection
            self.cursor.execute("""
                                SELECT column_name, data_type, is_nullable
                                FROM information_schema.columns
                                WHERE table_name = %s
                                """, (table_name,))

            result = self.cursor.fetchall()
            self.connection.commit()
            return result
        except Error as e:
            print(f"Error fetching columns: {e}")
            if self.connection:
                self.connection.rollback()
            return []

    def authenticate_user(self, username, password):
        print(f"Authenticating user: {username}")

        if self.connection:
            self.connection.close()
            self.connection = None
            self.cursor = None

        try:
            self.connection = psycopg2.connect(
                database="university_library",
                user=username,
                password=password,
                host="localhost"
            )

            self.current_user = username
            self.current_password = password
            self.cursor = self.connection.cursor()

            self.cursor.execute("SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL SERIALIZABLE")

            print(f"Authentication successful for {username}")
            return True

        except Exception as e:
            print(f"Authentication failed: {e}")
            return False

    @staticmethod
    def get_ready_queries():
        return [
            "Получить перечень и общее число читателей для данного читального зала или абонента, либо по всей библиотеке, по признаку принадлежности к кафедре, факультету, курсу, группе. ",
            "Получить список и общее число всех читателей-задолжников, задолжников со сроком более 10 дней на данном абоненте либо по всей библиотеке, по признаку принадлежности к кафедре, факультету, курсу, группе, по категориям читателей.",
            "Получить перечень двадцати наиболее часто заказываемых книг в данном читальном зале для данного факультета, для всего вуза.",
            "Получить перечень и общее число книг, поступивших и утерянных за последний год, для данного читального зала, абонента или по всей библиотеке, по указанному автору, году выпуска, году поступления в библиотеку.",
            "Определить пункт выдачи, на которой самое большое (маленькое) число читателей, читателей-задолжников, самая большая сумма задолжности.",
            "Получить перечень и общее число книг, заказанных на межбиблиотечном абонементе за последний месяц, семестр, год.",
            "Получить количество экземпляров книги для данного читального зала или абонента, во всей библиотеке, всех изданий.",
            "Получить перечень и общее число читателей, лишенных права пользования библиотекой, сроком более двух месяцев, во всей библиотеке, по признаку принадлежности к кафедре, факультету, курсу, группе, по категориям читателей.",
            "Получить перечень и общее число новых читателей, выбывших читателей для данного читального зала или абонента за последний месяц, семестр, год, во всей библиотеке, по признаку принадлежности к кафедре, факультету, курсу, группе, по категориям читателей. ",
            "Получить перечень и общее число книг, заказанных данным читателем за последний месяц, семестр, год, список книг, которые у него на руках.",
            "Определить, есть ли данная книга в наличии на абонементах, и в каком количестве.",
            "Получить перечень читателей, у которых на руках некоторая книга и читателя, который раньше всех ее должен сдать. ",
            "Выдать полную информацию о читателе по его фамилии - группу, курс, факультет или кафедру, правонарушения, их количество, штрафы, утерянные книги и т.п.",
        ]

    def get_ready_queries_results(self, query_description):
        query_index = self.get_ready_queries().index(query_description)
        queries = {
            # 1. List of readers for a given reading room or by various criteria
            0: """SELECT *
                  FROM reader;""",

            # 2. List of debtors and their information
            1: """SELECT *
                  FROM reader;""",

            # 3. Top 20 most frequently ordered books
            2: """SELECT *
                  FROM reader;""",

            # 4. Books acquired and lost in the last year
            3: """SELECT *
                  FROM reader;""",

            # 5. Pickup points statistics
            4: """SELECT *
                  FROM reader;""",

            # 6. Books ordered via interlibrary loan
            5: """SELECT *
                  FROM reader;""",

            # 7. Number of book copies available
            6: """SELECT *
                  FROM reader;""",

            # 8. Readers banned from the library
            7: """SELECT *
                  FROM reader;""",

            # 9. New and departed readers statistics
            8: """SELECT *
                  FROM reader;""",

            # 10. Books ordered by a specific reader
            9: """SELECT *
                  FROM reader;""",

            # 11. Check book availability
            10: """SELECT *
                   FROM reader;""",

            # 12. List of readers who have a certain book and the first to return
            11: """SELECT *
                   FROM reader;""",

            # 13. Complete reader information by surname
            12: """SELECT *
                   FROM reader;"""
        }

        try:
            if query_index in queries:
                query = queries[query_index]
                # Create a list of None values matching the number of parameters in the query
                param_count = query.count('%s')
                params = [None] * param_count

                result = self.execute_query(query, params)

                return result
            else:
                return [("Query not implemented yet.",)]
        except Exception as e:
            print(f"Error executing query: {e}")
            return [("Error executing query:", str(e))]
