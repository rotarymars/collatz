#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

// Memoization map: number -> steps to reach 1
std::unordered_map<long long, int> memo;

// Calculate the number of steps for a number to reach 1 in the Collatz sequence
// Uses memoization to avoid redundant calculations
int collatz_steps(long long n) {
  if (n == 1) {
    return 0;
  }

  // Check if already computed
  auto it = memo.find(n);
  if (it != memo.end()) {
    return it->second;
  }

  // Calculate next value in sequence
  long long next;
  if (n % 2 == 0) {
    next = n / 2;
  } else {
    next = 3 * n + 1;
  }

  // Recursively compute and memoize
  int steps = 1 + collatz_steps(next);
  memo[n] = steps;
  return steps;
}

// Read existing data file and populate the memo map
// Returns the last calculated number
long long load_existing_data(const std::string &filename) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    return 0; // File doesn't exist, start from beginning
  }

  long long last_number = 0;
  long long num;
  int steps;

  while (infile >> num >> steps) {
    memo[num] = steps;
    last_number = num;
  }

  infile.close();
  std::cout << "Loaded " << memo.size() << " cached values from " << filename
            << "\n";
  return last_number;
}

// Display a rich progress bar with percentage, bar visualization, and ETA
void display_progress(long long current, long long start, long long total,
                      std::chrono::steady_clock::time_point start_time) {
  const int bar_width = 40;
  long long completed = current - start + 1;
  long long range = total - start + 1;
  double progress = static_cast<double>(completed) / range;

  // Calculate elapsed time and ETA
  auto now = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

  double eta_seconds = 0;
  if (progress > 0 && progress < 1.0) {
    eta_seconds = (elapsed / progress - elapsed) / 1000.0;
  }

  // Build the progress bar
  int filled = static_cast<int>(progress * bar_width);

  std::cout << "\r\033[K";  // Clear line
  std::cout << "  \033[1;36m[\033[0m";  // Cyan bracket

  for (int i = 0; i < bar_width; i++) {
    if (i < filled) {
      std::cout << "\033[1;32m█\033[0m";  // Green filled
    } else if (i == filled) {
      std::cout << "\033[1;33m▓\033[0m";  // Yellow current position
    } else {
      std::cout << "\033[0;90m░\033[0m";  // Dark gray empty
    }
  }

  std::cout << "\033[1;36m]\033[0m ";  // Cyan bracket

  // Percentage
  std::cout << "\033[1;37m" << std::fixed << std::setprecision(1)
            << std::setw(5) << (progress * 100) << "%\033[0m";

  // Current number / Total
  std::cout << "  \033[0;90m(" << current << "/" << total << ")\033[0m";

  // ETA
  if (eta_seconds > 0 && eta_seconds < 86400) {  // Less than a day
    int mins = static_cast<int>(eta_seconds) / 60;
    int secs = static_cast<int>(eta_seconds) % 60;
    std::cout << "  \033[0;35mETA: ";
    if (mins > 0) {
      std::cout << mins << "m ";
    }
    std::cout << secs << "s\033[0m";
  }

  std::cout << std::flush;
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

  // Load existing data into memo map and find where to continue from
  long long start_number = load_existing_data(datafilename) + 1;

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
              << " to " << max_number << "...\n\n";

    // Start timing for progress bar
    auto progress_start = std::chrono::steady_clock::now();

    // Calculate update frequency (update every 0.1% or at least every 100 numbers)
    long long range = max_number - start_number + 1;
    long long update_interval = std::max(1LL, std::min(range / 1000, 100LL));

    // Calculate steps for each number and append to file
    for (long long i = start_number; i <= max_number; i++) {
      int steps = collatz_steps(i);
      datafile << i << " " << steps << "\n";

      // Update progress bar periodically
      if ((i - start_number) % update_interval == 0 || i == max_number) {
        display_progress(i, start_number, max_number, progress_start);
      }
    }

    std::cout << "\n\n";  // Move past the progress bar
    datafile.close();
    std::cout << "✓ Data appended to " << datafilename << "\n";
    std::cout << "✓ Memo cache size: " << memo.size() << " entries\n";
  }

  std::cout << "\n";

  // Generate gnuplot script
  std::ofstream plotfile("plot_collatz.gp");
  if (!plotfile.is_open()) {
    std::cerr << "✗ Error: Could not create gnuplot script\n";
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
  std::cout << "✓ Gnuplot script written to plot_collatz.gp\n";

  // Run gnuplot
  std::cout << "→ Generating plot...\n";
  int result = std::system("gnuplot plot_collatz.gp");

  if (result == 0) {
    std::cout << "✓ Plot saved to collatz_plot.png\n";
  } else {
    std::cerr << "⚠ Warning: gnuplot command failed. Run manually: gnuplot "
                 "plot_collatz.gp\n";
  }

  return 0;
}
