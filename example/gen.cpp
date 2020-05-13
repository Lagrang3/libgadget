#include "gadget2.hpp"
#include <iostream>
#include <random>
using namespace std;

template<unsigned int format>
void gaussian(const char* fname,const float sigma)
{
    gadget2::osnapshot<format> snap(fname);
    const double BoxSize = 320'000;
    {
        auto& header = snap.get_raw_header();
        header.BoxSize = BoxSize;
        header.num_files = 1;
        header.npart[1] = 16777216;
        header.mass[1] = 1.0;
        header.npartTotal[1] = header.npart[1];
    }
    
    
    
    std::default_random_engine rng;
    std::normal_distribution<float> normal(BoxSize/2,BoxSize * sigma);
    std::uniform_real_distribution<float> uniform(0.0, 1.0);

    int npart = 0;
    for (int i = 0; i < gadget2::PTYPES; ++i)
        npart += snap.npart(i);

    std::vector<float> pos(3 * npart), vel(3 * npart);
    std::vector<uint64_t> ids(npart);

    for (size_t i = 0; i < pos.size(); ++i)
    {
        float p = normal(rng);
        while(p>=BoxSize)p-=BoxSize;
        while(p<0) p+=BoxSize;
        pos[i] = p;
        vel[i] = uniform(rng);
    }

    std::iota(ids.begin(), ids.end(), 1);

    snap.write_header();
    snap.write_block("POS ", pos.begin(), pos.end());
    snap.write_block("VEL ", vel.begin(), vel.end());
    snap.write_block("ID  ", ids.begin(), ids.end());
}

template<unsigned int format>
void random(const char* fname)
{
    gadget2::osnapshot<format> snap(fname);

    {
        auto& header = snap.get_raw_header();
        header.BoxSize = 320000;
        header.num_files = 1;
        header.npart[1] = 5;
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

void generate_large_scale_gaussian()
{
    float sigma = 1;
    for(int i=0;i<5;++i,sigma*=.5)
    {
        char fname[100];
        sprintf(fname,"gaussian-snap%d",i);
        gaussian<1>(fname,sigma);
    }
}

int main()
{
    random<1>("small-snap");
    return 0;
}

