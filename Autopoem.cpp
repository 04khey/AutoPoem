/* http://www.imagemagick.org/Magick++/tutorial/Magick++_tutorial.pdf
* https://www.imagemagick.org/Magick++/
* https://imagemagick.org/Magick++/Image++.html
 */

//c++ -O2 -o autopoem Autopoem.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

#include <ImageMagick-7/Magick++.h> 
#include <iostream>
#include <fstream>
#include <vector>


 using namespace Magick;
 using namespace std;


vector<string> poem;
int marginpx = 20;



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

int getPtSizeTarget(string longestLine, Image image){
    int targetWidth = 1080 - 2 * marginpx;
    //size_points = (size_pixels * 72)/resolution


    TypeMetric currLineMetrics;
    //should be an under-estimate.
    int lower = (targetWidth)/ (longestLine.size());
    int higher = 100; //max acceptible size
    int test = lower;

    image.fontPointsize(lower);

    int width = 9999;
    while((lower != higher) ){
        image.fontTypeMetrics(longestLine, &currLineMetrics);
        width = currLineMetrics.textWidth();

        std::cout<< "ptsize: " << test << " width: " << width << "\n";

        if(width > targetWidth){
            higher = test;
        } else{
            lower = test;
        }
        int adjustment = lower + ((higher - lower) / 2);
        test = adjustment > 0 ? adjustment : 1;
        image.fontPointsize(test);

        if(test == lower) {return test;}
    }

    return lower;

}


int main( ssize_t argc, char ** argv)
{
    
    if(readPoem()){
        return -1;
    }

    
   

    InitializeMagick(*argv);


    Image my_image( Geometry(1080,1080), Color("white"));
    // set the text rendering font (the color is determined by the "current" image setting)
    my_image.font("EVA-Matisse_Standard-EB");
    my_image.fontPointsize(100);
    
    
    //https://imagemagick.org/Magick++/Geometry.html

    int longestLine = 0;
    int maxWidth = 0;

    TypeMetric currLineMetrics;
    my_image.fontTypeMetrics(poem[0], &currLineMetrics);
    std::cout << "width: " << currLineMetrics.textWidth();

    for(int i=0;i<poem.size();i++){
        
        my_image.fontTypeMetrics(poem[i], &currLineMetrics);
        int currWidth = currLineMetrics.textWidth();
        if(currWidth > maxWidth ){
            maxWidth = currWidth;
            longestLine = i;
        }
        std::cout<< i << ": " << poem[i] << " -- " << currWidth << "\n";
    }

    std::cout<< "longest line: " << longestLine << "\n" << maxWidth << "\n";

    int ptSize = getPtSizeTarget(poem[longestLine], my_image);
    my_image.fontPointsize(ptSize);

    
    

    Geometry test("1080x200+0+100");

    my_image.annotate(poem[longestLine], CenterGravity);
    //my_image.annotate(poem[longestLine], test, CenterGravity);

    my_image.write("out.png");
    

    return 0;
}


