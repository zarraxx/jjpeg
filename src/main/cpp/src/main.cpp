#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "JpegCodec.h"
int main() {
    std::cout << "Hello, World!" << std::endl;

    std::fstream in;
    //in.open("/Users/zarra/ClionProjects/image_codec/jplossless.raw",std::ios::binary|std::ios::in);
    in.open("./jplossless.raw",std::ios::binary|std::ios::in);

    std::ostringstream out;


    DecodeByStreams(in,out);


    out.flush();

    std::string o = out.str();

    std::cout<<"size:"<<o.length()<<std::endl;
    return 0;
}