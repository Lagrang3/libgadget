#pragma once

#include <cassert>
#include <exception>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

#define debug(x) std::cout << #x " = " << x << '\n';

namespace gadget2
{
    const int PTYPES = 6;  // number of particle types
    const uint32_t HDR_SIZE = 256;

    struct blk_header
    {
        int blksize1;
        char label[4];
        int nextblk;
        int blksize2;
    };

    union raw_header {
        struct
        {
            int npart[PTYPES];
            double mass[PTYPES];
            double time;
            double redshift;
            int flag_sfr;
            int flag_feedback;
            int npartTotal[PTYPES];
            int flag_cooling;
            int num_files;
            double BoxSize;
            double Omega0;
            double OmegaLambda;
            double HubbleParam;
        };
        char __fill__[HDR_SIZE];
    };

    struct snap_header
    {
        raw_header _data;
        snap_header()
        {
            std::fill(_data.__fill__, _data.__fill__ + HDR_SIZE, 0);
        }
        snap_header(raw_header data) : _data(data) {}
        double get_BoxSize()const{return _data.BoxSize;}
    };

    std::ostream& operator<<(std::ostream& os, const snap_header& H);

    auto& ls_block(std::istream& in);
    void ls(std::istream& in);

    template <unsigned int format>
    class base_snapshot
    {
       public:
        base_snapshot(const char* _fname) : filename(_fname) {}

        std::string filename;
        snap_header header;

        int& npart(int i) { return header._data.npart[i]; }
        const int& npart(int i) const { return header._data.npart[i]; }
        const auto& get_raw_header() const { return header._data; }
        auto& get_raw_header() { return header._data; }
    };

    template <unsigned int format>
    std::ostream& operator<<(std::ostream& os,
                             const base_snapshot<format>& snap)
    {
        os << "Snapshot file: " << snap.filename << '\n';
        os << "Format: " << format << '\n';
        os << snap.header << '\n';
        return os;
    }

    template <unsigned int format>
    class base_osnapshot : public base_snapshot<format>
    {
       protected:
        mutable std::ofstream out;

       public:
        base_osnapshot(const char* _fname)
            : base_snapshot<format>(_fname), out(_fname, std::ios::binary)
        {
        }

        template <class iterator>
        auto& write_raw_block(std::ostream& out,
                              iterator begin,
                              iterator end) const
        {
            const size_t sz = sizeof(*begin);
            const uint32_t blksize = sz * std::distance(begin, end);
            out.write(reinterpret_cast<const char*>(&blksize), sizeof(blksize));
            for (; begin != end; ++begin)
            {
                auto& tmp = *begin;
                out.write(reinterpret_cast<char*>(&tmp), sz);
            }
            out.write(reinterpret_cast<const char*>(&blksize), sizeof(blksize));
            return out;
        }
        auto& write_raw_header(std::ostream& out) const
        {
            const uint32_t blksize = HDR_SIZE;
            out.write(reinterpret_cast<const char*>(&blksize), sizeof(blksize));
            out.write(reinterpret_cast<const char*>(&this->get_raw_header()),
                      blksize);
            out.write(reinterpret_cast<const char*>(&blksize), sizeof(blksize));
            return out;
        }
    };

    template <unsigned int format>
    class base_isnapshot : public base_snapshot<format>
    {
       protected:
        std::ifstream in;

       public:
        base_isnapshot(const char* _fname)
            : base_snapshot<format>(_fname)
        {
            try
            {
                in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                in.open(_fname,std::ios::binary);
            }catch(const std::ifstream::failure& e)
            {
                std::ostringstream mess;
                mess << "Exception opening file "<<_fname ;
                throw std::runtime_error(mess.str());
            }
        }
        // using base_snapshot<format>::base_snapshot;  //(const char*);

        auto& skip_block(std::istream& in) const
        {
            uint32_t blksize1, blksize2;
            in.read(reinterpret_cast<char*>(&blksize1), sizeof(blksize1));
            in.seekg(blksize1, std::ios::cur);
            in.read(reinterpret_cast<char*>(&blksize2), sizeof(blksize2));

            if (blksize1 != blksize2)
                throw std::runtime_error("Missmatched placeholders size");

            return in;
        }
        template <class iterator>
        auto& copy_block(std::istream& in, iterator buff) const
        {
            uint32_t blksize1, blksize2;
            const size_t sz = sizeof(*buff);
            in.read(reinterpret_cast<char*>(&blksize1), sizeof(blksize1));

            for (int remain = blksize1; remain > 0; remain -= sz, ++buff)
            // for(int remain = 100;remain>0;remain-=sz,++buff)
            {
                auto& tmp = *buff;
                in.read(reinterpret_cast<char*>(&tmp), sz);
                // debug(tmp);
            }
            in.read(reinterpret_cast<char*>(&blksize2), sizeof(blksize2));

            if (blksize1 != blksize2)
                throw std::runtime_error("Missmatched placeholders size");

            return in;
        }
        auto& read_header(std::istream& in)
        {
            uint32_t blksize1, blksize2;
            raw_header H;
            try
            {
                in.read(reinterpret_cast<char*>(&blksize1), sizeof(blksize1));
                in.read(reinterpret_cast<char*>(&H), sizeof(H));
                in.read(reinterpret_cast<char*>(&blksize2), sizeof(blksize2));
            }catch (std::exception & e)
            {
                std::cerr << e.what() << '\n';
                throw std::runtime_error("Unable to read HEAD block");
            }
            if (blksize1 != HDR_SIZE)
                throw std::runtime_error(
                    "This is not a Gadget's HEAD block < format = " +
                    std::to_string(format) + " >");
            if (blksize1 != blksize2)
                throw std::runtime_error("Missmatched placeholders size");

            this->header = snap_header(H);
            return in;
        }
    };

