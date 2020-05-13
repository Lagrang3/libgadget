#include "gadget2.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>

template <unsigned int format>
void write_snapshot(const char* fname)
{
    gadget2::osnapshot<format> snap(fname);

    // generate header ...
    {
        auto& header = snap.get_raw_header();
        header.time = 1;
        header.redshift = 2.22045e-16;
        header.BoxSize = 479000;
        header.Omega0 = 0.3;
        header.OmegaLambda = 0.7;
        header.HubbleParam = 0.7;
        header.num_files = 1;
        header.npart[1] = 10;
        header.mass[1] = 1.0;
        header.npartTotal[1] = header.npart[1];
    }

    std::default_random_engine rng;
    std::uniform_real_distribution<double> uniform(0.0, 1.0);

    int npart = 0;
    for (int i = 0; i < gadget2::PTYPES; ++i)
        npart += snap.npart(i);

    std::vector<float> pos(3 * npart), vel(3 * npart);
    std::vector<uint64_t> ids(npart);

    for (size_t i = 0; i < pos.size(); ++i)
    {
        pos[i] = snap.get_raw_header().BoxSize * uniform(rng);
        vel[i] = uniform(rng);
    }

    std::iota(ids.begin(), ids.end(), 1);

    snap.write_header();
    snap.write_block("POS ", pos.begin(), pos.end());
    snap.write_block("VEL ", vel.begin(), vel.end());
    snap.write_block("ID  ", ids.begin(), ids.end());
}

template <unsigned int format>
void read_snapshot(const char* fname)
{
    gadget2::isnapshot<format> snap(fname);
    std::cout << snap << '\n';

    int npart = 0;
    for (int i = 0; i < gadget2::PTYPES; ++i)
        npart += snap.npart(i);

    const int display = 5;
    for (auto s : std::vector<std::string>{"POS ", "VEL "})
    {
        std::vector<float> buff(npart * 3);
        snap.scan_block(s, buff.begin());
        std::cout << "Block description: " << s << '\n';
        for (size_t i = 0; (i + 3) <= 3 * display and (i + 3) <= buff.size();
             i += 3)
        {
            std::cout << "(" << buff[i] << ", " << buff[i + 1] << ", "
                      << buff[i + 2] << ")\n";
        }
        std::cout << "...\n\n";
    }
}

int main()
{
    try
    {
        write_snapshot<1>("snap1");
        read_snapshot<1>("snap1");
        write_snapshot<2>("snap2");
        read_snapshot<2>("snap2");
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
