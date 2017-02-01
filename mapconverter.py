import csv

with open('res/map.csv', 'r+') as mapfile, open('source/map.h', 'w') as mapheader, open('source/map.c', 'w') as mapsource:
    mapheader.write('// Map header\n')

    mapreader = csv.reader(mapfile, delimiter=',')

    mapsource.write('// Map source\n')
    mapsource.write('#include <tonc.h> \n')
    mapsource.write('#include "map.h" \n')
    mapsource.write('SCR_ENTRY map[32][32] = {')

    for row in mapreader:
        mapsource.write('{')

        for column in row:
            mapsource.write(column + ', ') 

        mapsource.write('},')

    mapsource.write('};\n')

    mapsource.write('unsigned int mapLen = 32*32;\n')

    mapheader.write('extern SCR_ENTRY map[32][32]; \n')
    mapheader.write('extern unsigned int mapLen; \n')