    template <unsigned int format>
    class isnapshot : public base_isnapshot<format>
    {
       public:
        using base_isnapshot<format>::base_isnapshot;  //(const char*);
    };
    template <unsigned int format>
    class osnapshot : public base_osnapshot<format>
    {
       public:
        using base_osnapshot<format>::base_osnapshot;  //(const char*);
    };

    template <>
    class isnapshot<1> : public base_isnapshot<1>
    {
       public:
        template <class iterator>
        auto& scan_block(const std::string label, iterator buff = nullptr)
        {
            // uint32_t blksize1, blksize2;
            if (label == "HEAD")
                this->read_header(in);
            else
                this->copy_block(in, buff);
            // this->skip_block(in);
            return in;
        }
        isnapshot(const char* _fname) : base_isnapshot(_fname)
        {
            // std::ifstream in(_fname, std::ios::binary);
            scan_block<int*>("HEAD");
            // scan_block(in, "POS");
            // scan_block(in, "VEL");
            // scan_block(in, "ID");
        }
    };
    template <>
    class isnapshot<2> : public base_isnapshot<2>
    {
       public:
        template <class iterator>
        auto& scan_block(const std::string req_label, iterator buff = nullptr)
        {
            blk_header hd;
            in.read(reinterpret_cast<char*>(&hd), sizeof(hd));
            if (hd.blksize1 != 8 or hd.blksize1 != hd.blksize2)
                throw std::runtime_error(
                    "This is not a Gadget's block header < format = " +
                    std::to_string(2) + " >");

            if (in.eof())
                return in;

            std::string label(hd.label, hd.label + 4);

            std::cout << "-- Block --\n"
                      << "Size: " << hd.blksize1 << '\n'
                      << "Label: " << label << '\n'
                      << "NextSize: " << hd.nextblk << '\n'
                      << std::endl;
            if (label != req_label)
                throw std::runtime_error(std::string("Block '") + req_label +
                                         std::string("' requested, but '") +
                                         label + std::string("' was found!"));

            if (label == "HEAD")
                this->read_header(in);
            else
            {
                this->copy_block(in, buff);
            }
            return in;
        }

        isnapshot(const char* _fname) : base_isnapshot(_fname)
        {
            // std::ifstream in(_fname, std::ios::binary);
            scan_block<int*>("HEAD");
            // while (scan_block(this->in));
        }
    };

    template <>
    class osnapshot<1> : public base_osnapshot<1>
    {
       public:
        osnapshot(const char* _fname) : base_osnapshot(_fname) {}
        template <class iterator>
        void write_block(const std::string label,
                         iterator begin,
                         iterator end) const
        {
            this->write_raw_block(out, begin, end);
        }
        void write_header() const { this->write_raw_header(out); }
    };
    template <>
    class osnapshot<2> : public base_osnapshot<2>
    {
        void write_label_block(const std::string label,
                               const uint32_t size) const
        {
            blk_header hd;
            hd.blksize1 = hd.blksize2 = 8;
            hd.nextblk = size + 8;
            assert(std::distance(label.begin(), label.end()) == 4);
            std::copy(label.begin(), label.end(), hd.label);
            out.write(reinterpret_cast<char*>(&hd), sizeof(hd));
        }

       public:
        osnapshot(const char* _fname) : base_osnapshot(_fname) {}
        template <class iterator>
        void write_block(const std::string label,
                         iterator begin,
                         iterator end) const
        {
            const size_t sz = sizeof(*begin);
            const uint32_t blksize = sz * std::distance(begin, end);
            write_label_block(label, blksize);
            out.write(reinterpret_cast<const char*>(&blksize), sizeof(blksize));
            for (; begin != end; ++begin)
            {
                auto& tmp = *begin;
                out.write(reinterpret_cast<char*>(&tmp), sz);
            }
            out.write(reinterpret_cast<const char*>(&blksize), sizeof(blksize));
        }
        void write_header() const
        {
            const uint32_t blksize = HDR_SIZE;
            write_label_block("HEAD", blksize);
            out.write(reinterpret_cast<const char*>(&blksize), sizeof(blksize));
            out.write(reinterpret_cast<const char*>(&this->get_raw_header()),
                      blksize);
            out.write(reinterpret_cast<const char*>(&blksize), sizeof(blksize));
        }
    };

}  // namespace gadget2
