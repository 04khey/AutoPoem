.PHONY: test 

default:
	c++ -O2 -o autopoem Autopoemv2.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
test:
	c++ -O2 -o test Testing.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`