#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

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

// Read the last number from existing data file
long long get_last_calculated_number(const std::string &filename) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    return 0; // File doesn't exist, start from beginning
  }

  long long last_number = 0;
  long long num;
  int steps;

  while (infile >> num >> steps) {
    last_number = num;
  }

  infile.close();
  return last_number;
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

  const std::string datafilename = "collatz_data.txt";

  // Find where to continue from
  long long start_number = get_last_calculated_number(datafilename) + 1;

  if (start_number > max_number) {
    std::cout << "Already calculated up to " << (start_number - 1)
              << ", nothing new to compute for max=" << max_number << "\n";
  } else {
    // Open file in append mode
    std::ofstream datafile(datafilename, std::ios::app);
    if (!datafile.is_open()) {
      std::cerr << "Error: Could not open data file for writing\n";
      return 1;
    }

    std::cout << "Calculating Collatz steps for numbers " << start_number
              << " to " << max_number << "...\n";

    // Calculate steps for each number and append to file
    for (long long i = start_number; i <= max_number; i++) {
      int steps = collatz_steps(i);
      datafile << i << " " << steps << "\n";
    }

    datafile.close();
    std::cout << "Data appended to " << datafilename << "\n";
  }

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
