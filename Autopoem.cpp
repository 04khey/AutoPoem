/* http://www.imagemagick.org/Magick++/tutorial/Magick++_tutorial.pdf
* https://www.imagemagick.org/Magick++/
* https://imagemagick.org/Magick++/Image++.html
 */

//c++ -O2 -o autopoem Autopoem.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

#include <ImageMagick-7/Magick++.h> 
#include <iostream>
#include <fstream>
#include <vector>

#include <unistd.h>
#include <stdio.h>

//I think this is normally a cardinal sin but I just want to include all of parsePoemInput, so I do this
#include "parsePoemInput.cpp"

 using namespace Magick;
 using namespace std; //this is supposedly bad practice. But this is a small project, so should be fine?

struct lineMaxes{
    int longestLine = 0;
    int tallestLine = 0;

    int maxWidth = 0;
    int maxHeight = 0;
};


//USER PARAMS STRUCT
USER_OPTS opts;

//USER-SET PARAMETERS
//int IMAGE_WIDTH = 1080;
//int sideMarginPx = 0;
//int topMarginPx = 200;
//int lineSpacing = 60;

const string fontFilename = "EVA-Matisse_Standard-EB";

bool doPerImageFontTargets = true;
bool centreVerticalAlign = true;
//bool checkMode = false;
bool stdinpoem = false;
string userpoem = "seapoemspooky.txt";

GravityType desiredGravity;
// = NorthGravity; //for verse alignment
//GravityType desiredGravity = NorthWestGravity; //for verse alignment

//GLOBALS
vector<vector<string>> verses;
vector<lineMaxes> verseMaxes;
vector<int> numVersesInImages;



int readPoem(string filename){

    ifstream f(filename);
    
    if (!f.is_open()) {
        cerr << "read error\n";
        return -1;
    }

    string s;
    bool newVerseFlag = 0;
    vector<string>* currentVerse;
    currentVerse = new vector<string>; 

    for (int i=0; getline(f, s) ;i++){
        
        if((newVerseFlag) && s!= ""){
                verses.push_back(*currentVerse);
                currentVerse = new vector<string>; 
                newVerseFlag = 0;
        }

        if(s == ""){ 
            newVerseFlag = 1;
            
        } else {
           (*currentVerse).push_back(s); 
        }

    }

    verses.push_back(*currentVerse);

    f.close();
    return 0;
}

int readStdInPoem(){
    string s;
    bool newVerseFlag = 0;
    vector<string>* currentVerse;
    currentVerse = new vector<string>; 

    for (int i=0; getline(std::cin, s) ;i++){
        
        if((newVerseFlag) && s!= ""){
                verses.push_back(*currentVerse);
                currentVerse = new vector<string>; 
                newVerseFlag = 0;
        }

        if(s == ""){ 
            newVerseFlag = 1;
            
        } else {
           (*currentVerse).push_back(s); 
        }

    }

    verses.push_back(*currentVerse);
    return 0;
}


void trimArray(vector<string> in){ //unnecessary but I'm keeping it for now in case debugging deems it necessary
    return;
    while(in.front() == ""){
        in.erase(in.begin());
    }
    while(in.back() == ""){
        in.pop_back();
    }
}

int getLongestLineLength(int ptSize, vector<string> lines, int numLines, Image image){ //this doesn't decay. numLines always equals lines.size() 
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
    int targetWidth = opts.imageWidth - (2 * opts.sideMargin);
    //size_points = (size_pixels * 72)/resolution


    TypeMetric currLineMetrics;
    
    int lower = (targetWidth)/ (longestLine.size()); //should be an under-estimate.
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

    int lineHeight = currLineMetrics.textHeight();//bad?

    int maxLines = (1080 - (2 * opts.topMargin) ) / (lineHeight + opts.lineSpacing);

    return maxLines;
}

int getMinElementInArray(int* inpArray, int size){
    int min = inpArray[0];
    for(int i=0; i <size; i++){
        if(inpArray[i] < min){
            min = inpArray[i];
        }
    }
    return min;
}

