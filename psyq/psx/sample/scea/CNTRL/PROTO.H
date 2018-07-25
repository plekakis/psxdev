
/*****************************************************************************
 *	pad.c     		
 ****************************************************************************/

void main ( void );
void initialize_display ( void );
void load_textures ( void );
void make_pretty_colors ( Double_Buffer *theDB );
void read_controllers ( void );
void get_digital_direction ( Controller_Data *c , int buttondata );
void read_controller ( Controller_Data *c , unsigned char *buf , int port );
void draw_sprite ( Double_Buffer *theDB );
POLY_FT4 *get_sprite ( Double_Buffer *theDB , int x , int y , TIM_IMAGE *theTim );
void draw_analog ( Double_Buffer *theDB , int x , int y );
void draw_analog2 ( Double_Buffer *theDB , int x , int y );
void draw_analog3 ( Double_Buffer *theDB , int x , int y );
void draw_digital ( Double_Buffer *theDB , int x , int y );
void draw_psxpad ( Double_Buffer *theDB , int x , int y );
void draw_wheel ( Double_Buffer *theDB , int x , int y );
void draw_mouse ( Double_Buffer *theDB , int x , int y );
void draw_gun ( Double_Buffer *theDB , int x , int y );
void bounds_check ( int *xpos , int *ypos );
