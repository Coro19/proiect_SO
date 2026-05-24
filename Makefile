CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11
SRCS = main.c district.c add.c list.c view.c remove_report.c update_threshold.c filter.c permissions.c remove_district.c
TARGET = city_manager
all: $(TARGET) monitor_reports city_hub scorer
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
monitor_reports: monitor_reports.c
	$(CC) $(CFLAGS) -o monitor_reports monitor_reports.c
city_hub: city_hub.c
	$(CC) $(CFLAGS) -o city_hub city_hub.c
scorer: scorer.c city_manager.h
	$(CC) $(CFLAGS) -o scorer scorer.c
clean:
	rm -f $(TARGET) monitor_reports city_hub scorer