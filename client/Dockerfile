FROM gcc:9.2.0

RUN apt-get update && apt-get install -y libgtest-dev && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/app

COPY . .

RUN make

CMD [ "./client_runner.o" ]