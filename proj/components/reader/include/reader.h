#include <string>

void initialize(void);
void list_trackslist(void);

//helper function to easily create a path that differs from playlists
//albumName is the string of the album that will be opened.
const char* path_finder(std::string albumName);
void seek_track(const char *trackName);
void run(void);
void clear_songs(void);



//I need a way to send this as a file to then send to get decoded. 
//I think i should also create 
void select_song(const char *song);