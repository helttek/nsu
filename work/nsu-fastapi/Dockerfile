FROM python:3.9-slim
WORKDIR /app
COPY ./src/nsu_fastapi.py ./main.py
RUN pip install fastapi uvicorn
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "9090"]
