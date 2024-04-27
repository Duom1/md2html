SOURCE = main.c \
				 list.c
OBJS = $(SOURCE:.c=.o)
NAME = mymd2html
CFLAGS = -Wall -Wextra
LDFLAGS =

.PHONY: default def clean

default: $(NAME)
def: $(NAME)

debug: CFLAGS += -g
debug: LDLAGS += -g
debug: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(NAME) $(OBJS)
