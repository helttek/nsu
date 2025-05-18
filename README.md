# Some source materials for "Databases" subject at NSU
## Dependencies and Build
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
