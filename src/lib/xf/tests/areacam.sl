/****************************************************************************
 * areacam.sl
 *
 * Description:
 *   View a scene through a non-ideal lens specified as
 *   reference geometry for a clip-plane surface.
 *
 * Parameters:
 *   space - the name of the space that Pref/Eref are expressed in
 *   Pref - reference position of the lens
 *   Eref - optional eye reference geometry
 *   use_Eref - when nonzero, use the Eref geometry, otherwise just
 *          use the normal of the lens reference geometry
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 *
 ****************************************************************************/

surface
areacam (string space = "world";
	 varying point Pref = point -1e10;
	 varying point Eref = point -1e10;
	 uniform float use_Eref = 0;)
{
    varying point Pobj = transform ("object", Pref);
    varying point Pareacam = transform (space, "current", Pobj);
    varying vector Iareacam;
    if (use_Eref != 0.) {
	/* Look from the eye through the reference geometry */
	varying point Eobj = transform("object", Eref);
	varying point Eareacam = transform(space, "current", Eobj);
	Iareacam = Pareacam - Eareacam;
    } else {
	/* Look through the reference geometry from the visible side */
	Iareacam = -calculatenormal(Pareacam);
    }

    Oi = 1;
    Ci = trace(Pareacam, Iareacam);
}
