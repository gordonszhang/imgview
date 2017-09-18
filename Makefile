CC=g++
CFLAGS=-g
ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lOpenImageIO -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS   = -L /usr/lib64/ -lglut -lGL -lGLU -lOpenImageIO -lm
  endif
endif

OBJS=\
	imgview.o\
	image.o
EXEC=imgview

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf $(EXEC)

compress:
	zip gordonz.zip *.cpp *.h Makefile README