int getMaxElementInArray(int* inpArray, int size){
    int max = inpArray[0];
    for(int i=0; i <size; i++){
        if(inpArray[i] > max){
            max = inpArray[i];
        }
    }
    return max;
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


lineMaxes getLineMaxes(vector<string> verse, Image* my_image, TypeMetric* currLineMetrics){
    int currWidth =0;
    int currHeight = 0;

    lineMaxes currLineMaxes;
    for(int i=0;i<verse.size();i++){
            my_image->fontTypeMetrics(verse[i], currLineMetrics);
            int currWidth = currLineMetrics->textWidth();
            if(currWidth > currLineMaxes.maxWidth ){
                currLineMaxes.maxWidth = currWidth;
                currLineMaxes.longestLine = i;
            }
            int currHeight = currLineMetrics->textHeight();
            if(currHeight > currLineMaxes.maxHeight ){
                currLineMaxes.maxHeight = currHeight;
                currLineMaxes.tallestLine = i;
            }
            //std::cout<< i << ": " << verse[i] << " -- " << currWidth << "\n";
        }
    return currLineMaxes;
}

void createImage(vector<string> lines, int fontSize, int imageNum, int maxHeight){
    //maxHeight seems not to be what it should. Why?

    Image outImage( Geometry(opts.imageWidth,1080), opts.bgColour);
    //Image outImage( Geometry(opts.imageWidth,1080), Color("white"));
    
    //outImage.font("EVA-Matisse_Standard-EB");
    outImage.font(fontFilename);
    outImage.fontPointsize(fontSize);

    outImage.fillColor(opts.fontColour);


    int numLines = lines.size();

    int centralisationOffset=0;
    if(centreVerticalAlign){
        int bottomOfText = opts.topMargin + ((numLines+1-1) *(maxHeight + opts.lineSpacing));
        int bottomPossible = 1080 - maxHeight - opts.topMargin;
        centralisationOffset = (bottomPossible-bottomOfText)/2;
    }


    TypeMetric tm;


    //int maxLength = getLongestLineLength(fontSize, lines, numLines, outImage);
    int startVerseIndex = 0;
    for(int i=0;i<imageNum;i++){
        startVerseIndex += numVersesInImages[i];
    }

    int verseMaxLineLengths[numVersesInImages[imageNum]];
    for(int i=startVerseIndex;i<numVersesInImages[imageNum]+startVerseIndex;i++){
        lineMaxes tempLineMaxes = getLineMaxes(verses[i], &outImage, &tm);
        verseMaxLineLengths[i-startVerseIndex] = tempLineMaxes.maxWidth;
    }

    int maxLength = getMaxElementInArray(verseMaxLineLengths, numVersesInImages[imageNum]);

    //std::cout << "max line length for image " << imageNum << ": " << maxLength << "\n";


    std::cout << numLines <<" lines"<<", first line: " << lines[0] << "last line: " << lines[numLines - 1] << "\n";

    for(int i=0;i<numLines;i++){
        int tempwidth = maxLength;
        Geometry currTextGeom(tempwidth, opts.imageWidth, (opts.imageWidth/2 - maxLength/2), opts.topMargin + (i *(maxHeight + opts.lineSpacing))+centralisationOffset ); //w, h, xoff, yoff
        outImage.annotate(lines.at(i), currTextGeom, desiredGravity);
    }

    char imNum[5]; //will go out of bounds if > 99,999 images but I think there will be other problems then
    sprintf(imNum, "%d\0", imageNum);
    string str(imNum);

    outImage.write(opts.prefix + str + ".png");
    
}

void createImages(vector<int> numVersesInImages, int fontSize, int maxHeight){

    int verseIndex =0;

    //used to determine per-image font size
    Image testImage( Geometry(opts.imageWidth,1080), Color("white"));
    testImage.font(fontFilename);




    for(int i=0;i<numVersesInImages.size(); i++){

        vector<string> lines;

          if(opts.perImageFontResizing){ 
            int possibleFontSizeTargets[numVersesInImages[i]];
            for(int j=0;j<numVersesInImages[i];j++){
                possibleFontSizeTargets[j] = getPtSizeTarget(verses[j+verseIndex][verseMaxes[j+verseIndex].longestLine], testImage);
            }
            fontSize = getMinElementInArray(possibleFontSizeTargets, numVersesInImages[i]);
          }

        for(int j = 0; j < numVersesInImages[i];j++){
            
            for(string s : verses[verseIndex+j]){
                lines.push_back(s);
            }
            if(j != numVersesInImages[i]-1){
                lines.push_back("\n"); 
            }
              
        }


        createImage(lines, fontSize, i, maxHeight);

        verseIndex += numVersesInImages[i];
    }


}

void genNumVersesInImages(vector<vector<string>> verses, int maxLines){
    int currentVerseCount=0;
    int currentLineCount=0;

    for(vector<string> verse : verses){
        //std::cout<< "verse starting \n"<<verse[0] << "\n.verseCount: " << currentVerseCount << ", lineCount: " << currentLineCount << "\n";
        //TODO: if verse longer than maxlines, chunk verse and generate new verses vector
        if((verse.size() + 1 + currentLineCount > maxLines) && currentVerseCount>0){
            numVersesInImages.push_back(currentVerseCount);
            currentVerseCount = 1;
            currentLineCount = verse.size(); 
            
        } else {
            currentLineCount = currentLineCount + 1 + verse.size();
            currentVerseCount++;
        }
    }

    if(currentVerseCount > 0){
        numVersesInImages.push_back(currentVerseCount);
    }


}

int main(int argc, char* argv[])
{
    opts = parseFlags(argc, argv);

    if(opts.malformed){
        return -1;
    }
    opts.leftJustify ? desiredGravity = NorthWestGravity : desiredGravity = NorthGravity; 


    if(opts.useStdIn){
        if(readStdInPoem()){
            return -1;
        }
    } else if(readPoem(userpoem)){ //debug poem
        return -1;
    }

    int lineCounter=0;
    
    for(int i =0;i<verses.size();i++){
        std::cout<< "verses[" << i << "] size: " << verses[i].size() << "\n";
        for(int j =0;j<verses[i].size();j++){
            std::cout<< lineCounter << ": " << verses[i][j] << "\n";
            lineCounter++;
        }
        
    } std::cout<< "\n";

    InitializeMagick(*argv);


    Image my_image( Geometry(opts.imageWidth,1080), Color("white"));
    // set the text rendering font (the color is determined by the "current" image setting)
    my_image.font(fontFilename);
    
    
    
    //https://imagemagick.org/Magick++/Geometry.html

    TypeMetric currLineMetrics;

    for(vector<string> verse : verses){
        verseMaxes.push_back(getLineMaxes(verse, &my_image, &currLineMetrics));
    }

    for(int i=0;i<verses.size();i++){
        std::cout<< "verse " << i <<" longest line:" << verseMaxes[i].longestLine << ", maxWidth:" << verseMaxes[i].maxWidth << "\n";
    }

    //get longest verse for point size calculation.
    int longestVerse = 0;
    int longestVerseLine = verseMaxes[0].maxWidth;
    for(int i=1;i<verseMaxes.size();i++){
        if(verseMaxes[i].maxWidth > longestVerseLine){
            longestVerseLine = verseMaxes[i].maxWidth;
            longestVerse = i;
        }
    }
    std::cout<< "longest verse: " << longestVerse << ", longest line: " << longestVerseLine << "\n";

    int ptSize = getPtSizeTarget(verses[longestVerse][verseMaxes[longestVerse].longestLine], my_image);
    my_image.fontPointsize(ptSize);

    int maxLines = getMaxPageLines(verses[longestVerse][verseMaxes[longestVerse].longestLine], my_image);


    genNumVersesInImages(verses, maxLines);

    for(int i : numVersesInImages){
        std::cout<< i << ",";
    }
    
    //std::cout<< "max page lines: " << maxLines << "\n";

    if(opts.testMode){
        std::cout<< "\nRequired background image dimensions: " << numVersesInImages.size() * opts.imageWidth << "x1080" << "\n\nPress Enter to continue.";

        //wait for key input in case this is a windows system or some other case where terminal auto-closes.
        int c;
        c = getchar();
        
        return 0;
    } 

    createImages(numVersesInImages, ptSize, verseMaxes[0].maxHeight); //using verseMaxes[0].maxHeight is perhaps lazy, but I think it unlikely this will ever be an issue.
    
    return 0;
}


