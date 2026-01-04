#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

const int NUM_ITEMS = 10;
const char *BUFFER_FILE   = "buffer.txt";
const char *INPUT_FILE    = "input.txt";
const char *PRODUCED_LOCK = "produced.lock";
const char *CONSUMED_LOCK = "consumed.lock";

void create_lock(const char *filename)
{
    int fd = open(filename, O_CREAT | O_WRONLY, 0666);
    if (fd >= 0)
    {
        close(fd);
    }
}

void remove_lock(const char *filename)
{
    unlink(filename);
}

bool lock_exists(const char *filename)
{
    return (access(filename, F_OK) == 0);
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)) ^ getpid());

    if (!lock_exists(PRODUCED_LOCK))
    {
        create_lock(PRODUCED_LOCK);
    }
    if (lock_exists(CONSUMED_LOCK))
    {
        remove_lock(CONSUMED_LOCK);
    }

    std::ifstream in(INPUT_FILE);
    if (!in)
    {
        std::perror("Cannot open input file");
        return 1;
    }

    int produced = 0;
    int value;

    while (produced < NUM_ITEMS && (in >> value))
    {
        sleep(static_cast<unsigned int>(std::rand() % 3 + 1));

        while (!lock_exists(PRODUCED_LOCK))
        {
            usleep(100000);
        }

        remove_lock(PRODUCED_LOCK);

        std::ofstream buf(BUFFER_FILE);
        if (!buf)
        {
            std::perror("Cannot open buffer file for writing");
            return 1;
        }
        buf << value << std::endl;
        buf.close();

        create_lock(CONSUMED_LOCK);

        produced++;
    }

    return 0;
}