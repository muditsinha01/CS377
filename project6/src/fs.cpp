#include "fs.h"
#include <iostream>

using namespace std;

myFileSystem::myFileSystem(char diskName[16]) {
  // open the file with the above name
  // this file will act as the "disk" for your file system
  disk.open(diskName, fstream::in | fstream::out | fstream::binary);
}

int myFileSystem::create_file(char name[8], int size) {
  // create a file with this name and this size
  // Step 1: Check to see if we have sufficient free space on disk by
  //   reading in the free block list. To do this:
  // Move the file pointer to the start of the disk file.
  // Read the first 128 bytes (the free/in-use block information)
  // Scan the list to make sure you have sufficient free blocks to
  //   allocate a new file of this size
  if(size >8 || size < 1)
    return -1;
  disk.seekg(0, ios::beg);
  char freeList[128];
  disk.read(freeList, 128);
  int freeBlocks = 0;
  for(int i = 0; i < 128; i++){
    if(freeList[i] == 0)
      freeBlocks += 1;
  }
  if(freeBlocks < size)
    return -1;
  // Step 2: we look for a free inode on disk
  // Read in an inode
  // Check the "used" field to see if it is free
  // If not, repeat the above two steps until you find a free inode
  // Set the "used" field to 1
  // Copy the filename to the "name" field
  // Copy the file size (in units of blocks) to the "size" field
  idxNode nd;
  int index = -1;
  for(int i = 0; i < 16; i++){
    disk.seekg(i * sizeof(idxNode) + 128);
    disk.read((char*)&nd, sizeof(idxNode));
    if(nd.used == 0){
      nd.used = 1;
      index = i;
      break;
    }
  }
  if(index == -1)
    return -1;
  strcpy(nd.name, name);
  nd.size = size;
  // Step 3: Allocate data blocks to the file
  // for(i=0;i<size;i++)
  // Scan the block list that you read in Step 1 for a free block
  // Once you find a free block, mark it as in-use (Set it to 1)
  // Set the blockPointer[i] field in the inode to this block number.
  // end for
  int size_ = size;
  for(int i = 0; i < 128 && size_ > 0; i++){
    if(freeList[i] == 0){
      freeList[i] = 1;
      nd.blockPointers[size - size_] = i;
      size_--;
    }
  }

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  // Write out the 128 byte free block list
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode
  disk.seekp(0, ios::beg);
  disk.write(freeList, 128);
  disk.seekp(index * sizeof(idxNode) + 128);
  disk.write((char*)&nd, sizeof(idxNode));


  return 1;
}  // End Create

int myFileSystem::delete_file(char name[8]) {
  // Delete the file with this name
  // Step 1: Locate the inode for this file
  // Move the file pointer to the 1st inode (129th byte)
  // Read in an inode
  // If the inode is free, repeat above step.
  // If the inode is in use, check if the "name" field in the
  //   inode matches the file we want to delete. If not, read the next
  //   inode and repeat
  disk.seekg(0, ios::beg);
  idxNode nd;
  int index = -1;
  for(int i = 0; i < 16; i++){
    disk.seekg(i * sizeof(idxNode) + 128);
    disk.read((char*)&nd, sizeof(idxNode));
    if(nd.used == 1){
      if(strcmp(nd.name, name) == 0){
        index = i;
        break;
      }
    }
  }
  if(index == -1)
    return -1;

  // Step 2: free blocks of the file being deleted
  // Read in the 128 byte free block list (move file pointer to start
  //   of the disk and read in 128 bytes)
  // Free each block listed in the blockPointer fields as follows:
  // for(i=0;i< inode.size; i++)
  // freeblockList[ inode.blockPointer[i] ] = 0;
  char freeList[128];
  disk.seekg(0, ios::beg);
  disk.read(freeList, 128);
  for(int i = 0; i < nd.size; i++){
    freeList[nd.blockPointers[i]] = 0;
  }
  // Step 3: mark inode as free
  // Set the "used" field to 0.
  nd.used = 0;
  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  // Write out the 128 byte free block list
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode
  disk.seekp(0, ios::beg);
  disk.write(freeList, 128);
  disk.seekp(index * sizeof(idxNode) + 128);
  disk.write((char*)&nd, sizeof(idxNode));

  return 1;
}  // End Delete

int myFileSystem::ls() {
  // List names of all files on disk
  // Step 1: read in each inode and print
  // Move file pointer to the position of the 1st inode (129th byte)
  // for(i=0;i<16;i++)
  // Read in an inode
  // If the inode is in-use
  // print the "name" and "size" fields from the inode
  // end for
  idxNode nd;
  for(int i = 0; i < 16; i++){
    disk.seekg(i * sizeof(idxNode) + 128);
    disk.read((char*)&nd, sizeof(idxNode));
    if(nd.used){
      cout << nd.name << " " << nd.size << endl;
    }
  }
  return 1;
}  // End ls

int myFileSystem::read(char name[8], int blockNum, char buf[1024]) {
  // read this block from this file
  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in an inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat
  idxNode nd;
  int index = -1;
  for(int i = 0; i < 16; i++){
    disk.seekg(i * sizeof(idxNode) + 128);
    disk.read((char*)&nd, sizeof(idxNode));
    if(nd.used == 1){
      if(strcmp(nd.name, name) == 0){
        index = i;
        break;
      }
    }
  }
  if(index == -1)
    return -1;

  // Step 2: Read in the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // That is, addr = inode.blockPointer[blockNum]
  // Move the file pointer to the block location (i.e., to byte #
  //   addr*1024 in the file)
  disk.seekg(index * sizeof(idxNode) + 128);
  disk.read((char*)&nd, sizeof(idxNode));
  if(blockNum >= nd.size)
    return -1;


  // Read in the block => Read in 1024 bytes from this location
  //   into the buffer "buf"
  disk.seekg(1024 * nd.blockPointers[blockNum]);
  disk.read(buf, 1024);

  return 1;
}  // End read

int myFileSystem::write(char name[8], int blockNum, char buf[1024]) {
  // write this block to this file
  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in a inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat
  idxNode nd;
  int index = -1;
  for(int i = 0; i < 16; i++){
    disk.seekg(i * sizeof(idxNode) + 128);
    disk.read((char*)&nd, sizeof(idxNode));
    if(nd.used == 1){
      if(strcmp(nd.name, name) == 0){
        index = i;
        break;
      }
    }
  }
  if(index == -1)
    return -1;

  // Step 2: Write to the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // That is, addr = inode.blockPointer[blockNum]
  // Move the file pointer to the block location (i.e., byte # addr*1024)
  disk.seekg(index * sizeof(idxNode) + 128);
  disk.read((char*)&nd, sizeof(idxNode));
  if(blockNum >= nd.size)
    return -1;

  // Write the block! => Write 1024 bytes from the buffer "buff" to
  //   this location
  disk.seekp(1024 * nd.blockPointers[blockNum]);
  disk.write(buf, 1024);

  return 1;
}  // end write

int myFileSystem::close_disk() {
  // close the disk!
  disk.close();
  return 1;
}