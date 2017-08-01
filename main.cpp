#include "fat32.h"
#include <iomanip>
#include <string>

short ReadSect(const wchar_t *_dsk,    // disk to access
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

short ReadEntry(const wchar_t *_dsk,    // disk to access
	char *&_buff,         // buffer where entry will be stored
	LONGLONG _byte_offset   // byte number, starting with 0
)
{
	DWORD dwRead;
	HANDLE hDisk = CreateFile(_dsk, GENERIC_VALID_FLAGS, FILE_SHARE_VALID_FLAGS, 0, OPEN_EXISTING, 0, 0);
	if (hDisk == INVALID_HANDLE_VALUE) // this may happen if another program is already reading from disk
	{
		CloseHandle(hDisk);
		return 1;
	}

	_LARGE_INTEGER large_byte_offset;
	large_byte_offset.QuadPart = _byte_offset;
	SetFilePointerEx(hDisk, large_byte_offset, nullptr, FILE_BEGIN);

	ReadFile(hDisk, _buff, 512, &dwRead, 0);
	CloseHandle(hDisk);
	return 0;
}

int main() {
	BS *bs;
	FS *fs;
	MAIN_ENTRY *root_entry;
	unsigned short bytes_per_sector;
	unsigned int number_of_FAT;
	unsigned int sectors_per_FAT;
	unsigned int sectors_per_cluster;
	unsigned int reserved_sectors;
	unsigned int hidden_sectors;
	unsigned int first_data_cluster;
	unsigned int first_free_cluster;
	char *buffer = new char[512];
	short read;
	wstring name(L"\\\\.\\G:");

	read = ReadSect(name.c_str(), buffer, 0);
	if (read == 0)
	{
		bs = (BS*)buffer;

		string OEM_name((char*)bs->OEM_name);
		OEM_name.resize(8);
		cout << "OEM name: " << OEM_name << endl;

		bytes_per_sector = *(unsigned short*)bs->bytes_per_sector;
		cout << "Bytes per sector: " << bytes_per_sector << endl;

		sectors_per_cluster = (unsigned int)bs->sectors_per_cluster;
		cout << "Sectors per cluster: " << sectors_per_cluster << endl;

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

	unsigned int first_data_sector = reserved_sectors + (number_of_FAT * sectors_per_FAT);
	long long current_byte = first_data_sector * bytes_per_sector;
	
	char *entry = new char[512];
	short read_entry = ReadEntry(name.c_str(), entry, current_byte);
	if (read_entry == 0)
	{
		root_entry = (MAIN_ENTRY*)entry;
		string disk_name((char*)(root_entry->short_name));
		disk_name.resize(12);
		cout << "Disk name: " << disk_name << endl;
	}

	current_byte += 96;
	read_entry = ReadEntry(name.c_str(), entry, current_byte);
	MAIN_ENTRY *sys_entry = (MAIN_ENTRY*)entry;
	string sys_name((char*)sys_entry->short_name);
	sys_name.resize(8);
	cout << "SYS: " << sys_name << endl;

	/*while (read_entry == 0 && entry[0] != 0x00)
	{
		read_entry = ReadEntry(name.c_str(), entry, current_byte);
		if (entry[11] != 0x0F)
		{
			MAIN_ENTRY *file_main_entry = (MAIN_ENTRY*)entry;
			string file_name((char*)file_main_entry->short_name);
			file_name.resize(8);
			cout << "Short file name: " << file_name << endl;

			unsigned long lfn_current_byte = current_byte - 32;
			char *lfn_buffer = new char[512];
			short read_lfn = ReadEntry(name.c_str(), lfn_buffer, lfn_current_byte);
			if (read_lfn == 0)
			{
				if (lfn_buffer[11] == 0x0F)
				{
					wstring long_file_name = L"";
					while (lfn_buffer[11] == 0x0F)
					{
						LFN_ENTRY *lfn_entry = (LFN_ENTRY*)lfn_buffer;
						wstring name_part;
						name_part = wstring((wchar_t*)lfn_entry->name_characters_part0);
						name_part.resize(5);
						long_file_name += name_part;

						name_part = wstring((wchar_t*)lfn_entry->name_characters_part1);
						name_part.resize(6);
						long_file_name += name_part;

						name_part = wstring((wchar_t*)lfn_entry->name_characters_part2);
						name_part.resize(2);
						long_file_name += name_part;

						lfn_current_byte -= 32;
					}

					cout << "Long file name: ";
					wcout << long_file_name << endl;
				}
			}

			char file_attributes = (char)file_main_entry->attributes;
			if ((file_attributes >> 4) % 2 == 1)
			{
				cout << "File type: Folder" << endl;
			}
			else
			{
				string extension((char*)file_main_entry->short_extension);
				extension.resize(3);
				cout << "File type: " << extension << endl;
			}

			BYTE first_cluster[4];
			first_cluster[0] = file_main_entry->low_bytes_first_cluster[0];
			first_cluster[1] = file_main_entry->low_bytes_first_cluster[1];
			first_cluster[2] = file_main_entry->high_bytes_first_cluster[0];
			first_cluster[3] = file_main_entry->high_bytes_first_cluster[1];
			unsigned int first_cluster_value = *(unsigned int*)first_cluster;
			cout << "First data cluster: " << first_cluster_value << endl;

			unsigned int file_size = *(unsigned int*)file_main_entry->sizes;
			cout << "File size: " << file_size << endl;
		}

		current_byte += 32;
	}*/

	system("pause");


	return 0;
}