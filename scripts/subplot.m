%% 1. Carga la señal
filename = 'prueba.wav'; 
[x, fs] = audioread(filename);

%% 2. Selecciona un segmento sonoro 
start_time = 0.75;  %Segundo donde empieza el segmento 
duration_ms = 30;  
N = round((duration_ms / 1000) * fs); %Muestras en 30 ms
start_sample = round(start_time * fs);

%Extrae el segmento
segment = x(start_sample : start_sample + N - 1);

%Crea eje de tiempo en milisegundos
t_axis = (0:N-1) / fs * 1000;

%% 3. Calcula Autocorrelación
% Usamos xcorr. 'r' tendrá tamaño 2*N-1 (lags negativos y positivos)
[r, lags] = xcorr(segment);

%Solo con la mitad positiva (lags >= 0)
mid_point = ceil(length(r)/2);
r_pos = r(mid_point:end);
lags_pos = lags(mid_point:end);

%Eje de lags en milisegundos
lags_ms = lags_pos / fs * 1000;

%% 4. Detecta el Pitch (Primer máximo secundario) 
%Ignora el pico en 0 
% Busca picos a partir de ~2ms (500Hz) para evitar ruido de alta frecuencia
min_lag_ms = 2; 
min_lag_idx = find(lags_ms > min_lag_ms, 1);

[max_val, loc_idx] = max(r_pos(min_lag_idx:end));
peak_idx = loc_idx + min_lag_idx - 1; %Índice real en el vector r_pos
pitch_period_ms = lags_ms(peak_idx);  %Periodo (ms)
pitch_freq = 1000 / pitch_period_ms;  %Frecuencia (Hz)

%% 5. Grafica (Subplots)
figure('Color', 'w');

%Subplot 1: Señal Temporal
subplot(2,1,1);
plot(t_axis, segment, 'b', 'LineWidth', 1.5); hold on;
grid on;
title(['Señal Temporal. Pitch detectado: ' num2str(pitch_freq, '%.1f') ' Hz']);
xlabel('Tiempo (ms)');
ylabel('Amplitud');
axis tight;

%Busca un pico local en la señal temporal para pintar la flecha
[~, i_p1] = max(segment(1:round(N/2)));
x1 = t_axis(i_p1);
x2 = x1 + pitch_period_ms;
y_arrow = max(segment) * 0.8;
%Dibuja una línea roja (representa T0)
line([x1, x2], [y_arrow, y_arrow], 'Color', 'r', 'LineWidth', 2);
text(x1 + (x2-x1)/2, y_arrow*1.1, 'T_0', 'Color', 'r', 'HorizontalAlignment', 'center');

%Subplot 2: Autocorrelación
subplot(2,1,2);
plot(lags_ms, r_pos, 'k', 'LineWidth', 1.5); hold on;
grid on;
title('Autocorrelación normalizada');
xlabel('Lag (ms)');
ylabel('Magnitud');
axis([0 duration_ms/2 min(r_pos) max(r_pos)*1.1]); %Zoom 

%Marca el máximo principal (Lag 0) y el secundario (Pitch)
plot(0, r_pos(1), 'ro', 'MarkerFaceColor', 'r'); % Origen
plot(pitch_period_ms, max_val, 'ro', 'MarkerFaceColor', 'r'); %Máximo secundario

%Línea vertical en el pitch
xline(pitch_period_ms, '--r', [num2str(pitch_period_ms, '%.2f') ' ms']);

sgtitle('Análisis de Pitch: Señal vs Autocorrelación');