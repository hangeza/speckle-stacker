#include <errno.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <cmath>
#include <complex>
#include <fstream>

#include "multidimarray.h"
#include "phasemap.h"

PhaseMapElement::PhaseMapElement(bool a_flag)
    : flag(a_flag)
{}

PhaseMapElement::PhaseMapElement(bool a_flag, double a_consistency)
      :flag(a_flag),consistency(a_consistency)
{}

/*
PhaseMap::PhaseMap(std::size_t a_xsize, std::size_t a_ysize) 
    : Array<PhaseMapElement,2>(Array<PhaseMapElement,2>::extends{ a_xsize,a_ysize })
{}
*/
