#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <getopt.h> //for flag parsing

//#include <ImageMagick-7/Magick++.h>
//#include <stdio.h>
//#include <iostream>



typedef struct USER_OPTS{

    bool malformed = false;

    int width = -1;

    std::string infile;
    std::string title;
    int useSameImage = false;
    std::string prefix = "image";
    int imageWidth = 1080;
    bool testMode = false;
    std::string fontFile;
    bool titleVerse = false;
    uint16_t legendBitMask = 0x8000; //=10....0
    std::string bgFile;
    std::string titleFile;

    std::string fontColourHex;
    std::string bgColourHex;

    Magick::Color fontColour;
    Magick::Color bgColour;

    bool verticalCentre = false;
    bool leftJustify = false;
    int topMargin = 200;
    int sideMargin = 0;
    int lineSpacing = 60;
    bool perImageFontResizing = false;

    bool useStdIn = false;
} USER_OPTS;


int useSameImage = false;

//https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
static struct option const long_options[] = //see https://linux.die.net/man/3/getopt_long //this is a struct option
  { //char *name, int has_arg, int *flag (pointer to load val into if flag true), int val  
  //help stdin fontcolour bgcolour sameimage title
    //{"sameimage", 0, 0, 1},
    {"sameimage", 0, &useSameImage, 1},
    {"fontcolour", 1, 0, '['}, 
    {"bgcolour", 1, 0, ']'},
    {"stdin", 0, 0, '@'},
    {"help", 0, 0, 'h'},
    {"title", 1, 0, '*'},
    {NULL, 0, NULL, 0} //required convention. Acts as a terminator struct for the thing reading long_options.
  };

int parseInt(std::string s){
    try{
        int c = std::stoi(s);
        return c;
    } catch(std::invalid_argument){
        
    }
    return 0;
}

uint16_t parseBinText(std::string s){
    return 0; //TODO
}

bool parseHexCode(std::string s, Magick::Color &colourVar){
    int l = s.length();
    if(l>=6 && l<=7){
        if(l==6){
            //prepend #
            s.insert(0, "#");

        }
        //does this work as I think?
        if(s[0] == '#'){
                //Magick::Color parsedColour = Magick::Color(s);
                colourVar = Magick::Color(s);
                std::cout << "final parsed str: " << s << "\n";
                return true;
        }
        
    }
    return false;
}

uint16_t parseHexText(std::string s){
    
    return 0; //TODO
}

