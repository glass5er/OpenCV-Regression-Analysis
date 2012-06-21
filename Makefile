TARGET = bin_regression

GITREV := $(shell git rev-list HEAD | wc -l)
GITSTATUS := $(shell git st 2>&1 | grep "Changes" | head -n 1 | sed -e s/.*Changes.*/M/)

SRC = \
	main.cpp \

INCLUDE = \
	-I./ \
	`pkg-config opencv --cflags`


LIB = 	\


LIBDEF=	-lpthread \
	-llapack \
	`pkg-config opencv --libs`

DEFINE = \
	-D_LINUX_ \
	-DREVISION=\"${GITREV}${GITSTATUS}\" \
#	-DREVISION=\"NONE\" \

CC = g++ -g

CFLAGS = \
	-O1 \
	-msse2\
	-g \
	-Wall \
	-funroll-all-loops\
	-ftree-vectorize\
#	-fopenmp \



OBJ = $(patsubst %.cpp,%.o,$(filter %.cpp,$(SRC)))

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(DEFINE) $(INCLUDE) -c $< -o $@

all: $(TARGET)

libraries:
	@ for d in $(dir $(LIB)); do \
		make -C $$d; \
	done

$(TARGET): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB) $(LIBDEF)

clean:
	rm -f $(TARGET) $(OBJ)

clobber: clean
	@ for d in $(dir $(LIB)); do \
		make -C $$d clean; \
	done



