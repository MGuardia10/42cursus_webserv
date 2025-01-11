#include "../../include/methods.hpp"

/**
 * @brief Function to check the modes that an item/folder has
 * 
 * @param	path Path of the item
 * @param	data Struct where the item information is saved. If it
 * 					is null, no information is saved
 * @param	mode Mode to check on the item
 * 
 * @return The function return a pair of bools: The first value indicates
 * 			if the file exists; if it does, the second value indicates
 *			if has the mode of the parameter
 */
std::pair<bool, bool>	check_mode( std::string path, struct stat *data, mode_t mode )
{
	struct stat aux;

	if (!data)
		data = &aux;

	/* Get the file info */
	if ( stat( path.c_str(), data ) == -1 )
		return std::pair<bool, bool>(false, false);
	
	/* Check the mode */
	return std::pair<bool, bool>(true, (data->st_mode & mode) != 0);
}