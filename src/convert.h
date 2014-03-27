#ifndef __convert_h
#define __convert_h

extern int        *twrp;
extern int        twrap_n;
extern int        twrp_idx;

enum { xz_axis, xy_axis, yz_axis };

C_ShapeInfo *reflect(C_ShapeInfo &, int which_axis);
void reverse(C_ShapeInfo &);
void scale(C_ShapeInfo &, float factor, int = 0);
void move(C_ShapeInfo &, float delta_x, float delta_y, float delta_z);
void rotate(C_ShapeInfo &, float angle, Port_3D &ref_port);
void swap(C_ShapeInfo &, int w);
C_PolyInfo *axis4to8(C_PolyInfo *axis_poly);
C_PolyInfo *axis4to6(C_PolyInfo *axis_poly);
C_PolyInfo *axis4toN(C_PolyInfo *axis_poly, float ratios[][2]);
C_PolyInfo *axis2shape(C_PolyInfo &axis_1, C_PolyInfo &axis_to, int &n);
int convert1(C_ShapeInfo &);
int convert2(C_ShapeInfo &);
int convert3(int );
int convert4();
int convert5();
void reflect_axis();
void dupe_reverse();
int poly2shape(C_ShapeInfo &, int);
C_PolyInfo *cat_poly_infos(C_PolyInfo *cp1, int size1,
			   C_PolyInfo *cp2, int size2,
			   int &size, int del = 1);
void reverse_polyinfo(C_PolyInfo &ply);
void adjust_params(int i);
void port2world(char *outfile);
R_3DPoint step_y(R_3DPoint &p1, R_3DPoint &p2, REAL_TYPE y);
void measure_poly(C_PolyInfo &, bounding_cube &, REAL_TYPE *x, REAL_TYPE *y, REAL_TYPE *z);
C_PolyInfo *poly2pyramid(C_PolyInfo &cp, int *, REAL_TYPE f1 = 2.0,
			 REAL_TYPE f2 = 2.0);
C_PolyInfo *create_poly_info(int n, int npoints);
void tri2trap(C_PolyInfo &p, C_PolyInfo **p1, C_PolyInfo **p2, REAL_TYPE d);
C_PolyInfo *dupe_poly_info(C_PolyInfo &, REAL_TYPE scale = 1.0);
C_PolyInfo *contour2polys(C_PolyInfo &bot_c, C_PolyInfo &top_c, int &n);
void interpolate_x(C_PolyInfo &);
void interpolate_y(C_PolyInfo &);
void interpolate_z(C_PolyInfo &, int = 1);
void apply_texture(int, int);
void enumColors(ostream &os, TextrMap &tmap);
void dumpTxtrMaps();
void consolidatePolys();
C_ShapeInfo &centerShape(C_ShapeInfo &cs, R_3DPoint &cp);
C_PolyInfo &centerPoly(C_PolyInfo &cp, R_3DPoint &p);
void poly2xzPlane(C_PolyInfo &cp);
void apply_texture(int tmap, int n, int m);
int apply_texture(C_PolyInfo &, TextrMap &);
int apply_texture_xz(C_PolyInfo &, TextrMap &);
int apply_texture_xy(C_PolyInfo &, TextrMap &);
int apply_texture_yz(C_PolyInfo &, TextrMap &);
void adjust_texture_coords(int n, int m, REAL_TYPE ud, REAL_TYPE vd, int flg=0);
#endif




