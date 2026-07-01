/**
 * Decoder will handle decoding FLAC along with I2s.
 * Currently working soley on flac decoding then moving to I2s. 
 * Will be aiming for 32-bit bit depth.
 * 
 * hard code song decoding for now.
 */

void decode_song(const char* file);

/// @brief This function focuses on refilling the data after there are about 124 bytes left to decode
/// Make the left over data be pushed to the front.
/// @param dataToFill 
void refill(size_t dataToFill);


/// @brief This function focuses on the data being subtracted after decoding 
/// @param data the data length that is being subtracted from
/// @param bites the amount of bytes consumed while decoding.
void consume(size_t data, size_t bites);