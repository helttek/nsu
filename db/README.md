# Some source materials for "Databases"
## Computer store database (1 semester)
Database was first created in the libre office database client, then moved to postgresql.
## University library database app project (2 semester)
The database used is postgresql, the app itself is written in python.
### Dependencies and Build
Dependencies:
```bash
apt install python3 python3-pip
cd university_library && python3 -m venv .venv
source .venv/bin/activate
pip install psycopg future
```
Build:
```bash
cd university_library && python3 -m venv .venv
source .venv/bin/activate
export PYTHONPATH="<path_to_cloned_directory>/databases/university_library_project:$PYTHONPATH"
python src/main.py
```
