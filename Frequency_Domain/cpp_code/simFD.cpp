#include <iostream>
#include <complex>
#include <vector>
#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"  // A bit overkill...
#include "mfem.hpp"

#include "solver.hpp"

#ifndef M_PI 
#define M_PI 3.14159265358979323846 
#endif

using namespace mfem;

// Ferrite N30("N30", 5.98e-2, 4.44e-1, 2.48e-6, 5111, 2277.94e3);
// Ferrite N87("N87", 4.24e-2, 1.48e-1, 2.68e-6, 2200, 1.8e6);
// Ferrite T38("T38", 4.04e-2, 1.07e1, 8.06e-6, 10000, 380e3);

int main(int argc, char *argv[]){
    // Default parameters
    std::string path = "../../../mesh/square.msh";             // Path to the mesh
    std::string f_value = "100e3";
    std::string material = "5.98e-2 4.44e-1, 2.48e-6";
    std::string mu = "4300 1.8e6";
    std::string b_option = "10";
    std::string ni_option = "0";
    int viz = 0;


    OptionsParser args(argc, argv);
    args.AddOption(&path, "-p", "--path", "Path of the mesh file");
    args.AddOption(&f_value, "-f", "--Frequency", "Frequency of the input flux / current");
    args.AddOption(&material, "-d", "--material", "Properties of the ferrite (pho, sigma, epsilon)");
    args.AddOption(&mu, "-u", "--permeability", "Properties of the permeability (mu_DC, fc)");
    args.AddOption(&b_option, "-b", "--b_peak", "Average of b peak (mT)");
    args.AddOption(&ni_option, "-ni", "--NI", "current in the coil (ampere-turns)");
    args.AddOption(&viz, "-v", "--visualization", "0 : visualization disabled, 1 : enabled using GLVis");
    args.ParseCheck();


    std::vector<std::string> ferrite_param;
    boost::split(ferrite_param, material, boost::is_any_of(" ,;"), boost::algorithm::token_compress_on); 
    
    std::vector<std::string> mu_param;
    boost::split(mu_param, mu, boost::is_any_of(" ,;"), boost::algorithm::token_compress_on); 

    Mesh *mesh = new Mesh(path, 1, 1);
    mesh->UniformRefinement();

    Ferrite ferrite("Custom ferrite", std::stod(ferrite_param[0]), std::stod(ferrite_param[1]),
                    std::stod(ferrite_param[2]), std::stod(mu_param[0]), std::stod(mu_param[1]));


    // parameters to be computed (will be passed as reference)
    real_t b_peak = std::stod(b_option)*1e-3;
    real_t NI=std::stod(ni_option);
    if (NI != 0.) b_peak = 0.;
    real_t f = std::stod(f_value);
    real_t PLoss_eddy, PLoss_mag;
    std::complex<real_t> flux(0,0);    
                               
    GetPowerLoss(mesh, f, b_peak, NI, ferrite, PLoss_eddy, PLoss_mag, flux, viz);   // Calling the functions that computes the power losses
    std::cout << f << ";" << PLoss_eddy << ";" << PLoss_mag << ";" << PLoss_eddy + PLoss_mag << ";" << flux.real() << ";" << flux.imag() << ";"<< NI << std::endl;

return 0;
}

