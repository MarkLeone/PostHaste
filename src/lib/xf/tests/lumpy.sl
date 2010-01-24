/************************************************************************
 * lumpy.sl -- displacement shader for simple lumps
 *
 * Description:
 *   Makes a surface appear lumpy.
 * 
 * Parameters:
 *   Km 	   	the amplitude of the lumps
 *   frequency  	the frequency of the lumps
 *   maxoctaves  	the number of fBm octaves for the lumps
 *   shadingspace       what space to shade in
 *   truedisp           when 1, true disp, when 0 just bump
 *
 * Author: Larry Gritz (lg AT larrygritz DOT com)
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 ************************************************************************/



#include "noises.h"
#include "displace.h"


displacement
lumpy ( float Km = 1, frequency = 1, maxoctaves = 6; 
	string shadingspace = "shader";
        float truedisp = 1;)
{
    point Pshad = transform (shadingspace, frequency*P);
    float dPshad = filterwidthp(Pshad);
    float magnitude = fBm (Pshad, dPshad, maxoctaves, 2, 0.5);
    N = Displace (normalize(N), shadingspace, Km*magnitude, truedisp);
}
