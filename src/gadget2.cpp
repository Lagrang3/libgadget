#include "gadget2.hpp"

namespace gadget2
{
    std::ostream& operator<<(std::ostream& os, const snap_header& H)
    {
        int colsize = 16;
        std::stringstream hline;

        hline << std::right << "+ " << std::setw(3 * 2 + 4 * colsize)
              << std::setfill('-') << ""
              << "+\n";

        os << hline.str() << std::left << std::setfill(' ') << "| "
           << std::setw(colsize) << "type"
           << "| " << std::setw(colsize) << "npart"
           << "| " << std::setw(colsize) << "mass"
           << "| " << std::setw(colsize) << "npartTotal"
           << "|\n"
           << hline.str();

        for (int i = 0; i < 6; ++i)
        {
            os << std::setfill(' ') << std::left << "| " << std::setw(colsize)
               << i << "| " << std::setw(colsize) << H._data.npart[i] << "| "
               << std::setw(colsize) << H._data.mass[i] << "| "
               << std::setw(colsize) << H._data.npartTotal[i] << "|\n";
        }
        os << hline.str();

        os << std::left << std::setfill(' ') << std::setw(colsize)
           << "time:" << H._data.time << '\n'
           << std::setw(colsize) << "redshift:" << H._data.redshift << '\n'
           << std::setw(colsize) << "BoxSize:" << H._data.BoxSize << '\n'
           << std::setw(colsize) << "Omega0:" << H._data.Omega0 << '\n'
           << std::setw(colsize) << "OmegaLambda:" << H._data.OmegaLambda
           << '\n'
           << std::setw(colsize) << "HubbleParam:" << H._data.HubbleParam
           << '\n'
           << std::setw(colsize) << "num_files:" << H._data.num_files << '\n'
           << std::setw(colsize) << "flag_sfr:" << H._data.flag_sfr << '\n'
           << std::setw(colsize) << "flag_feedback:" << H._data.flag_feedback
           << '\n'
           << std::setw(colsize) << "flag_cooling:" << H._data.flag_cooling
           << '\n';
        return os;
    }

}  // namespace gadget2
