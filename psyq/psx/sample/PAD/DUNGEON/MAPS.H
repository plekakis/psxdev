/*
 * $PSLibId: Run-time Library Release 4.4$
 */
extern void make_world_maps( int wldx, int wldy );
extern int calc_world_data( GsRVIEW2* pview, SVECTOR* pvect );
extern int calc_model_data( int n, int px, int py, int fact, int atrb );
extern void draw_maps_data( GsOT* ot );
extern void clear_view_point( int vpx, int vpy );
extern int get_world_maps( int wx, int wy );
extern int set_world_maps( int wx, int wy, int val );
