#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <cstring>
#include <cerrno>

std::string octalToString(int digit) {
    switch (digit) {
        case 7: return "rwx";
        case 6: return "rw-";
        case 5: return "r-x";
        case 4: return "r--";
        case 3: return "-wx";
        case 2: return "-w-";
        case 1: return "--x";
        case 0: return "---";
        default: return "Not in Range!";
    }
}

std::string getPermissions(unsigned int mode) {
     std::string perm = "";

    // Owner Permissions
    int owner = (mode >> 6) & 7;
    perm += octalToString(owner);

    // Group Permissions
    int group = (mode >> 3) & 7;
    perm += octalToString(group);

    // Other Permissions
    int other = mode & 7;
    perm += octalToString(other);

    return perm;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath>" << std::endl;
        return 1;
    }

    const char* filepath = argv[1];
    struct statx stx;

    //First Syscall UID And GID
    int ret = syscall(SYS_statx, AT_FDCWD, filepath, AT_SYMLINK_NOFOLLOW, STATX_UID | STATX_GID, &stx);
    if (ret != 0) {
     std::cerr << "Error statx (UID/GID): " << strerror(errno) << std::endl;
      return 1;
    }
    std::cout << "UID: " << stx.stx_uid << ", GID: " << stx.stx_gid << std::endl;

    //Second Syscall Size in Bytes
    ret = syscall(SYS_statx, AT_FDCWD, filepath, AT_SYMLINK_NOFOLLOW, STATX_SIZE, &stx);
    if (ret != 0) {
     std::cerr << "Error statx (Size): " << strerror(errno) << std::endl;
      return 1;
    }
    std::cout << "Size: " << stx.stx_size << std::endl;

    //Third Syscall Permissions
    ret = syscall(SYS_statx, AT_FDCWD, filepath, AT_SYMLINK_NOFOLLOW, STATX_MODE, &stx );
    if (ret != 0) {
     std::cerr << "Error statx (Mode): " << strerror(errno) << std::endl;
      return 1;
    }
    std::cout << getPermissions(stx.stx_mode) << std::endl;


    return 0;
}

