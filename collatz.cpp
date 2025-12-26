#include <cstdlib>
#include <fstream>
#include <iostream>

// Calculate the number of steps for a number to reach 1 in the Collatz sequence
int collatz_steps(long long n) {
  int steps = 0;
  while (n != 1) {
    if (n % 2 == 0) {
      n = n / 2;
    } else {
      n = 3 * n + 1;
    }
    steps++;
  }
  return steps;
}

int main(int argc, char *argv[]) {
  // Default range
  long long max_number = 1000;

  // Allow user to specify range via command line
  if (argc > 1) {
    max_number = std::atoll(argv[1]);
    if (max_number < 1) {
      std::cerr << "Error: Please provide a positive number\n";
      return 1;
    }
  }

  // Output data file for gnuplot
  std::ofstream datafile("collatz_data.txt");
  if (!datafile.is_open()) {
    std::cerr << "Error: Could not open data file for writing\n";
    return 1;
  }

  std::cout << "Calculating Collatz steps for numbers 1 to " << max_number
            << "...\n";

  // Calculate steps for each number and write to file
  for (long long i = 1; i <= max_number; i++) {
    int steps = collatz_steps(i);
    datafile << i << " " << steps << "\n";
  }

  datafile.close();
  std::cout << "Data written to collatz_data.txt\n";

  // Generate gnuplot script
  std::ofstream plotfile("plot_collatz.gp");
  if (!plotfile.is_open()) {
    std::cerr << "Error: Could not create gnuplot script\n";
    return 1;
  }

  plotfile << "set terminal png size 1200,800\n";
  plotfile << "set output 'collatz_plot.png'\n";
  plotfile << "set title 'Collatz Conjecture: Steps to Reach 1'\n";
  plotfile << "set xlabel 'Starting Number'\n";
  plotfile << "set ylabel 'Number of Steps'\n";
  plotfile << "set grid\n";
  plotfile << "set style data points\n";
  plotfile << "set pointsize 0.5\n";
  plotfile << "plot 'collatz_data.txt' using 1:2 with points pt 7 lc rgb "
              "'#3366cc' title 'Steps'\n";

  plotfile.close();
  std::cout << "Gnuplot script written to plot_collatz.gp\n";

  // Run gnuplot
  std::cout << "Generating plot...\n";
  int result = std::system("gnuplot plot_collatz.gp");

  if (result == 0) {
    std::cout << "Plot saved to collatz_plot.png\n";
  } else {
    std::cerr << "Warning: gnuplot command failed. Run manually: gnuplot "
                 "plot_collatz.gp\n";
  }

  return 0;
}
