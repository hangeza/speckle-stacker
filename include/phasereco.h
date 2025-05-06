#pragma once

#include "array2.h"
#include "phasemap.h"
#include "global.h"
#include "constants.h"

namespace smip {

template <typename T>
class Bispectrum;
template <typename T>
class Array2;

template <typename T, typename U>
Array2<T> reconstruct_phases(const Bispectrum<U>& bispec,
    std::size_t xsize, std::size_t ysize,
    double reco_radius,
    PhaseMap* phasemap = nullptr);

void SMIP_PUBLIC NextRecoIndex(double& r, double& phi, int& i, int& j);

template <typename T, typename U>
void calc_phase(const Bispectrum<U>& bispec,
    Array2<T>& phases,
    PhaseMap& pm,
    DimVector<int, 2> w);

//********************
// implementation part
//********************

template <typename T, typename U>
Array2<T> reconstruct_phases(const Bispectrum<U>& bispec,
    std::size_t xsize, std::size_t ysize,
    double reco_radius,
    PhaseMap* phasemap)
{
    PhaseMap pm(xsize, ysize);
    Array2<T> phases(xsize, ysize);
    // Startwerte fuer Reko
    constexpr T init_phase { T { 1., 0. } };

    phases.at({ 0, 0 }) = init_phase;
    phases.at({ 1, 0 }) = init_phase;
    phases.at({ 0, 1 }) = init_phase;
    phases.at({ -1, 0 }) = std::conj(init_phase);
    phases.at({ 0, -1 }) = std::conj(init_phase);

    pm.at({ 0, 0 }) = { true, 1.0 };
    pm.at({ 1, 0 }) = { true, 1.0 };
    pm.at({ 0, 1 }) = { true, 1.0 };
    pm.at({ -1, 0 }) = { true, 1.0 };
    pm.at({ 0, -1 }) = { true, 1.0 };

    double r { 0. };
    double phi { 0. };
    DimVector<int, 2> pm_indices {};

    while (r <= reco_radius) {
        NextRecoIndex(r, phi, pm_indices[0], pm_indices[1]);
        if (pm.range().contains(pm_indices)) {
            if (!pm.at(pm_indices).flag) {
                calc_phase(bispec, phases, pm, pm_indices);
                //std::cout<<"calc_phase: i="<<i<<" j="<<j<<"\n";
            }
        }
    }
    if (phasemap != nullptr) {
        *phasemap = pm;
    }
    return phases;
}

template <typename T, typename U>
void calc_phase(const Bispectrum<U>& bispec,
    Array2<T>& phases,
    PhaseMap& pm,
    DimVector<int, 2> w)
{
    const Range<DimVector<int, 2>> bispec_u_range { bispec.min_indices()[std::slice(0, 2, 1)], bispec.max_indices()[std::slice(0, 2, 1)] };
    const Range<DimVector<int, 2>> bispec_v_range { bispec.min_indices()[std::slice(2, 2, 1)], bispec.max_indices()[std::slice(2, 2, 1)] };

    if (std::abs(w).sum() <= 1 || (!bispec_u_range.contains(w))) {
        return;
    }

    std::vector<T> phaselist {};

    for (auto u : pm.range()) {
        DimVector<int, 2> v { w - u };
        if (bispec_v_range.contains(v)
            && (pm.at(u).flag)
            && (pm.at(v).flag)) {
            T temp { bispec.get_element(DimVector<int>::merge(u, v)) };
            // std::cout<<"bispec["<<ux<<","<<uy<<","<<vx<<","<<vy<<"]="<<temp<<"\n";
            T ph { phases.at(u) };
            // std::cout<<"phase["<<ux<<","<<uy<<"]="<<ph<<"\n";
            ph *= phases.at(v);
            if (std::abs(temp) > constants::c_epsilon<double>) {
                temp /= abs(temp);
                temp = std::conj(temp);
                ph *= temp;
                phaselist.push_back(ph / std::abs(ph));
                // std::cout<<"phaselist entry="<<phaselist.back()<<std::endl;
            }
        }
    }

    T mean_phase { std::accumulate(phaselist.begin(), phaselist.end(), T {}, std::plus<T>()) };
    if (!phaselist.empty()) {
        pm.at(w).flag = true;
        mean_phase /= static_cast<double>(phaselist.size());
        //         std::cout<<"wx="<<wx<<" wy="<<wy<<" multipl="<<phaselist.size()<<" mean phase="<<mean_phase<<" consis="<<std::abs(mean_phase)<<std::endl;
        const double abs_phase { std::abs(mean_phase) };
        pm.at(w).consistency = abs_phase;
        if (abs_phase > constants::c_epsilon<double>) {
            phases.at(w) = mean_phase / abs_phase;
        } else {
            phases.at(w) = T { 0 };
        }
    }
}

} // namespace smip
