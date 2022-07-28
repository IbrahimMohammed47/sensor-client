FROM gcc:9.2.0

RUN apt-get update && apt-get install -y libgtest-dev && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/app

COPY . .

ENV SENSOR_PORT=12345

RUN make

EXPOSE 12345

CMD [ "./sensor_runner.o" ]