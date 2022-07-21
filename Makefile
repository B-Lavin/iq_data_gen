CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall -std=c++1z
LDFLAGS=-g
#$(shell root-config --ldflags)

LDLIBS=-ldl
#$(shell root-config --libs)

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

 
all: main

main: $(OBJS)
	$(CXX) $(LDFLAGS) -o iq_data_gen $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend