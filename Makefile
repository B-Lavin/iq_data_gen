CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall
LDFLAGS=-g
#$(shell root-config --ldflags)

LDLIBS=-ldl
#$(shell root-config --libs)

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

 
all: main

main: $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend