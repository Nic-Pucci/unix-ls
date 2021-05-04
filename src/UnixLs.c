/* Nic Pucci
 * UnixLs.c IMPLEMENTATION
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <dirent.h>

#define PREMISSIONS_STRING_LENGTH 11 // including null terminated char at end, i.e. "drwxrwxrwx\0"
#define DATE_FORMAT_STRING_LENGTH 18

#define INODE_FIELD_WIDTH "7"
#define PERMISSIONS_FIELD_WIDTH "-10"
#define LINK_COUNT_FIELD_WIDTH "-2"
#define OWNER_FIELD_WIDTH "-8"
#define GROUP_FIELD_WIDTH "-8"
#define BYTE_SIZE_FIELD_WIDTH "7"
#define MOD_DATE_WIDTH "-18"
#define ENTRY_NAME_WIDTH "-18"

#define CURRENT_WORKING_DIR "./"

const char DUMMY_SYM_LINK_PERMISSIONS_STR [] = "lrwxrwxrwx\0";
const char INFO_FLAG [] = "-i";
const char LONG_FLAG [] = "-l";
const char INFO_LONG_FLAG [] = "-il";
const char LONG_INFO_FLAG [] = "-li";

const char ROOT_DIR_ENTRY_NAME [] = ".";
const char PARENT_ROOT_DIR_ENTRY_NAME [] = "..";  

const char *DATE_FORMAT_STRING = "%b %d %Y %H:%M"; // "mmm dd yyyy hh:mm" 

char *currentPath = "./";
int infoFlagEnabled = 0;
int longFlagEnabled = 0;

int StrEqual ( const char* str1 , const char* str2 ) {
	if ( !str1 || !str2 ) {
		return 0;
	}

	int comp = ( strcmp ( str1 , str2 ) == 0 );
	return comp;
}

void PrintEntry ( const struct dirent *dirEntry , const struct stat *fileStat , const char *fullFilePath ) {
	if ( !dirEntry ) {
		return;
	}
	
	if ( !fileStat ) {
		return;
	}

	if ( !fullFilePath ) {
		return;
	}

	// column 1 - print inode # (only if flag is enabled)
	if ( infoFlagEnabled ) {
		printf ( "%"INODE_FIELD_WIDTH"ld " , fileStat -> st_ino );
	}

	if ( !longFlagEnabled ) {
		// column 8 - print name of entry
		printf ( "%"ENTRY_NAME_WIDTH"s " , dirEntry -> d_name );
		printf ( "\n" );
		return;
	}

	// column 2 - entry's file mode/permissions
	char filePermissionsStr [ PREMISSIONS_STRING_LENGTH ];
	if ( dirEntry -> d_type == DT_LNK ) {
		strcpy ( filePermissionsStr , DUMMY_SYM_LINK_PERMISSIONS_STR );
	} 
	else {
		if ( dirEntry -> d_type == DT_DIR ) {
			filePermissionsStr [ 0 ] = 'd';
		} 
		else {
			filePermissionsStr [ 0 ] = '-';
		}

		// check user read
	    if ( fileStat -> st_mode & S_IRUSR ) {
	    	filePermissionsStr [ 1 ] = 'r';
	    } 
	    else { 
	    	filePermissionsStr [ 1 ] = '-'; 
	    }

	    // check user write
	    if ( fileStat -> st_mode & S_IWUSR ) {
	    	filePermissionsStr [ 2 ] = 'w';
	    } 
	    else { 
	    	filePermissionsStr [ 2 ] = '-'; 
	    }

	    // check user execute
	    if ( fileStat -> st_mode & S_IXUSR ) {
	    	filePermissionsStr [ 3 ] = 'x';
	    } 
	    else { 
	    	filePermissionsStr [ 3 ] = '-';
	    }
		
		// check group read
	    if ( fileStat -> st_mode & S_IRGRP ) {
	    	filePermissionsStr [ 4 ] = 'r';
	    } 
	    else { 
	    	filePermissionsStr [ 4 ] = '-'; 
	    }
		
		// check group write
	    if ( fileStat -> st_mode & S_IWGRP ) {
	    	filePermissionsStr [ 5 ] = 'w';
	    } 
	    else { 
	    	filePermissionsStr [ 5 ] = '-'; 
	    }
		
		// check group execute
	    if ( fileStat -> st_mode & S_IXGRP ) {
	    	filePermissionsStr [ 6 ] = 'x';
	    } 
	    else { 
	    	filePermissionsStr [ 6 ] = '-'; 
	    }
		
		// check all others read
	    if ( fileStat -> st_mode & S_IROTH ) {
	    	filePermissionsStr [ 7 ] = 'r';
	    } 
	    else { 
	    	filePermissionsStr [ 7 ] = '-'; 
	    }
		
		// check all others write
	    if ( fileStat -> st_mode & S_IWOTH ) {
	    	filePermissionsStr [ 8 ] = 'w';
	    } 
	    else { 
	    	filePermissionsStr [ 8 ] = '-'; 
	    }
		
		// check all others execute
	    if ( fileStat -> st_mode & S_IXOTH ) {
	    	filePermissionsStr [ 9 ] = 'x';
	    } 
	    else { 
	    	filePermissionsStr [ 9 ] = '-'; 
	    }

	    filePermissionsStr [ 10 ] = '\0';
	}

    printf ( "%"PERMISSIONS_FIELD_WIDTH"s " , filePermissionsStr );


	// column 3 - print entry's link count
	printf ( "%"LINK_COUNT_FIELD_WIDTH"ld " , fileStat -> st_nlink );

	// column 4 - print owner/user name
	struct passwd *pw = getpwuid ( fileStat -> st_uid );
	if ( pw ) {
		printf ( "%"OWNER_FIELD_WIDTH"s " , pw -> pw_name );
	} 
	else {
		printf ( "%"OWNER_FIELD_WIDTH"d " , fileStat -> st_uid );
	}
	
	// column 5 - print group name
	struct group *grp = getgrgid ( fileStat -> st_gid ); 
	if ( grp ) {
		printf ( "%"GROUP_FIELD_WIDTH"s " , grp -> gr_name );
	} 
	else {
		printf ( "%"GROUP_FIELD_WIDTH"d " , fileStat -> st_gid );
	}

	// column 6 - print size of entry in bytes
	if ( dirEntry -> d_type == DT_LNK ) {
		int buffSize = 30;
		char linkedFileName [ buffSize ];
		for ( int i = 0 ; i < buffSize ; i ++ ) {
			linkedFileName [ i ] = 0;
		}

		ssize_t numBytes = readlink ( fullFilePath , linkedFileName , buffSize );
		printf ( "%"BYTE_SIZE_FIELD_WIDTH"ld " , numBytes );
	}
	else {
		printf ( "%"BYTE_SIZE_FIELD_WIDTH"ld " , fileStat -> st_size );
	}

	// column 7 - print formatted string of the entry's last modification date
	char formattedDate [ DATE_FORMAT_STRING_LENGTH ];
	strftime ( 
		formattedDate , 
		DATE_FORMAT_STRING_LENGTH , 
		DATE_FORMAT_STRING, 
		localtime ( &fileStat -> st_mtime ) 
	);
	printf ( "%"MOD_DATE_WIDTH"s " , formattedDate );

	// column 8 - print name of entry
	printf ( "%"ENTRY_NAME_WIDTH"s " , dirEntry -> d_name );
	if ( dirEntry -> d_type == DT_LNK ) {

		int buffSize = 30;
		char linkedFileName [ buffSize ];
		for ( int i = 0 ; i < buffSize ; i ++ ) {
			linkedFileName [ i ] = 0;
		}
		
		readlink ( fullFilePath , linkedFileName , buffSize - 2 );
		printf ( "-> %s" , linkedFileName );
	}
	else {
		
	}

	printf ( "\n" );
}

void PrintDirectory () {
	DIR *dir = opendir ( currentPath ); 
    if ( dir == NULL ) 
    { 
        printf ( "ERROR: Directory could not be found: %s\n" , currentPath ); 
        return; 
    } 

    printf ( "\n%s:\n" , currentPath );
  
    struct dirent *dirEntry;
    while ( ( dirEntry = readdir ( dir ) ) != NULL ) {
    	int ignoreEntry = StrEqual ( dirEntry -> d_name , ROOT_DIR_ENTRY_NAME )
    		|| StrEqual ( dirEntry -> d_name , PARENT_ROOT_DIR_ENTRY_NAME )
    		|| dirEntry -> d_name [ 0 ] == '.';

    	if ( ignoreEntry ) {
    		continue;
    	}

    	int pathSize = strlen ( currentPath ) + strlen ( dirEntry -> d_name );
    	char fullFilePath [ pathSize + 2 ]; // +2 because of the extra slash and the null terminated char at end
    	fullFilePath [ 0 ] = '\0';
    	strcat ( fullFilePath , currentPath );
    	strcat ( fullFilePath , "/" );
    	strcat ( fullFilePath , dirEntry -> d_name );

		struct stat fileStat;
		int exists = stat ( fullFilePath , &fileStat );
		if ( exists < 0 ) {
			printf ( "ERROR: could not get stats for dir entry: %s\n" , fullFilePath );
			continue;
		}

		PrintEntry ( dirEntry , &fileStat , fullFilePath );
    }
  
    closedir ( dir ); 
}

int main ( int argc , char *argv [] ) 
{
	int foundAPath = 0;
	for ( int i = 1 ; i < argc; i++ ) {
		if ( !foundAPath && StrEqual ( argv [ i ] , INFO_FLAG ) ) {
			infoFlagEnabled = 1;
		}

		else if ( !foundAPath && StrEqual ( argv [ i ] , LONG_FLAG ) ) {
			longFlagEnabled = 1;
		}

		else if ( !foundAPath && StrEqual ( argv [ i ] , LONG_INFO_FLAG ) ) {
			infoFlagEnabled = 1;
			longFlagEnabled = 1;
		}

		else if ( !foundAPath && StrEqual ( argv [ i ] , INFO_LONG_FLAG ) ) {
			infoFlagEnabled = 1;
			longFlagEnabled = 1;
		}

		else {
			foundAPath = 1;
			currentPath = argv [ i ];
			PrintDirectory ();
		}
	}

	if ( !foundAPath ) {
		PrintDirectory ();
	}
    
	return 0; 
}

