/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <cmath>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -m, --umaxnorm FLOAT  Llindar de decisió sonor/sord per a rmaxnorm [defaul: 0.5]
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  float umaxnorm = stof(args["--umaxnorm"].asString());


  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::HAMMING, 50, 500, umaxnorm);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  

  float max_val = 0.0F;
  for (unsigned int i = 0; i < x.size(); ++i) {
    if (fabs(x[i]) > max_val)
      max_val = fabs(x[i]);
  }

  float Cl = 0.02F * max_val;

  for (unsigned int i = 0; i < x.size(); ++i) {
    if (x[i] >= Cl) {
      x[i] = x[i] - Cl;
    } else if (x[i] <= -Cl) {
      x[i] = x[i] + Cl;
    } else {
      x[i] = 0.0F;
    }
  }


  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  // --- INICIO FILTRO DE MEDIANA (L=3) ---
  // Solo aplicamos si tenemos suficientes muestras
  if (f0.size() > 2) {
      vector<float> f0_med = f0; // Copia para guardar resultados sin afectar la lectura
      
      // Iteramos desde el segundo elemento hasta el penúltimo
      for (unsigned int i = 1; i < f0.size() - 1; ++i) {
          // Extraemos la ventana local de 3 muestras
          float v1 = f0[i-1];
          float v2 = f0[i];
          float v3 = f0[i+1];

          // Ordenamos los 3 valores para encontrar el del medio (mediana)
          // Una forma sencilla sin arrays es comparar manualmente:
          float median;
          if ((v1 <= v2 && v2 <= v3) || (v3 <= v2 && v2 <= v1)) median = v2;
          else if ((v2 <= v1 && v1 <= v3) || (v3 <= v1 && v1 <= v2)) median = v1;
          else median = v3;

          f0_med[i] = median;
      }
      f0 = f0_med; // Sobrescribimos el vector original con el filtrado
  }
  // --- FIN FILTRO DE MEDIANA ---

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
