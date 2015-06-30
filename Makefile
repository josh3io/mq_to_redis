
OBJECTS = redis_writer_app.o redis_writer.o mq_reader.o

EXEC = redis_writer_app

FLAGS = -Wall -I/usr/local/include -L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -lredis3m -lboost_system -std=c++11 -I/opt/mqm/inc -L/opt/mqm/lib64 -Wl,-rpath=/opt/mqm/lib64 -Wl,-rpath=/usr/lib64 -limqc23gl_r -limqb23gl_r -lmqic_r

CC = g++

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(FLAGS) -o $(EXEC)

redis_writer_app.o: redis_writer_app.cpp
	$(CC) $(FLAGS) -c redis_writer_app.cpp

redis_writer.o: redis_writer.cpp
	$(CC) $(FLAGS) -c redis_writer.cpp

mq_reader.o: mq_reader.cpp
	$(CC) $(FLAGS) -c mq_reader.cpp
