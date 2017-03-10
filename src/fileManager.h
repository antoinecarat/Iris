/**
 * @file fileManager.h
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief all file utilities
**/
#ifndef __FILEMANAGER_H__
#define __FILEMANAGER_H__

#include <sys/stat.h>
#include <libgen.h>

#define DATASIZE 256
#define STRINGSIZE 256

/**
 * @enum    transaction_e
 * @brief   All transaction available in iris.
 */
enum transaction_e {
  CLONE,
  INIT,
  PULL,
  PUSH,
  MKDIR,  //Create this repository
  ADD,    //This file has been added
  DEL,    //This file has been removed
  MOD,    //This file has been modified
  VERSION //What is the latest version ?
};

typedef enum transaction_e transaction_t;

/*
 * @struct  datagram_st
 * @brief   Define datagram
 *
 * @var transaction Transaction type
 * @var project_name Name of the project concerned
 * @var user_name Name of the user sending datagram
 * @var version Current revision
 * @var file_path Path of the file concerned
 * @var datagram_number Id of this datagram
 * @var datagram_total Number of datagram concerning the file
 * @var data_length Size of the data
 * @var data Data, what else ?
 */
struct datagram_st {
  // infos
  transaction_t transaction;
  char project_name[STRINGSIZE];
  char user_name[STRINGSIZE];
  unsigned int version;
  char file_path[STRINGSIZE];
  // data
  unsigned int datagram_number;
  unsigned int datagram_total;
  unsigned int data_length;
  char data[DATASIZE];
};

typedef struct datagram_st datagram_t;

/**
 * @brief Serialize a datagram.
 *
 * @param Datagram to be serialized
 *
 * @return A string representing datagram.
 */
char* serialize(datagram_t* datagram);

/**
 * @brief Unserialize a datagram.
 *
 * @param String to be unserialized
 *
 * @return A datagram.
 */
datagram_t* unserialize(char * serial);


/**
 * @brief Prepare a file for sending by cutting it into datagrams.
 * pouet/prout/file.truc
 * @param project_name Name of the project. pouet
 * @param file_path Path of the file to be cut. prout/file.truc
 * @param transaction Transaction type.
 * @param version Version of the project.
 * @param user_name Name of the user.
 *
 * @return An array of datagrams.
 */
datagram_t **prepare_file(char* project_name, char* file_path, 
                          transaction_t transaction, unsigned int version,
                          char* user_name);

/**
 * @brief Build a file from an array of datagrams.
 *  
 * @param project_name Name of the project.
 * @param file_path Path of the file to be re-built.
 * @param tab Array of datagrams.
 */
void rebuild_file(char* project_name, char* file_path, datagram_t** tab);


/**
* @brief Create a directory.
*
* @param dir_path Name of the directory to be created. 
*/
void create_dir(char* dir_path);

/**
* @brief Rename a directory.
*
* @param dir_path Name of the directory to be renamed. 
*/
void rename_dir(char* dir_path);

/**
* @brief Copy a directory.
*
* @param dir_path Name of the directory to be copied. 
*/
void copy_dir(char* dir_path);

/**
* @brief Remove a directory.
*
* @param dir_path Name of the directory to be removed. 
*/
void remove_dir(char* dir_path);


#endif //__FILEMANAGER_H__