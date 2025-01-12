default:
	c++ -O2 -o autopoem Autopoem.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
