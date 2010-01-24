/* Extracted from RCskin.sl:
 *
 * Copyright (C) 2002, Rudy Cortes   rcortes AT hntb DOT com 
 * created  09/16/2002
 * 
 * This software is placed in the public domain and is provided as is 
 * without express or implied warranty. 
 */

float phase(vector v1, v2; float g) {
  float costheta = -v1 . v2;
  return (1. - g*g) / pow(1. + g*g - 2.*g*costheta, 1.5);
}

float singleScatter(vector wi, wo; normal n; float g, albedo, thickness) {
    float win = abs(wi . n);
    float won = abs(wo . n);

    return albedo * phase(wo, wi, g) / (win + won) *
    (1. - exp(-(1/win + 1/won) * thickness));
}

vector efresnel(vector II; normal NN; float eta; output float Kr, Kt;) {
    vector R, T;
    fresnel(II, NN, eta, Kr, Kt, R, T);
    Kr = smoothstep(0., .5, Kr);
    Kt = 1. - Kr;
    return normalize(T);
}

surface TestRudyCSkin(vector Vf=0;
                      normal Nn=0;
                      color skinColor=0, sheenColor=0;
                      float eta=0, thickness=0) 
{
  extern point P;
  float Kr, Kt, Kr2, Kt2;
  color C = 0;

  vector T = efresnel(-Vf, Nn, eta, Kr, Kt);
      
  illuminance(P, Nn, PI/2) {
      vector Ln = normalize(L);

      vector H = normalize(Ln + Vf);
      if (H . Nn > 0)
    C += Kr * sheenColor * Cl * (Ln . Nn) * pow(H . Nn, 4.);
      C += Kr * sheenColor * Cl * (Ln . Nn) * .2;

      vector T2 = efresnel(-Ln, Nn, eta, Kr2, Kt2);
      C += skinColor * Cl * (Ln . Nn) * Kt * Kt2 * 
    (singleScatter(T, T2, Nn, .8, .8, thickness) +
     singleScatter(T, T2, Nn, .3, .5, thickness) +
     singleScatter(T, T2, Nn, 0., .4, thickness));
  }
  Ci = C;
}
