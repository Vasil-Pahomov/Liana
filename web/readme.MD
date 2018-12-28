To embed HTML file into ESP web server you should:
1. Minify the file using https://htmlcompressor.com/compressor/
2. Convert the minified file to C/C++ string (escape characters) using http://tomeko.net/online_tools/cpp_text_escape.php?lang=en
All this is to be automated https://github.com/Vasil-Pahomov/Liana/issues/9