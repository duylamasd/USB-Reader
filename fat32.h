#pragma once

#include <iostream>
#include <Windows.h>

#define FILE_SHARE_VALID_FLAGS (FILE_SHARE_READ | FILE_SHARE_WRITE)
#define GENERIC_VALID_FLAGS (GENERIC_READ | GENERIC_WRITE)

using namespace std;

// Boot sector structure
typedef struct BS {
	BYTE jump_code[3];
	BYTE OEM_name[8];
	BYTE bytes_per_sector[2];
	BYTE sectors_per_cluster;
	BYTE reserved_sectors[2];
	BYTE num_copies_of_FAT;
	BYTE no_answer_0[2];
	BYTE no_answer_1[2];
	BYTE media_descriptor;
	BYTE no_answer_2[2];
	BYTE sectors_per_track[2];
	BYTE number_of_heads[2];
	BYTE number_of_hidden_sectors[4];
	BYTE number_of_sectors[4];
	BYTE number_of_sectors_per_FAT[4];
	BYTE flags[2];
	BYTE version[2];
	BYTE first_cluster_of_root[4];
	BYTE first_FS_sector[2];
	BYTE first_backup_BS_sector[2];
	BYTE reserved[12];
	BYTE logical_drive_number;
	BYTE unused;
	BYTE extended_signature;
	BYTE serial_number[4];
	BYTE volume_name[11];
	BYTE FAT_name[8];
	BYTE executable_code[420];
	BYTE boot_record_signature[2];
};

// File system infomation
typedef struct FS {
	BYTE first_signature[4];
	BYTE first_reserved[480];
	BYTE second_signature[4];
	BYTE number_free_clusters[4];
	BYTE most_recently_allocated_cluster[4];
	BYTE second_reserved[12];
	BYTE sector_signature[4];
};

// Entry structure
typedef struct MAIN_ENTRY {
	BYTE short_name[8];
	BYTE short_extension[3];
	BYTE attributes;
	BYTE DOS_user_attributes;
	BYTE create_time_ms;
	BYTE create_time[2];
	BYTE create_date[2];
	BYTE last_access_date[2];
	BYTE high_bytes_first_cluster[2];
	BYTE last_modified_time[2];
	BYTE last_modified_date[2];
	BYTE low_bytes_first_cluster[2];
	BYTE sizes[4];
};