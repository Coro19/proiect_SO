CC = gcc
CFLAGS = -Wall -Wextra
SRCS = main.c district.c add.c list.c view.c remove_report.c update_threshold.c filter.c permissions.c
TARGET = city_manager

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)