USER_OPTS parseFlags(int argc, char* argv[]){

    struct USER_OPTS user_opts;

    

    int option_index = 0;
    int c;
    
    //Defaults
    bool helpFlag = false;
    user_opts.bgColour = Magick::Color("white");

    while ( (c = getopt_long(argc, argv, "W:df:Tk:K:b:B:clt:s:L:pi:o:h", long_options, &option_index)) != -1){
        switch(c){
            case 'W':
            {
                std::string s(optarg);
                int parsedInt = parseInt(s);
                if(!parsedInt){
                    std::cout << "W argument requires integer value\n";
                    user_opts.malformed = true;
                    //return -1;
                } else {
                    user_opts.imageWidth = parsedInt;
                    std::cout << "parsed W as " << parsedInt << "\n";
                }
                break;
            }
            case 'd':
            {
                user_opts.testMode = true;
                std::cout << "test mode on\n";
                break;
            }
            case '*':
            {
                std::string s(optarg);
                user_opts.title = s;
                std::cout << "Using title " << user_opts.title << "\n";
                break;
            }
            case 'f':
            {
                std::string s(optarg);
                user_opts.fontFile = s; 
                std::cout << "Using font file " << user_opts.fontFile << "\n";
                break;
            }
            case 'T':
            {
                user_opts.titleVerse = true;
                std::cout << "title verse active\n";
                break;
            }
            case 'k':{
                std::string s(optarg);
                user_opts.legendBitMask = parseBinText(s);
                std::cout << "legend bitmask active bin\n";
                break;
            }
            case 'K':{
                std::string s(optarg);
                user_opts.legendBitMask = parseHexText(s);
                std::cout << "legend bitmask active hex\n";
                break;
            }
            case 'b':
            {
                std::string s(optarg);
                user_opts.bgFile = s; 
                std::cout << "Using bg image file " << user_opts.bgFile << "\n";
                break;
            }
            case 'B':
            {
                std::string s(optarg);
                user_opts.titleFile = s; 
                std::cout << "Using bg image file " << user_opts.titleFile << "\n";
                break;
            }
            case '[':
             {
                std::string s(optarg);
                user_opts.fontColourHex = s; 

                if(!parseHexCode(s, user_opts.fontColour)){
                    user_opts.malformed = true;
                    std::cout << "could not parse hex code" << s << "\n";
                } else{
                    std::cout << (std::string) user_opts.fontColour << " is the colour now\n";
                }

                //std::cout << "font colour #" << s << "\n";
                break;
            }
            case ']':
             {
                std::string s(optarg);
                user_opts.bgColourHex = s; 
                if(!parseHexCode(s, user_opts.bgColour)){
                    std::cout << "could not parse bg hex" << s << "\n";
                } else{
                    std::cout << (std::string) user_opts.bgColour << " is the colour now\n";
                }
                break;
            }
            case 'c':
            {
                user_opts.verticalCentre = true;
                break;
            }
            case 'l':
            {
                user_opts.leftJustify = true;
                break;
            }
            case 't':
            {
                std::string s(optarg);
                int parsedInt = parseInt(s);
                if(!parsedInt){
                    std::cout << "t argument requires integer value\n";
                    user_opts.malformed = true;
                    //return -1;
                } else {
                    user_opts.topMargin = parsedInt;
                    std::cout << "parsed t as " << parsedInt << "\n";
                }
                break;
            }
            case 's':
            {
                std::string s(optarg);
                int parsedInt = parseInt(s);
                if(!parsedInt){
                    std::cout << "s argument requires integer value\n";
                    user_opts.malformed = true;
                    //return -1;
                } else {
                    user_opts.sideMargin = parsedInt;
                    std::cout << "parsed s as " << parsedInt << "\n";
                }
                break;
            }
            case 'L':
            {
                std::string s(optarg);
                int parsedInt = parseInt(s);
                if(!parsedInt){
                    std::cout << "L argument requires integer value\n";
                    user_opts.malformed = true;
                    //return -1;
                } else {
                    user_opts.lineSpacing = parsedInt;
                    std::cout << "parsed L as " << parsedInt << "\n";
                }
                break;
            }
            case 'p':
            {
                user_opts.perImageFontResizing = true;
                break;
            }      
            case 'i':
            {
                std::string s(optarg);
                user_opts.infile = s; 
                break;
            }
            case '@':
            {
                user_opts.useStdIn = true;
                std::cout << "reading from stdin\n";
                break;
            }
            case '?':
            {
                //std::cout << "command not recognised.\n";
                break;
            }
            case 'o':
            {
                std::string s(optarg);
                user_opts.prefix = s; 
                break;
            }

            case 'h':
            {helpFlag = true; break;}
        }
        
    }
    //since getopt_long takes pointer to int as a param and this doesn't exist until call, must do this.
    user_opts.useSameImage = useSameImage; 

    if( (user_opts.infile.size() == 0 && !user_opts.useStdIn) || helpFlag){ //probably a standard way to do this with lib. Also -i test -o --visualise gives bad behaviour.
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
         "--fontcolour [ABC123] = set font colour to be hex code [#ABC123]. requires no hash.\n"
         "--bgcolour [ABC123] use a solid bg colour. see above.\n"
         "-c = vertically centre verses (aligned to top by default)\n"
         "-l = left-justify text in verses (centred by default) \n"
         "-t [num] = set top margin to be [num] pixels\n"
         "-s [num] = set side margin to be [num] pixels\n"
         "-L [num] = set line spacing to be [num] pixels (60 by default)\n"
         "-p = do per-image font resizing (maximise text size on a per-image basis)\n"
         "-i [file.txt] = read from file.txt"
         "-o [prefix] = prefix images "
         "--linespacing [num] = space lines by [num] pixels. Default is 60.\n"
         "--stdin = read poem from stdin instead of from -i flag\n"
         "--help = print this dialogue\n"); 
         user_opts.malformed = true;
         //return -1;
    } else {
        std::cout << "i: " << user_opts.infile << ", o:" << user_opts.prefix << "\n";
    }
    //return 0;
    return user_opts;
}


/*

int main(int argc, char * argv[]){

    USER_OPTS opt = parseFlags(argc, argv);

    if(opt.useStdIn){
        std::string s;

        for (int i=0;std::getline(std::cin, s) ;i++){
        
            std::cout << s << "\n";

        }
    
    }

}*/