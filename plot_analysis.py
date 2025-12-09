import matplotlib.pyplot as plt
import numpy as np
import sys

def plot_features(filename):
    try:
        # Cargar datos: Col 0: Pot, Col 1: r1norm, Col 2: rmaxnorm, Col 3: lag
        data = np.loadtxt(filename)
    except Exception as e:
        print(f"Error cargando {filename}: {e}")
        return

    pot = data[:, 0]
    r1norm = data[:, 1]
    rmaxnorm = data[:, 2]
    lag = data[:, 3]
    
    # Eje X en frames
    frames = np.arange(len(pot))

    # Configuración de la figura con 3 subplots compartiendo eje X
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, sharex=True, figsize=(10, 8))
    plt.subplots_adjust(hspace=0.4)

    # 1. Gráfica de Estimación de Pitch (Lag)
    ax1.plot(frames, lag, label='Pitch Candidate (Lag)', color='black')
    ax1.set_ylabel('Lag (muestras)')
    ax1.set_title('Estimación de Pitch (Candidato)')
    ax1.grid(True)

    # 2. Gráfica de Potencia
    ax2.plot(frames, pot, label='Potencia (dB)', color='blue')
    ax2.set_ylabel('Potencia (dB)')
    ax2.set_title('Nivel de Potencia (r[0])')
    ax2.grid(True)

    # 3. Gráfica de Coeficientes de Autocorrelación
    ax3.plot(frames, r1norm, label='r1norm (r[1]/r[0])', color='green', alpha=0.6)
    ax3.plot(frames, rmaxnorm, label='rmaxnorm (r[lag]/r[0])', color='red', alpha=0.8)
    
    # Líneas guía para ayudar a decidir umbrales
    ax3.axhline(y=0.5, color='gray', linestyle='--', linewidth=0.8)
    
    ax3.set_ylabel('Correlación Norm.')
    ax3.set_xlabel('Número de Frame')
    ax3.set_title('Indicadores de Sonoridad (Voiced/Unvoiced)')
    ax3.legend()
    ax3.grid(True)
    ax3.set_ylim(-1, 1.1)

    print("Mostrando gráfica. Cierre la ventana para terminar.")
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Uso: python3 plot_analysis.py features.txt")
    else:
        plot_features(sys.argv[1])