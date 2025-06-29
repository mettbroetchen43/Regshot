/*
    Copyright 2011-2012 Regshot Team

    This file is part of Regshot.

    Regshot is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    Regshot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Regshot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "global.h"

int main(int argc, char *argv[])
{
#ifdef _WIN64
    printf("Cannot be compiled as 64bit program.\n");
    return 1;
#else
    printf("Regshot hive file transfer program version 1.8.3beta1V4\n\
Transfer hive file used in <= 1.8.2 to new structure used in 1.8.3\n\
Usage: 182to183 infile.hiv outfile.hiv\n\n");
    if (argc < 3) {
        printf("[X] Need input and output filenames!\n");
        return 1;
    }
    if (LoadHive(argv[1], &lpHeadLocalMachine, &lpHeadUsers, &lpHeadFile) != TRUE) {
        return 1;
    }
    SaveHive(argv[2], lpHeadLocalMachine, lpHeadUsers, lpHeadFile);

    return 0;
#endif
}
