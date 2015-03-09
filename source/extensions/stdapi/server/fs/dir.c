#include "precomp.h"

#include "fs_local.h"

void request_fs_ls_cb(void *arg, char *name, const char *dir)
{
	Packet *response = arg;
	char path[FS_MAX_PATH];
	struct meterp_stat s;

	/*
	 * Build the full path if we have a base directory
	 */
	if (dir) {
		_snprintf(path, sizeof(path), "%s%c%s", dir, FS_SEPARATOR, name);
	} else {
		_snprintf(path, sizeof(path), "%s", name);
	}

	/*
	 * Add the file name, full path and stat information
	 */
	packet_add_tlv_string(response, TLV_TYPE_FILE_NAME, name);
	packet_add_tlv_string(response, TLV_TYPE_FILE_PATH, path);
	if (fs_stat(path, &s) >= 0) {
		packet_add_tlv_raw(response, TLV_TYPE_STAT_BUF, &s, sizeof(s));
	}
}

/*
 * Gets the contents of a given directory path and returns the list of file
 * names to the requestor.
 *
 * req: TLV_TYPE_DIRECTORY_PATH - The directory that should be listed
 */
DWORD request_fs_ls(Remote * remote, Packet * packet)
{
	Packet *response = packet_create_response(packet);
	LPCSTR directory = packet_get_tlv_value_string(packet, TLV_TYPE_DIRECTORY_PATH);
	DWORD result;

	if (!directory) {
		result = ERROR_INVALID_PARAMETER;
	} else {
		result = fs_ls(directory, request_fs_ls_cb, response);
	}

	packet_add_tlv_uint(response, TLV_TYPE_RESULT, result);
	return packet_transmit(remote, response, NULL);
}

/*
 * Gets the current working directory
 *
 * req: TLV_TYPE_DIRECTORY_PATH - The directory path to change the working
 *                                directory to.
 */
DWORD request_fs_getwd(Remote * remote, Packet * packet)
{
	Packet *response = packet_create_response(packet);
	char directory[FS_MAX_PATH];
	DWORD result;

	result = fs_getwd(directory, sizeof(directory));

	packet_add_tlv_string(response, TLV_TYPE_DIRECTORY_PATH, directory);

	packet_add_tlv_uint(response, TLV_TYPE_RESULT, result);
	return packet_transmit(remote, response, NULL);
}

/*
 * Changes the working directory of the process
 *
 * req: TLV_TYPE_DIRECTORY_PATH - The directory path to change the working
 *                                directory to.
 */
DWORD request_fs_chdir(Remote * remote, Packet * packet)
{
	Packet *response = packet_create_response(packet);
	LPCSTR directory;
	DWORD result;
	directory = packet_get_tlv_value_string(packet, TLV_TYPE_DIRECTORY_PATH);

	if (directory == NULL) {
		result = ERROR_INVALID_PARAMETER;
	} else {
		result = fs_chdir(directory);
	}

	packet_add_tlv_uint(response, TLV_TYPE_RESULT, result);
	return packet_transmit(remote, response, NULL);
}

/*
 * Creates a new directory
 *
 * req: TLV_TYPE_DIRECTORY_PATH - The directory path to create.
 */
DWORD request_fs_mkdir(Remote * remote, Packet * packet)
{
	Packet *response = packet_create_response(packet);
	LPCSTR directory;
	DWORD result;
	directory = packet_get_tlv_value_string(packet, TLV_TYPE_DIRECTORY_PATH);

	if (directory == NULL) {
		result = ERROR_INVALID_PARAMETER;
	} else {
		result = fs_mkdir(directory);
	}

	packet_add_tlv_uint(response, TLV_TYPE_RESULT, result);
	return packet_transmit(remote, response, NULL);
}

/*
 * Removes the supplied directory from disk if it's empty
 *
 * req: TLV_TYPE_DIRECTORY_PATH - The directory that is to be removed.
 */
DWORD request_fs_delete_dir(Remote * remote, Packet * packet)
{
	Packet *response = packet_create_response(packet);
	LPCSTR directory;
	DWORD result;
	directory = packet_get_tlv_value_string(packet, TLV_TYPE_DIRECTORY_PATH);

	if (directory == NULL) {
		result = ERROR_INVALID_PARAMETER;
	} else {
		result = fs_delete_dir(directory);
	}

	packet_add_tlv_uint(response, TLV_TYPE_RESULT, result);
	return packet_transmit(remote, response, NULL);
}
