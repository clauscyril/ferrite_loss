#include <iostream>
#include <complex>
#include "mfem.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp" 
#include "solver_TD.hpp"

#include "signals.hpp"  // different waveformes (sine, triangle, square)


using namespace mfem;


int main(int argc, char *argv[]) {
    // Default parameters
    std::string path = "../../../mesh/square.msh";             // Path to the mesh
    std::string source = "b";
    std::string waveform_option = "sine";
    std::string nb_period_option = "3";
    std::string nb_iter_by_period = "100";
    std::string f_value = "500e3";
    std::string peak_option = "10";
    std::string material = "5.98e-2 4.44e-1, 2.48e-6";
    std::string mu = "4300 1.8e6";
    std::string ni_option = "0";
    int viz = 0;


    OptionsParser args(argc, argv);
    args.AddOption(&path, "-p", "--path", "Path of the mesh file");
    args.AddOption(&source, "-s", "--source", "source imposed (b (mT), NI (A.turns) or H (A/m))");
    args.AddOption(&waveform_option, "-w", "--waveform", "waveform of the signal (sine, triangle or square)");
    args.AddOption(&nb_period_option, "-np", "--Number_Period", "Number of period of the signal");
    args.AddOption(&nb_iter_by_period, "-ni", "--Number_Iteration", "Number of iterations by period");
    args.AddOption(&f_value, "-f", "--Frequency", "Frequency of the input flux / current");
    args.AddOption(&peak_option, "-pk", "--peak", "peak value of the signal");
    args.AddOption(&material, "-d", "--material", "Properties of the ferrite (pho, sigma, epsilon)");
    args.AddOption(&mu, "-u", "--permeability", "Properties of the permeability (mu_DC, fc)");
    args.AddOption(&viz, "-v", "--visualization", "0 : visualization disabled, 1 : enabled using GLVis");
    args.ParseCheck();


    std::vector<std::string> ferrite_param;
    boost::split(ferrite_param, material, boost::is_any_of(" ,;"), boost::algorithm::token_compress_on); 
    
    std::vector<std::string> mu_param;
    boost::split(mu_param, mu, boost::is_any_of(" ,;"), boost::algorithm::token_compress_on); 

 
    Mesh *mesh = new Mesh(path, 1, 1);
    mesh->UniformRefinement();
    // mesh->UniformRefinement();

    Ferrite ferrite("Custom ferrite", std::stod(ferrite_param[0]), std::stod(ferrite_param[1]),
                    std::stod(ferrite_param[2]), std::stod(mu_param[0]), std::stod(mu_param[1]));


    real_t height = 7.59e-3;
    real_t w = 5.3e-3;


    real_t f = std::stod(f_value);
    real_t T = 1./f;
    int nb_period = std::stoi(nb_period_option);
    int nb_iter_tot = std::stoi(nb_iter_by_period) * nb_period; 
    real_t Ts = nb_period/f/nb_iter_tot;

    real_t peak = std::stod(peak_option);
    

    std::function<real_t(real_t)> function;

    if (waveform_option == "sine") {
        function = [&](real_t t) {
            return sine_func(t, peak, f);
        };
    } 
    else if (waveform_option == "triangle") {
        function = [&](real_t t) {
            return triangle_func(t, peak, f);
        };
    } 
    else if (waveform_option == "square") {
        function = [&](real_t t) {
            return square_func(t, peak, f);
        };
    } 
    else {
        std::cout << "Waveform invalid ! Please select between : sine, triangle or square" << std::endl;
        return 1;
    }


    if (source == "NI" | source == "ni") {
        TD_sim(mesh, function, Ts, nb_iter_tot, ferrite, viz);
    } else if (source == "b" |source == "B") {
        peak *= height*w*1e-3;  // The imposed value is the flux and the input is in mT
        TD_sim_by_flux(mesh, function, Ts, nb_iter_tot, ferrite, viz);
    } else if (source == "H" | source == "h") {
        peak *=  height*w;
        TD_sim_by_fluxH(mesh, function, Ts, nb_iter_tot, ferrite, viz);
    } else {
        std::cout << "Source invalid ! Please select between : b, H or NI" << std::endl;
        return 1;
    }

    std::cout << "Surface = " << height * w << std::endl;

    return 0;
}


