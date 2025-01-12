/* http://www.imagemagick.org/Magick++/tutorial/Magick++_tutorial.pdf
* https://www.imagemagick.org/Magick++/
 */

//c++ -O2 -o autopoem Autopoem.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

 #include <ImageMagick-7/Magick++.h> //ImageMagick-7/
 #include <iostream>
 using namespace Magick;


int main( ssize_t argc, char ** argv)
{
    InitializeMagick(*argv);
    std::cout << "Hello World" << std::endl;

    Image my_image( Geometry(320,220), Color("white"));
    // set the text rendering font (the color is determined by the "current" image setting)
    my_image.font("EVA-Matisse_Standard-EB");
    // draw text with different gravity position
    my_image.annotate("NorthWest gravity annotation", NorthWestGravity);
    my_image.annotate("SouthEast gravity annotation", SouthEastGravity);
    my_image.annotate("Center gravity annotation", CenterGravity);

    my_image.write("file_name_explicit_extension.gif");

    return 0;
}
