PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

OBJS = Raytracer.o Camera.o Light.o Sphere.o Ray.o Model.o ModelToWorld.o

CXX = g++
CFLAGS = -Wall -g

all:	raytracer

raytracer:	$(OBJS)
	$(CXX) -o $@ $^

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

clean:
	rm -fr raytracer $(OBJS) *.ppm
