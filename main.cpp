#include "fat32.h"
#include <iomanip>
#include <string>

short ReadSect (const wchar_t *_dsk,    // disk to access
				char *&_buff,         // buffer where sector will be stored
				unsigned int _nsect   // sector number, starting with 0
				)
{
	DWORD dwRead;
	HANDLE hDisk = CreateFile(_dsk, GENERIC_VALID_FLAGS, FILE_SHARE_VALID_FLAGS, 0, OPEN_EXISTING, 0, 0);
	if (hDisk == INVALID_HANDLE_VALUE) // this may happen if another program is already reading from disk
	{
		CloseHandle(hDisk);
		return 1;
	}
	SetFilePointer(hDisk, _nsect * 512, 0, FILE_BEGIN); // which sector to read

	ReadFile(hDisk, _buff, 512, &dwRead, 0);  // read sector
	CloseHandle(hDisk);
	return 0;
}

int main() {
	BS *bs;
	FS *fs;
	MAIN_ENTRY *root_entry;
	unsigned int number_of_FAT;
	unsigned int sectors_per_FAT;
	unsigned int sectors_per_cluster;
	unsigned int reserved_sectors;
	unsigned int hidden_sectors;
	unsigned int first_data_cluster;
	unsigned int first_free_cluster;
	char *buffer = new char[512];
	short read;
	wstring name(L"\\\\.\\F:");

	read = ReadSect(name.c_str(), buffer, 0);
	if (read == 0)
	{
		bs = (BS*)buffer;
		
		string OEM_name((char*)bs->OEM_name);
		OEM_name.resize(8);
		cout << "OEM name: " << OEM_name << endl;

		cout << "Bytes per sector: " << *(unsigned short*)bs->bytes_per_sector << endl;

		sectors_per_cluster = (unsigned int)bs->sectors_per_cluster;
		cout << "Sector per cluster: " << sectors_per_cluster << endl;

		reserved_sectors = *(unsigned short*)bs->reserved_sectors;
		cout << "Reserved sectors: " << reserved_sectors << endl;

		number_of_FAT = (unsigned int)bs->num_copies_of_FAT;
		cout << "Number copies of FAT: " << number_of_FAT << endl;

		cout << "Media descriptor: " << hex << uppercase << (short)bs->media_descriptor << dec << endl;
		cout << "Sectors per track: " << *(unsigned short*)bs->sectors_per_track << endl;
		cout << "Number of heads: " << *(unsigned short*)bs->number_of_heads << endl;

		hidden_sectors = *(unsigned int*)bs->number_of_hidden_sectors;
		cout << "Number of hidden sectors: " << hidden_sectors << endl;
		cout << "Number of sectors: " << *(unsigned int*)bs->number_of_sectors << endl;

		sectors_per_FAT = *(unsigned int*)bs->number_of_sectors_per_FAT;
		cout << "Number of sectors per FAT: " << sectors_per_FAT << endl;

		first_data_cluster = *(unsigned int*)bs->first_cluster_of_root;
		cout << "First cluster of root directory: " << first_data_cluster << endl;

		cout << "File system info sector: " << *(unsigned short*)bs->first_FS_sector << endl;
		cout << "Backup Boot Sector offset: " << *(unsigned short*)bs->first_backup_BS_sector << endl;
		cout << "Logical drive number: " << (short)bs->logical_drive_number << endl;
		cout << "Extended signature: " << (short)bs->extended_signature << endl;
		cout << "Serial number: " << hex << uppercase << *(unsigned int*)bs->serial_number << dec << endl;

		string volume_name((char*)bs->volume_name);
		volume_name.resize(11);
		cout << "Volume name: " << volume_name << endl;

		string FAT_name((char*)bs->FAT_name);
		FAT_name.resize(8);
		cout << "FAT name: " << FAT_name << endl;

		cout << "Boot record signature: " << hex << uppercase << *(unsigned short*)bs->boot_record_signature << dec << endl;
	}

	read = ReadSect(name.c_str(), buffer, 1);
	if (read == 0)
	{
		fs = (FS*)buffer;
		cout << "Number of free clusters: " << hex << uppercase << *(unsigned int*)fs->number_free_clusters << dec << "-" << *(unsigned int*)fs->number_free_clusters << endl;
		cout << "Most recently allocated cluster: " << hex << uppercase << *(unsigned int*)fs->most_recently_allocated_cluster << dec << "-" << *(unsigned int*)fs->most_recently_allocated_cluster << endl;
		cout << "Sector signature: " << hex << uppercase << *(unsigned int*)fs->sector_signature << dec << endl;
	}

	read = ReadSect(name.c_str(), buffer, reserved_sectors + (number_of_FAT * sectors_per_FAT));
	if (read == 0)
	{
		root_entry = (MAIN_ENTRY*)buffer;
		string disk_name((char*)root_entry->short_name);
		disk_name.resize(12);
		cout << "Disk name: " << disk_name << endl;
	}

	system("pause");

	return 0;
}