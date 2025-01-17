/* http://www.imagemagick.org/Magick++/tutorial/Magick++_tutorial.pdf
* https://www.imagemagick.org/Magick++/
* https://imagemagick.org/Magick++/Image++.html
 */

//c++ -O2 -o autopoem Autopoem.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

 #include <ImageMagick-7/Magick++.h> //ImageMagick-7/
 #include <iostream>
#include <fstream>
#include <queue>
#include <vector>
//#include <qt6/QtCore/qfile.h>
//#include <qt6/qfile>
//#include <qt6/QtCore/QFile>
//#include <qt6/QtCore>

 using namespace Magick;
 using namespace std;


vector<string> poem;

int readPoem(){
     //ifstream f(argv[1] + ".txt");
    ifstream f("seapoemspooky.txt");

    if (!f.is_open()) {
        cerr << "read error\n";
        return -1;
    }
    string s;

    for (int i=0;getline(f, s) ;i++){
        //s = getline(f, s)
        poem.push_back(s);
        //poem[i] << s << endl;
        //poem[i] = s;
    }
    /*
    getline(f,poem[0]);
    getline(f,poem[1]);
    getline(f,poem[2]);
*/
    f.close();

    return 0;
}


int main( ssize_t argc, char ** argv)
{
    //vector<string> poem;
    
    if(readPoem()){
        return -1;
    }

    for(int i=0;i<poem.size();i++){
        std::cout<< i << ": " << poem[i] <<"\n";
    }
   

    InitializeMagick(*argv);
    //std::cout << poem.size() << std::endl;

    Image my_image( Geometry(320,220), Color("white"));
    // set the text rendering font (the color is determined by the "current" image setting)
    my_image.font("EVA-Matisse_Standard-EB");
    // draw text with different gravity position
    my_image.annotate("NorthWest gravity annotation", NorthWestGravity);
    my_image.annotate("SouthEast gravity annotation", SouthEastGravity);
   
    my_image.annotate(poem[0], CenterGravity);

    my_image.write("file_name_explicit_extension.gif");

    return 0;
}


