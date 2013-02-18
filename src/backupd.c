#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/inotify.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "ini_parse.h"

#define LOCK_FILE "/var/run/backupd.pid"


int is_daemon_running()
{
    int     fd;
    char    buf[16];

    struct flock file_lock = {F_WRLCK, SEEK_SET, 0, 0, 0};
    
    fd = open(LOCK_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (fd < 0) {
      perror("open file lock failed");
      exit(1);
    }
    
    if (fcntl(fd, F_SETLK, &file_lock) < 0) {
      if (errno == EACCES || errno == EAGAIN) {
	close(fd);
	return (1);
      }
      perror("Cannot lock file");
      exit(1);
    }
    
    ftruncate(fd, 0);
    
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);
    
    return (0);
}

void monitor_fs(char *cfg_file)
{
  int fd;
  int wd;

  struct timeval time;
  fd_set descript = {0};
  int ret;

  ini_data_st *cfg;
  char *ptr;
  char watch_dir[PATH_MAX];
  char backup_dir[PATH_MAX];
  
  if ((fd = inotify_init()) < 0) {
    exit(1);
  }

  cfg = ini_init(cfg_file);
  ptr = ini_get_data(cfg, "SOURCE DIR", "PATH");
  if (!ptr) {
    ini_free(cfg);
    exit(1);
  }

  strcpy(watch_dir, ptr);

  if ((wd = inotify_add_watch(fd, watch_dir, IN_DELETE | IN_MODIFY | IN_MOVE | IN_CREATE)) < 0) {
    exit(1);
  }

  ptr = ini_get_data(cfg, "DESTINATION DIR", "PATH");
  if (!watch_dir) {
    ini_free(cfg);
    exit(1);
  }
  
  strcpy(backup_dir, ptr);

  ini_free(cfg);
  
  while (1) {
    
    time.tv_sec = 5;
    time.tv_usec = 0;
    
    FD_SET (fd, &descript);
    
    ret = select (fd + 1, &descript, NULL, NULL, &time);
    if (ret < 0) {
      exit(1);
    } else if (!ret) {
      exit(1);
    } else if (FD_ISSET (fd, &descript)) {
      char buf[1024 * sizeof(struct inotify_event)];
      int len, i = 0;
      
      len = read (fd, buf, 1024 * sizeof(struct inotify_event));
      if (len < 0) {
	if (errno == EINTR) {
	  // syscall was interrupted, reissue call
	  continue;
	} else {
	  // some other error
	  exit(1);
	} 
      } else if (!len) {
	// this shouldnt happen. if it does...blow up
	exit(1);
      }
      
      while (i < len) {
	struct inotify_event *event;
	
	event = (struct inotify_event *) &buf[i];
	
	switch (event->mask) {
	case IN_DELETE:
	case IN_MOVED_FROM:
	{
	  char file_name[1024];
	  snprintf(file_name, sizeof(file_name), "%s/%s", backup_dir, event->name);
	  unlink(file_name);
	  break;
	}
	    
	case IN_CREATE:
	case IN_MOVED_TO:
	case IN_MODIFY:
	{
	  char in_file_name[1024];
	  char out_file_name[1024];
	  snprintf(in_file_name, sizeof(in_file_name), "%s/%s", watch_dir, event->name);
	  snprintf(out_file_name, sizeof(out_file_name), "%s/%s", backup_dir, event->name);
	  
	  int in_fd = open(in_file_name, O_RDONLY);
	  assert(in_fd >= 0);
	  int out_fd = open(out_file_name, O_WRONLY | O_CREAT);
	  
	  assert(out_fd >= 0);
	  
	  char buf[8192];
	  
	  ssize_t result = read(in_fd, &buf[0], sizeof(buf));
	  while (result) {
	    assert(result > 0);
	    assert(write(out_fd, &buf[0], result) == result);
	    result = read(in_fd, &buf[0], sizeof(buf));
	  }
	  
	  close(in_fd);
	  close(out_fd);
	  
	  struct stat fst;
	  stat(in_file_name,&fst);
	  chown(out_file_name,fst.st_uid,fst.st_gid); 
	  chmod(out_file_name,fst.st_mode);
	  break;
	}
	default:
	  //do something...
	  break;
	}
	
	i += sizeof(struct inotify_event) + event->len;
      }
    } 
    
  }
  
}


int main(int argc, char* argv[])
{
  FILE *fp = NULL;
  pid_t pid = 0;
  pid_t sid = 0;

  if (argc != 2) {
    fprintf(stderr, "usage: backupd <config file>\n");
    exit(1);
  }

  if ((pid = fork()) < 0) {
    perror("fork failed");
    exit(1);
  }

  if (pid > 0) {
    // parent process - kill to make child a daemon
    exit(0);
  }

  umask(0);

  if ((sid = setsid()) < 0) {
    exit(1);
  }

  chdir("/");

  // check to see if we are already running
  if (is_daemon_running()) {
    exit(1);
  }

  // by this point we will no longer log anything to stdout/stderr and we will not take in any
  // user input, so close the respective FDs
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  monitor_fs(argv[1]);
  
  return (0);
}
