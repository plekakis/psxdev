/***********************************************************************/
/***********************************************************************/
/* effects.h */
#ifndef EFFECTS_HEADER
#define EFFECTS_HEADER

void init_distortion_cells(void);
void update_distortion_cells(unsigned long** poly_p,int frame);
void draw_distortion_cell(unsigned long** poly_p,int frame,short x_pos,short y_pos,short x_dist,short y_dist,short distortion_size,CVECTOR* col);
  
#endif

