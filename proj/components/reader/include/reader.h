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

/**
 * This function will update the path so that the songs can be read from the directory
 * @param path path to the directory.
 */
void update_selected_album(std::string path);

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



/**
 * This function will be used to return the path of the song that will be decoded
 * @param song a string of the filepath to the targeted FLAC file that will be decoded.
 * Avoid hard coding the song in and use songList[i] for desired FLAC file.
 */
std::string select_song(std::string song);