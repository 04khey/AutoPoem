/* http://www.imagemagick.org/Magick++/tutorial/Magick++_tutorial.pdf
* https://www.imagemagick.org/Magick++/
* https://imagemagick.org/Magick++/Image++.html
 */

//c++ -O2 -o autopoem Autopoem.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

#include <ImageMagick-7/Magick++.h> 
#include <iostream>
#include <fstream>
//#include <queue>
#include <vector>


 using namespace Magick;
 using namespace std;


vector<string> poem;

int readPoem(){
     //ifstream f(argv[1] + ".txt");
    ifstream f("seapoemspooky.txt"); //debug

    if (!f.is_open()) {
        cerr << "read error\n";
        return -1;
    }
    string s;

    for (int i=0;getline(f, s) ;i++){

        poem.push_back(s);
        //poem[i] = s;
    }

    f.close();

    return 0;
}


int main( ssize_t argc, char ** argv)
{
    
    if(readPoem()){
        return -1;
    }

    for(int i=0;i<poem.size();i++){
        std::cout<< i << ": " << poem[i] <<"\n";
    }
   

    InitializeMagick(*argv);


    Image my_image( Geometry(1080,1080), Color("white"));
    // set the text rendering font (the color is determined by the "current" image setting)
    my_image.font("EVA-Matisse_Standard-EB");
    my_image.fontPointsize(100);
    // draw text with different gravity position
    my_image.annotate("NorthWest gravity annotation", NorthWestGravity);
    my_image.annotate("SouthEast gravity annotation", SouthEastGravity);
    
    //https://imagemagick.org/Magick++/Geometry.html

    Geometry test("1080x200+0+100");

    //my_image.annotate(poem[0], CenterGravity);
    my_image.annotate(poem[0], test, CenterGravity);

    my_image.write("out.png");

    return 0;
}


