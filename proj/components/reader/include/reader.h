#include <string>

/**
 * initializes spi bus, sdspi,
 * mounts at /sdcard
 */
void initialize(void);

/**
 * registers all albums/playlists found within MOUNT
 * adds them to albumList
 */
void register_trackslist(void);


void display_album_list(void);
void display_queue(void);

//marked for removal.
//helper function to easily create a path that differs from playlists
//albumName is the string of the album that will be opened.
const char* path_finder(std::string albumName);

/**
 * Searches through an album and adds songs within it to a queue.
 * @param albumName name of the desired album
 * 
 * GOAL: figure out serial UI by using albumIndexes. 
 */
void seek_track(const char* albumName);
void run(void);
void clear_songs(void);



//I need a way to send this as a file to then send to get decoded.
//This will return as a file for song that will get decoded.
FILE* select_song(std::string song, std::string album);