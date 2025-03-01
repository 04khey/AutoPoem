.PHONY: test 
.PHONY: parse 

default:
	c++ -O2 -o autopoem Autopoem.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
test:
	c++ -O2 -o test Testing.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
parse:
	c++ -O2 -o parse parsePoemInput.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`