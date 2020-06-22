#include "gadget.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

template <unsigned int format>
int g2_ls(int narg, char** argv)
{
    try
    {
        gadget::isnapshot<format> snap(argv[1]);
        std::cout << snap << '\n';

        int npart = 0;
        for (int i = 0; i < gadget::PTYPES; ++i)
            npart += snap.npart(i);

        const int display = 5;
        for (auto s : std::vector<std::string>{"POS ", "VEL "})
        {
            std::vector<float> buff(npart * 3);
            snap.scan_block(s, buff.begin());
            std::cout << "Block description: " << s << '\n';
            for (size_t i = 0;
                 (i + 3) <= 3 * display and (i + 3) <= buff.size(); i += 3)
            {
                std::cout << "(" << buff[i] << ", " << buff[i + 1] << ", "
                          << buff[i + 2] << ")\n";
            }
            std::cout << "...\n\n";
        }

        // scan_block(in, "ID ");
        return EXIT_SUCCESS;
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "Exception opening/reading file\n";
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return EXIT_FAILURE;
}

int main(int narg, char** argv)
{
    if (g2_ls<1>(narg, argv) == EXIT_SUCCESS)
    {
        return EXIT_SUCCESS;
    }
    if (g2_ls<2>(narg, argv) == EXIT_SUCCESS)
    {
        return EXIT_SUCCESS;
    }
    std::cerr << "Error. " << argv[0] << ": Gadget format not recognized\n";
    return EXIT_FAILURE;
}
