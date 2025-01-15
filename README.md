# AutoPoem
Dynamically chops an input text file into square images.

# Motivation
Up-and-coming poets have to market themselves. Nowadays this takes place over social media, i.e. Instagram. 
A unique challenge presented by Instagram is that all images have to be 1080x1080 square. This requires poems to be cut up into square blocks. 
This is, therefore, a repeated process aiming to maximise text size and minimise breaks between lines of the same paragraph. <br><br>
This seems to be a good candidate for automation.

(Note: while poetry is artistic, and some artists may wish to painstakingly beautify every poem-image, some opt for a minimalistic style. This programme is targeted at them)
# Dependencies:

## Build: 
ImageMagick
Magick++ header files

## Run:
ImageMagick

# Timeline:

Test image generation using Imagemagick   
Generate one large image from a text file **<--- HERE** <br>
Dynamically partition this image on line boundaries <br>
Centre text vertically in each square image <br>
Accept custom fonts <br>
Analyse a given text file and request the required (N*1080)x1080 custom continuous background image <br>
Either accept .rtf files or have some markdown for italics, bold, strikethrough. <br>
Integrate with Instagram API <br>
