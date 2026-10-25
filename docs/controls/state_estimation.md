# Sensor Processing and State Estimation

## 1. Measurements

Typical mini-ROV measurements include accelerometer, gyroscope, magnetometer, pressure depth, leak state, voltage, current, temperature, and optional DVL, sonar, or camera-derived motion. Every sample requires timestamp, valid flag, expected rate, frame, unit, calibration revision, and covariance or uncertainty estimate.

## 2. Calibration model

A generic three-axis sensor model is

$$
y_m=S_a M_a y+b+n,
$$

where $S_a$ is scale, $M_a$ cross-axis and misalignment, $b$ bias, and $n$ noise. Corrected measurement is

$$
\hat y=M_a^{-1}S_a^{-1}(y_m-b).
$$

Temperature-dependent bias may be modeled as $b(T)$. Calibration must be versioned and linked to raw datasets.

## 3. Pressure depth

$$
h=\frac{p-p_0}{\rho g}.
$$

Estimate $p_0$ at the surface, reject implausible jumps, and avoid resetting the datum while submerged. Density uncertainty directly causes depth-scale uncertainty.

## 4. Complementary attitude concept

Gyroscopes provide short-term orientation change but drift; gravity and magnetic field provide long-term references but are disturbed. A conceptual complementary filter blends them:

$$
\hat\theta(s)=\frac{s}{s+\omega_c}\theta_{gyro}(s)+\frac{\omega_c}{s+\omega_c}\theta_{ref}(s).
$$

Magnetometers can be unreliable near power wiring, motors, steel, and high current. Detect disturbance and allow heading assistance to degrade safely.

## 5. Extended Kalman filter

For nonlinear state $x$, input $u$, and measurement $z$:

$$
x_{k+1}=f(x_k,u_k)+w_k,\qquad z_k=h(x_k)+v_k.
$$

Prediction:

$$
\hat x_k^-=f(\hat x_{k-1}^+,u_{k-1}),
\qquad P_k^-=F_kP_{k-1}^+F_k^T+Q_k.
$$

Update:

$$
S_k=H_kP_k^-H_k^T+R_k,
\quad K_k=P_k^-H_k^TS_k^{-1},
$$

$$
\hat x_k^+=\hat x_k^-+K_k(z_k-h(\hat x_k^-)),
\quad P_k^+=(I-K_kH_k)P_k^-.
$$

Use stable linear solves, preserve covariance symmetry, monitor innovation, and gate outliers using normalized innovation squared.

## 6. Observability warning

An IMU and pressure sensor do not generally provide bounded horizontal position or velocity over long durations. Do not claim station keeping without an external velocity or position reference. Treat unobservable states and drift explicitly in requirements and UI.

## 7. Health checks

Detect stale timestamps, frozen signals, impossible magnitude, saturation, rate violation, disagreement between redundant sources, excessive innovation, invalid quaternion, pressure discontinuity, and magnetometer disturbance. Fault response may hold last valid value briefly, degrade a mode, or disarm, but must be predetermined and tested.
