#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

const int NUM_ITEMS = 10;
const char *BUFFER_FILE   = "buffer.txt";
const char *OUTPUT_FILE   = "output.txt";
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

    std::ofstream out(OUTPUT_FILE, std::ios::app);
    if (!out)
    {
        std::perror("Cannot open output file");
        return 1;
    }

    int consumed = 0;

    while (consumed < NUM_ITEMS)
    {
        sleep(static_cast<unsigned int>(std::rand() % 3 + 1));

        while (!lock_exists(CONSUMED_LOCK))
        {
            usleep(100000);
        }

        remove_lock(CONSUMED_LOCK);

        std::ifstream buf(BUFFER_FILE);
        if (!buf)
        {
            std::perror("Cannot open buffer file for reading");
            return 1;
        }

        int value;
        if (!(buf >> value))
        {
            std::cerr << "Buffer file did not contain a valid number\n";
            return 1;
        }
        buf.close();

        out << value << std::endl;
        out.flush();

        create_lock(PRODUCED_LOCK);

        consumed++;
    }

    return 0;
}