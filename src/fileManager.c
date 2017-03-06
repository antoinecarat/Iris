/**
 * @file fileManager.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief all file utilities
 */

 #include "fileManager.h"


datagram_t **prepare_file(char* project_name, char* file_path, 
                          transaction_t transaction, unsigned int version,
	                      char* user_name) 
{
	return;
}


void rebuild_file(char* project_name, char* file_path, datagram_t** tab) 
{

}

void create_dir(char* dir_path)
{
	//read/write/search permissions for owner and group, read/search for others. 
	mkdir(dir_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void rename_dir(char* dir_path)
{

}

void copy_dir(char* dir_path) 
{

}

void remove_dir(char* dir_path)
{
	
}