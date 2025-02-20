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
int width = -1;

string infile;
string title;
int useSameImage = false;
string prefix = "image";
int imageWidth = 1080;
bool testMode = false;
string fontFile;
bool titleVerse = false;

//https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
static struct option const long_options[] = //see https://linux.die.net/man/3/getopt_long //this is a struct option
  { //char *name, int has_arg, int *flag (pointer to load val into if flag true), int val  
  //help stdin fontcolour bgcolour sameimage title
    {"sameimage", 0, 0, 1},
    {"sameimage", 0, &useSameImage, 1},
    {NULL, 0, NULL, 0} //required convention. Acts as a terminator struct for the thing reading long_options.
  };

int parseInt(string s){
    try{
        int c = std::stoi(s);
        return c;
    } catch(std::invalid_argument){
        
    }
    return 0;
}

int parseFlags(int argc, char* argv[]){

    int option_index = 0;
    int c;
    bool helpFlag = false;
    while ( (c = getopt_long(argc, argv, "W:df:Tk:K:b:B:clt:s:pi:o:h", long_options, &option_index)) != -1){
        switch(c){
            case 'W':
            {
                std::string s(optarg);
                int parsedInt = parseInt(s);
                if(!parsedInt){
                    std::cout << "W argument requires integer value\n";
                    return -1;
                } else {
                    imageWidth = parsedInt;
                    std::cout << "parsed W as " << parsedInt << "\n";
                }
                break;
            }
            case 'd':
            {
                testMode = true;
                std::cout << "test mode on\n";
                break;
            }
            case 'f':
            {
                std::string s(optarg);
                fontFile = s; 
                std::cout << "Using font file " << fontFile << "\n";
                break;
            }
            case 'T':
            {
                titleVerse = true;
                std::cout << "title verse active\n";
                break;
            }

            
                        
            case 'i':
            {
                std::string s(optarg);
                infile = s; 
                break;
            }
            case 'o':
            {
                std::string s(optarg);
                prefix = s; 
                break;
            }

            case 'h':
            {helpFlag = true; break;}
        }
        
    }
    if(infile.size() == 0 || helpFlag){ //probably a standard way to do this with lib. Also -i test -o --visualise gives bad behaviour.
         fprintf (stderr, "Invalid arguments. Available commands (case sensitive):\n\n"
         "-W [num] = set image width to be [num] pixels\n"
         "-d = get required dimensions for background image\n"
         "-f [font] = use font [font]. Can specify exact filename (relative) or family name as in fc-list o.e. can also symlink into this directory.\n"
         "--title \"example multi word title\" = create title card with specified title. Use speech marks to enclose.\n"
         "-T = use first verse as title. Allows title to be broken over multiple lines.\n"
         "-k 0100110011... = add key/legend to title card. Masks files named [0.png,1.png,2.png,...]. arranges in a 2x8 grid below title card. Right-padded with zeroes. Right MSB.\n"
         "-K [hex] = same as above but as 4 hex digits\n"
         "-b [image] = use [image] as a background image\n"
         "-B [image] = use [image] as a titlecard background image\n"
         "--sameimage [image] = repeat same 1080x1080 square image as a background\n"
         "--fontcolour [#ABC123] = set font colour to be hex code supplied in [#ABC123]. requires hash.\n"
         "--bgcolour [#ABC123] use a solid bg colour. see above.\n"
         "-c = vertically centre verses (aligned to top by default)\n"
         "-l = left-justify text in verses (centred by default) \n"
         "-t [num] = set top margin to be [num] pixels\n"
         "-s [num] = set side margin to be [num] pixels\n"
         "-p = do per-image font resizing (maximise text size on a per-image basis)\n"
         "-i [file.txt] = read from file.txt"
         "-o [prefix] = prefix images "
         "--linespacing [num] = space lines by [num] pixels. Default is 60.\n"
         "--stdin = read poem from stdin instead of from -i flag\n"
         "--help = print this dialogue\n"); 
         return -1;
    } else {
        std::cout << "i: " << infile << ", o:" << prefix << "\n";
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