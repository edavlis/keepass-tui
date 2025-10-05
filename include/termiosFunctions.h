void enable_raw_mode(struct termios *new_terminal_settings) ;
void reset_terminal(struct termios *old_terminal_settings) ;
void get_terminal_dimensions( int *width, int *height) ;
void moveCursor(int x, int y) ;
void clearscreen() ;
