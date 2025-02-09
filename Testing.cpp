#include <gtest/gtest.h>
#include <random>
#include <iostream>
#include <vector>
#include <ImageMagick-7/Magick++.h> 

#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <getopt.h> //for flag parsing

using namespace std;


vector<int> bruteSqueeze(vector<int> toSqueeze){
    vector<int> out;
    return out;
}

vector<int> naiveSqueeze(vector<int> toSqueeze){
    vector<int> out;
    return out;
}

vector<int> genRandomVec(int maxWidth, int length){
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> rand(0,maxWidth); // distribution in range [0, maxWidth]

    vector<int> out;

    for(int i=0;i<length;i++){
        out.push_back(rand(rng));
    }
    return out;
}

vector<string> modifyThing(vector<string> in){
    in.push_back("decayed?");
    return in;
}

void byRef(vector<string> &in){
    in.push_back("notDecayed?");
}

void drawTestImage()
{
    using namespace Magick;

    Image outImage( Geometry(1080,1080), Color("white"));
    outImage.font("EVA-Matisse_Standard-EB");
    outImage.fontPointsize(38);
    
    TypeMetric testLineInfo;

    outImage.fontTypeMetrics("Tortured by tremors and pushed close to breaking,", &testLineInfo );

    std::cout<<"text height: "<<testLineInfo.textHeight();

    Geometry currTextGeom(1080, 1080, 0, 0); //w, h, xoff, yoff
    outImage.annotate("Tortured by tremors and pushed close to breaking,", currTextGeom, NorthWestGravity);
    outImage.write("test.png");
}

//FLAGS
const int NAMEBUFFERSIZE = 60;
int VISUALISE_MODE = 0;
char INPUT_FILE[NAMEBUFFERSIZE];
char OUTPUT_FILE[NAMEBUFFERSIZE];

static struct option const long_options[] = //see https://linux.die.net/man/3/getopt_long //this is a struct option
  { //char *name, int has_arg, int *flag (pointer to load val into if flag true), int val  
    {"visualise", 0, &VISUALISE_MODE, 1},
    {NULL, 0, NULL, 0} //required convention. Acts as a terminator struct for the thing reading long_options.
  };

int parseFlags(int argc, char* argv[]){


    int c;
    while ( (c = getopt_long(argc, argv, "i:o:", long_options, NULL)) != -1){
        switch(c){
            
            case 'i':
            strcpy(INPUT_FILE, optarg); break;
            case 'o':
            strcpy(OUTPUT_FILE, optarg); break;
        }
        
    }
    if(INPUT_FILE[0] == '\0' || OUTPUT_FILE[0] == '\0'){ //probably a standard way to do this with lib. Also -i test -o --visualise gives bad behaviour.
         fprintf (stderr, "Incorrect arguments. Usage: MusWeb -i infile.m3u -o outfile.\n");
         return -1;
    }
    return 0;
}

void displayFlags(){

}

int main(int argc, char * argv[]){

    parseFlags(argc, argv);
    displayFlags();

    bool gotStdInput = false;

    if(isatty(STDIN_FILENO))
    {
        std::cout << "no stdin\n";
    } else {
        gotStdInput = true;
    }

    if(gotStdInput){
        string s;

        for (int i=0;std::getline(std::cin, s) ;i++){
        
            std::cout << s << "\n";

        }
    
    }
    



    //drawTestImage();
    /*
    for(int i=0;i<1;i++){
        vector<int> testData = genRandomVec(9,20);
        vector<int> brute = bruteSqueeze(testData);
        vector<int> naive = naiveSqueeze(testData);

        for (int j = 0; j < brute.size(); ++j) {
            EXPECT_EQ(brute[j], naive[j]) << "Vectors x and y differ at index " << j;
        }
        
    }*/
    /*
    vector<string> arrayDecayTestA = {"one", "two", "three", "four"};
    vector<string> arrayDecayTestB = {"one", "two", "three", "four"};


    std::cout  << modifyThing(arrayDecayTestA)[0] << " size:" << modifyThing(arrayDecayTestA).size();
    byRef(arrayDecayTestB);
    std::cout  << arrayDecayTestB[0] << arrayDecayTestB[4] << " size:" << arrayDecayTestB.size();*/


}