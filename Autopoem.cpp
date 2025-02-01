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
 using namespace std; //this is supposedly bad practice. But this is a small project, so should be fine?


vector<string> poem; //should this be vector<string*>?
int sideMarginPx = 100;
int topMarginPx = 200;
int lineSpacing = 20;
vector<int> runs;



int readPoem(){
    
    int runCounter=0;

     //ifstream f(argv[1] + ".txt");
    ifstream f("seapoemspooky.txt"); //debug

    if (!f.is_open()) {
        cerr << "read error\n";
        return -1;
    }
    string s;

    for (int i=0;getline(f, s) ;i++){

        if(s == ""){ //empty line
            runs.push_back(runCounter);
            runCounter = 0;

        } else {
            runCounter++;
        }

        poem.push_back(s);
        //poem[i] = s;
    }
    runs.push_back(runCounter + 1); //+1 to account for final line



    f.close();

    return 0;
}

void printVector(vector<string> in){ //todo: use autos for indices, replace some of these vectors with std::arrays
    for(int i=0;i<in.size();i++){
        std::cout << i << ": " << in[i];
    }
}

void trimArray(vector<string> in){
    return;
    //int start = 0;
    //int end = *sizePtr -1;
    while(in.front() == ""){
        in.erase(in.begin());
    }
    while(in.back() == ""){
        in.pop_back();
    }
    //*sizePtr = end - start + 1;
    //return &array[start];
}

int getLongestLineLength(int ptSize, vector<string> lines, int numLines, Image image){
    TypeMetric currLineMetrics;
    int length = 0;

    for(int i=0;i< numLines;i++){
        image.fontTypeMetrics(lines.at(i), &currLineMetrics);
        if(currLineMetrics.textWidth() > length){
            length = currLineMetrics.textWidth();
        }
    }
    
    return length;

}

int getPtSizeTarget(string longestLine, Image image){
    int targetWidth = 1080 - 2 * sideMarginPx;
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

        //std::cout<< "ptsize: " << test << " width: " << width << "\n";

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

int getMaxPageLines(string tallestLine, Image image){

    TypeMetric currLineMetrics;
    image.fontTypeMetrics(tallestLine, &currLineMetrics);

    int lineHeight = currLineMetrics.textHeight();

    int maxLines = (1080 - (2 * topMarginPx) ) / (lineHeight + lineSpacing);

    return maxLines;
}

vector<int> squeezeRuns(int maxLines, vector<int> runsIn){
    int runningCount =0;
    vector<int> squeezed;
    for(int i=0;i<runsIn.size();i++){
        if(runsIn[i] > maxLines){
            if(runningCount > 0){
                squeezed.push_back(runningCount);
                runningCount = 0;
            }
            squeezed.push_back(runsIn[i]);

        } else if (runsIn[i] + runningCount + 1> maxLines){
            squeezed.push_back(runningCount);
            runningCount = runsIn[i];
        } else {
            if(runsIn[i] == 0){
                runningCount += 1; //maintain spaces between two verses on same image
            } else {
                runningCount += runsIn[i] + 1;
            }
            
        }
    }
    if(runningCount > 0){
        squeezed.push_back(runningCount);
    }
    return squeezed;
}

void createImage(vector<string> lines, int fontSize, int imageNum, int numLines, int maxHeight){

    trimArray(lines);

    Image outImage( Geometry(1080,1080), Color("white"));
    // set the text rendering font (the color is determined by the "current" image setting)
    outImage.font("EVA-Matisse_Standard-EB");
    outImage.fontPointsize(fontSize);

    int maxLength = getLongestLineLength(fontSize, lines, numLines, outImage);

    int topOffset = (1080 - topMarginPx - (numLines * (maxHeight + lineSpacing) * 2));
    std::cout << numLines <<" lines"<<", first line: " << lines[0] << "last line: " << lines[numLines - 1] << "\noffset: " << topOffset << "px\n";

    for(int i=0;i<numLines;i++){
        Geometry currTextGeom(1080, 1080, sideMarginPx + (1080 - (2 * sideMarginPx) - maxLength)/2, -(1080/2) + topMarginPx + topOffset+ (i *(maxHeight + lineSpacing))); //w, h, xoff, yoff
        //Geometry currTextGeom(1080, 1080, sideMarginPx,  topMarginPx + (i *(maxHeight + lineSpacing))); //w, h, xoff, yoff
        outImage.annotate(lines.at(i), currTextGeom, WestGravity);
    }
    //my_image.annotate(test, testGeom, WestGravity);

    char imNum[5];
    sprintf(imNum, "%d\0", imageNum);
    string str(imNum);

    outImage.write("Image" + str + ".png");
    
}

void createImages(vector<int> squeezedLines, int fontSize, int maxHeight){
    int numRequired = squeezedLines.size();
    int count = 0;
    for(int i=0;i<squeezedLines.size(); i++){
        vector<string> lines;

        for(int j = 0; j < squeezedLines[i];j++){
            lines.push_back(poem[count]);
            count++;
        }

        createImage(lines, fontSize, i, squeezedLines[i], maxHeight);
    }


}

int main( ssize_t argc, char ** argv)
{
    
    if(readPoem()){
        return -1;
    }

    for(int i =0;i<runs.size();i++){
        std::cout<< runs[i] << ",";
    } std::cout<< "\n";

    InitializeMagick(*argv);


    Image my_image( Geometry(1080,1080), Color("white"));
    // set the text rendering font (the color is determined by the "current" image setting)
    my_image.font("EVA-Matisse_Standard-EB");
    
    
    
    //https://imagemagick.org/Magick++/Geometry.html

    int longestLine = 0;
    int tallestLine = 0;

    int maxWidth = 0;
    int maxHeight = 0;
    

    TypeMetric currLineMetrics;

    for(int i=0;i<poem.size();i++){
        
        my_image.fontTypeMetrics(poem[i], &currLineMetrics);
        int currWidth = currLineMetrics.textWidth();
        if(currWidth > maxWidth ){
            maxWidth = currWidth;
            longestLine = i;
        }
        int currHeight = currLineMetrics.textHeight();
        if(currHeight > maxHeight ){
            maxHeight = currHeight;
            tallestLine = i;
        }
        std::cout<< i << ": " << poem[i] << " -- " << currWidth << "\n";
    }

    std::cout<< "longest line: " << longestLine << "\n" << maxWidth << "\n";

    int ptSize = getPtSizeTarget(poem[longestLine], my_image);
    my_image.fontPointsize(ptSize);

    int maxLines = getMaxPageLines(poem[tallestLine], my_image);

    vector<int> optimumRuns = squeezeRuns(maxLines, runs);

    for(int i : optimumRuns){
        std::cout<< i << ",";
    }
    
    std::cout<< "max page lines: " << maxLines << "\n";

    createImages(optimumRuns, ptSize, maxHeight);

    Geometry testGeom("1080x200+0+100");
    //my_image.annotate(poem[longestLine], CenterGravity);

    string test = "line 1\nline 2 blah blah blah\nline 3";

    my_image.annotate(test, testGeom, WestGravity);

    //my_image.annotate(poem[longestLine], test, CenterGravity);

    my_image.write("out.png");
    

    return 0;
}


