#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::string decode(unsigned long term)
{
    std::ostringstream outs;
    char letters[12];
    unsigned termSize = term & 0xFU;
    for (unsigned i = 0; i < termSize; ++i)
    {
        letters[i] = (term >> (i * 5 + 4) & 0x1FUL);
        // is letter?
        if ((letters[i] > 9) && (letters[i] < 10 + 22 + 1 ))  // WV: this is wrong! it's 22 I think
        {
            letters[i] += (letters[i] > 17) ? ((letters[i] > 21) ? ((letters[i] > 27) ? 4 : 3) : 2) : 0;
            letters[i] += 65 - 10;
            // is digit?
        }
        else
        {
            letters[i] = letters[i] + 48;
        }
        outs << letters[i];
    }
    //  std::cout << std::endl;
    //std::cout << "size: " << termSize << std::endl;
    std::string os = outs.str();
    return os;
}


std::vector<unsigned long> *readEncodedFile(std::string file)
{
    std::vector<unsigned long> *encoding = new std::vector<unsigned long>;
    std::ifstream fh(file.c_str());
    if (fh.bad())
    {
        std::cerr << "Error: Could not open " << file << std::endl;
    }
    unsigned long word;
    while (fh >> word)
    {
        encoding->push_back(word);
    }
    fh.close();
    return encoding;
}

int main(int argc, char *argv[])
{
    std::vector<unsigned long> *encoding;
    if (argc == 2)
    {
        encoding = readEncodedFile(argv[1]);
    }
    else
    {
        std::cout << "need file to decode" << std::endl;
        return 0;
    }
    for (int i = 0; i < encoding->size(); i++)
    {
        std::string decoded = decode(encoding->at(i));
        if (decoded.find("APW") != std::string::npos)
        {
            std::cout << std::endl;
        }
        std::cout << decode(encoding->at(i)) << " ";
    }
    return 0;
}