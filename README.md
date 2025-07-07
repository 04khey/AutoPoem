# AutoPoem
Dynamically partitions an input text file into square images.

# Motivation
Up-and-coming poets have to market themselves. Nowadays this takes place over social media, i.e. Instagram. 
A unique challenge presented by Instagram is that all images have to be 1080x1080 square. This requires poems to be cut up into square blocks. 
This is, therefore, a repeated process aiming to maximise text size and minimise breaks between lines of the same paragraph. <br><br>


(Note: while poetry is an artistic endeavour, and some artists enjoy painstakingly beautifying every poem-image, some opt for a consistent and minimalistic style. This programme is targeted at them)
# Dependencies:

## Build: 
ImageMagick <br>
libmagick++-dev <br>
Magick++ header files

## Run:
ImageMagick

# Timeline:

Test image generation using Imagemagick   <br>
Generate one large image from a text file  <br>
Dynamically partition this image on line boundaries   <br>
Centre text vertically in each square image <br>
Accept custom fonts **<--- HERE** <br>
Analyse a given text file and request the required (N*1080)x1080 custom continuous background image (users swipe horizontally, so the image must be wide rather than tall) <br>
Title image generation with encodable legend/key images<br>
Text from STDIN<br>
Parse user flags for these <br
Parse config file for user-set defaults so flags don't need to be input every time<br>
Either accept .rtf files or have some markdown for italics, bold, strikethrough. <br>
Integrate with Instagram API <br>
