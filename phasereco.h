#pragma once
#include "array2.h"
#include "phasemap.h"

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

void NextRecoIndex(double& r, double& phi, int& i, int& j);

template <typename T, typename U>
void calc_phase(const Bispectrum<U>& bispec,
    Array2<T>& phases,
    PhaseMap& pm,
    int wx, int wy);

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
    int i { 0 }, j { 0 };

    while (r <= reco_radius) {
        NextRecoIndex(r, phi, i, j);
        if (!((i < pm.min_sindices()[0])
                || (i > pm.max_sindices()[0])
                || (j < pm.min_sindices()[1])
                || (j > pm.max_sindices()[1]))) {
            //      if ((abs(complex_t(i,j))<=min(bispec->size1,bispec->size2)/2))
            if (!pm.at({ i, j }).flag) {
                calc_phase(bispec, phases, pm, i, j);
                //std::cout<<"calc_phase: i="<<i<<" j="<<j<<"\n";
            }
        }
    }
    //    phases.print();
    if (phasemap != nullptr) {
        *phasemap = pm;
    }
    return phases;
}

template <typename T, typename U>
void calc_phase(const Bispectrum<U>& bispec,
    Array2<T>& phases,
    PhaseMap& pm,
    int wx, int wy)
{
    constexpr double c_epsilon { 1e-25 };
    if (((wx == 0) && (wy == 0))
        || ((wx == 1) && (wy == 0))
        || ((wx == 0) && (wy == 1))
        || ((wx == -1) && (wy == 0))
        || ((wx == 0) && (wy == -1))
        || (wx < bispec.min_indices()[0])
        || (wx > bispec.max_indices()[0])
        || (wy < bispec.min_indices()[1])
        || (wy > bispec.max_indices()[1]))
        return;

    int x_lo = pm.min_sindices()[0];
    int y_lo = pm.min_sindices()[1];
    int x_hi = pm.max_sindices()[0];
    int y_hi = pm.max_sindices()[1];

    std::vector<T> phaselist;

    for (int ux = x_lo; ux <= x_hi; ux++) {
        for (int uy = y_lo; uy <= y_hi; uy++) {
            int vx = wx - ux;
            int vy = wy - uy;

            if ((vx >= bispec.min_indices()[2]) && (vx <= bispec.max_indices()[2])
                && (vy >= bispec.min_indices()[3]) && (vy <= bispec.max_indices()[3])
                && (pm.at({ ux, uy }).flag)
                && (pm.at({ vx, vy }).flag)) {
                T temp { bispec.get_element({ ux, uy, vx, vy }) };
                //                 std::cout<<"bispec["<<ux<<","<<uy<<","<<vx<<","<<vy<<"]="<<temp<<"\n";
                T ph { phases.at({ ux, uy }) };
                //                 std::cout<<"phase["<<ux<<","<<uy<<"]="<<ph<<"\n";
                ph *= phases.at({ vx, vy });
                if (std::abs(temp) > c_epsilon) {
                    temp /= abs(temp);
                    temp = std::conj(temp);
                    ph *= temp;
                    phaselist.push_back(ph / std::abs(ph));
                    //                     std::cout<<"phaselist entry="<<phaselist.back()<<std::endl;
                }
            }
        }
    }

    T mean_phase { std::accumulate(phaselist.begin(), phaselist.end(), T {}, std::plus<T>()) };
    if (!phaselist.empty()) {
        pm.at({ wx, wy }).flag = true;
        mean_phase /= static_cast<double>(phaselist.size());
        //         std::cout<<"wx="<<wx<<" wy="<<wy<<" multipl="<<phaselist.size()<<" mean phase="<<mean_phase<<" consis="<<std::abs(mean_phase)<<std::endl;
        const double abs_phase { std::abs(mean_phase) };
        pm.at({ wx, wy }).consistency = abs_phase;
        if (abs_phase > c_epsilon) {
            phases.at({ wx, wy }) = mean_phase / abs_phase;
        } else {
            phases.at({ wx, wy }) = T { 0 };
        }
    }
}

} // namespace smip
