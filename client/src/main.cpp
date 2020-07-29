#include "unistd.h"
#include <iostream>

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

/* Read all available inotify events from the file descriptor 'fd'.
        wd is the table of watch descriptors for the directories in argv.
        argc is the length of wd and argv.
        argv is the list of watched directories.
        Entry 0 of wd and argv is unused. */

static void handle_events(int fd, int *wd, int argc) {
  /* Some systems cannot read integer variables if they are not
     properly aligned. On other systems, incorrect alignment may
     decrease performance. Hence, the buffer used for reading from
     the inotify file descriptor should have the same alignment as
     struct inotify_event. */

  char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
  const struct inotify_event *event;
  ssize_t len;
  char *ptr;

  /* Loop while events can be read from inotify file descriptor. */

  for (;;) {

    /* Read some events. */
    std::clog << "entrato in handle loop\n";
    len = read(fd, buf, sizeof buf);
    if (len == -1 && errno != EAGAIN) {
      perror("read");
      exit(-1);
    }

    /* If the nonblocking read() found no events to read, then
       it returns -1 with errno set to EAGAIN. In that case,
       we exit the loop. */

    if (len <= 0)
      break;

    /* Loop over all events in the buffer */

    for (ptr = buf; ptr < buf + len;
         ptr += sizeof(struct inotify_event) + event->len) {

      event = (const struct inotify_event *)ptr;

      /* Print event type */
      if (event->mask & IN_ATTRIB)
        std::clog << "IN_ATTRIB: " << event->name << "\n";
      if (event->mask & IN_OPEN)
        std::clog << "IN_OPEN: " << event->name << "\n";
      if (event->mask & IN_CLOSE_NOWRITE)
        std::clog << "IN_CLOSE_NOWRITE: " << event->name << "\n";
      if (event->mask & IN_CLOSE_WRITE)
        std::clog << "IN_CLOSE_WRITE: " << event->name << "\n";

      /* Print the name of the watched directory */

      if (wd[0] == event->wd) {
        printf("%s/", "./sync");
        break;
      }

      /* Print the name of the file */

      if (event->len)
        printf("%s", event->name);

      /* Print type of filesystem object */

      if (event->mask & IN_ISDIR)
        printf(" [directory]\n");
      else
        printf(" [file]\n");
    }
  }
}

int main() {
  int fd, poll_num;
  int *wd;
  nfds_t nfds;
  struct pollfd fds[1];

  /* Create the file descriptor for accessing the inotify API */

  fd = inotify_init1(IN_NONBLOCK);
  if (fd == -1) {
    perror("inotify_init1");
    exit(-1);
  }

  /* Allocate memory for watch descriptors */

  wd = (int *)calloc(1, sizeof(int));
  if (wd == NULL) {
    perror("calloc");
    exit(-1);
  }

  /* Mark directories for events
     - file was opened
     - file was closed */

  wd[0] = inotify_add_watch(fd, "./sync", IN_OPEN | IN_CLOSE);
  if (wd[0] == -1) {
    fprintf(stderr, "Cannot watch '%s': %s\n", "../sync", strerror(errno));
    exit(-1);
  }

  /* Prepare for polling */

  nfds = 1;

  /* Console input */

  //  fds[0].fd = STDIN_FILENO;
  // fds[0].events = POLLIN;

  /* Inotify input */

  fds[0].fd = fd;
  fds[0].events = POLLIN;

  /* Wait for events and/or terminal input */

  std::clog << "Listening for events.\n";

  while (1) {
    poll_num = poll(fds, nfds, -1);
    // std::clog << "poll_num: " << poll_num << "\n";
    if (poll_num == -1) {
      if (errno == EINTR)
        continue;
      perror("poll");
      exit(-1);
    }

    if (poll_num > 0) {

      if (fds[0].revents & POLLIN) {

        /* Inotify events are available */
        handle_events(fd, wd, 2);
      }
    }
  }
  std::clog << "Listening for events stopped.\n";

  /* Close inotify file descriptor */

  close(fd);

  free(wd);
  exit(0);
}