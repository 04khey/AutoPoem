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

struct lineMaxes{
    int longestLine = 0;
    int tallestLine = 0;

    int maxWidth = 0;
    int maxHeight = 0;
};


//vector<string> poem; //should this be vector<string*>? Probs not. Surely string* is the default. 
int sideMarginPx = 0;
int topMarginPx = 200;
int lineSpacing = 60;
//vector<int> runs;

bool doPerImageFontTargets =false;
bool centreVerticalAlign = false;

//GravityType desiredGravity = NorthGravity; //for verse alignment
GravityType desiredGravity = NorthWestGravity; //for verse alignment

vector<vector<string>> verses;
vector<lineMaxes> verseMaxes;
vector<int> numVersesInImages;



int readPoem(){

     //ifstream f(argv[1] + ".txt");
    ifstream f("seapoemspooky.txt"); //debug

    if (!f.is_open()) {
        cerr << "read error\n";
        return -1;
    }
    string s;
    bool newVerseFlag = 0;
    vector<string>* currentVerse;
    currentVerse = new vector<string>; 

    for (int i=0;getline(f, s) ;i++){
        
        //if(((*currentVerse).size() > 0)&&(newVerseFlag)){
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

int getLongestLineLength(int ptSize, vector<string> lines, int numLines, Image image){ //this doesn't decay. numLines always equals lines.size() 
    TypeMetric currLineMetrics;
    int length = 0;

    std::cout << "HERE: .size()=" << lines.size() << ", numLines= " << numLines << "\n"; 

    for(int i=0;i< numLines;i++){
        image.fontTypeMetrics(lines.at(i), &currLineMetrics);
        if(currLineMetrics.textWidth() > length){
            length = currLineMetrics.textWidth();
        }
    }
    
    return length;

}

int getPtSizeTarget(string longestLine, Image image){
    int targetWidth = 1080 - (2 * sideMarginPx);
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

int getMinElementInArray(int* inpArray, int size){
    int min = inpArray[0];
    for(int i=0; i <size; i++){
        if(inpArray[i] < min){
            min = inpArray[i];
        }
    }
    std::cout<<"min encountered: " << min << "\n";
    return min;
}

int getMaxElementInArray(int* inpArray, int size){
    int max = inpArray[0];
    for(int i=0; i <size; i++){
        if(inpArray[i] > max){
            max = inpArray[i];
        }
    }
    std::cout<<"max encountered: " << max << "\n";
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
            std::cout<< i << ": " << verse[i] << " -- " << currWidth << "\n";
        }
    return currLineMaxes;
}

void createImage(vector<string> lines, int fontSize, int imageNum, int maxHeight){

    Image outImage( Geometry(1080,1080), Color("white"));
    outImage.font("EVA-Matisse_Standard-EB");
    outImage.fontPointsize(fontSize);


    TypeMetric tm;

    int numLines = lines.size();
    std::cout<<"reachedfine!" << numLines <<"b\n";

    //int maxLength = getLongestLineLength(fontSize, lines, numLines, outImage);
    int startVerseIndex = 0;
    for(int i=0;i<imageNum;i++){
        
        startVerseIndex += numVersesInImages[i];
        std::cout<<"i: " << i << ", numVersesInImages[i]: " << numVersesInImages[i] << " svi: " << startVerseIndex <<"\n";
    }

    std::cout<<"size versemamlinel: " << numVersesInImages[imageNum] << "\n";
    int verseMaxLineLengths[numVersesInImages[imageNum]];
    for(int i=startVerseIndex;i<numVersesInImages[imageNum]+startVerseIndex;i++){
        lineMaxes tempLineMaxes = getLineMaxes(verses[i], &outImage, &tm);
        verseMaxLineLengths[i-startVerseIndex] = tempLineMaxes.maxWidth;
        std::cout << "verse " << imageNum << " max: " << tempLineMaxes.maxWidth << "\n";
    }

    std::cout<<"start verse index: " << startVerseIndex << "\n";
    int maxLength = getMaxElementInArray(verseMaxLineLengths, numVersesInImages[imageNum]);

    std::cout << "max line length for image " << imageNum << ": " << maxLength << "\n";

    //int topOffset = (1080 - topMarginPx - (numLines * (maxHeight + lineSpacing) * 2));
    std::cout << numLines <<" lines"<<", first line: " << lines[0] << "last line: " << lines[numLines - 1] << "\n";

    for(int i=0;i<numLines;i++){
        int tempwidth = maxLength;
        Geometry currTextGeom(tempwidth, 1080, (1080/2 - maxLength/2), topMarginPx + (i *(maxHeight + lineSpacing))); //w, h, xoff, yoff
        //Geometry currTextGeom(1080, 1080, sideMarginPx,  topMarginPx + (i *(maxHeight + lineSpacing))); //w, h, xoff, yoff
        outImage.annotate(lines.at(i), currTextGeom, desiredGravity);
    }
    //my_image.annotate(test, testGeom, WestGravity);

    char imNum[5];
    sprintf(imNum, "%d\0", imageNum);
    string str(imNum);

    outImage.write("Image" + str + ".png");
    
}

void createImages(vector<int> numVersesInImages, int fontSize, int maxHeight){

    std::cout<<"hi!\n" << numVersesInImages.size() << "\n" << fontSize << "\n" << maxHeight << "\n";
    int verseIndex =0;

    //
    Image testImage( Geometry(1080,1080), Color("white"));
    testImage.font("EVA-Matisse_Standard-EB");
    //



    for(int i=0;i<numVersesInImages.size(); i++){

        vector<string> lines;

          if(doPerImageFontTargets){ //TODO. 
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
    std::cout << "beep!\n";
    for(vector<string> verse : verses){
        std::cout<< "verse starting \n"<<verse[0] << "\n.verseCount: " << currentVerseCount << ", lineCount: " << currentLineCount << "\n";
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

int main( ssize_t argc, char ** argv)
{
    
    if(readPoem()){
        return -1;
    }

    int lineCounter=0;
    
    for(int i =0;i<verses.size();i++){
        std::cout<< "verses[" << i << "] size: " << verses[i].size() << "\n";
        for(int j =0;j<verses[i].size();j++){
            std::cout<< lineCounter << ": " << verses[i][j] << "OK\n";
            lineCounter++;
        }
        
    } std::cout<< "\n";

    InitializeMagick(*argv);


    Image my_image( Geometry(1080,1080), Color("white"));
    // set the text rendering font (the color is determined by the "current" image setting)
    my_image.font("EVA-Matisse_Standard-EB");
    
    
    
    //https://imagemagick.org/Magick++/Geometry.html

    TypeMetric currLineMetrics;

    for(vector<string> verse : verses){
        verseMaxes.push_back(getLineMaxes(verse, &my_image, &currLineMetrics));
    }

    for(int i=0;i<verses.size();i++){
        std::cout<< "verse" << i <<"longest line: " << verseMaxes[i].longestLine << "\n" << verseMaxes[i].maxWidth << "\n";
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
    
    //should be of longest line
    int ptSize = getPtSizeTarget(verses[longestVerse][verseMaxes[longestVerse].longestLine], my_image);
    my_image.fontPointsize(ptSize);

    int maxLines = getMaxPageLines(verses[longestVerse][verseMaxes[longestVerse].longestLine], my_image);

    //vector<int> optimumRuns;


    

    genNumVersesInImages(verses, maxLines);

    //vector<int> optimumRuns = squeezeRuns(maxLines, runs);

    for(int i : numVersesInImages){
        std::cout<< i << ",";
    }
    
    std::cout<< "max page lines: " << maxLines << "\n";

    createImages(numVersesInImages, ptSize, verseMaxes[0].maxHeight);

    //Geometry testGeom("1080x200+0+100");
    

    return 0;
}


