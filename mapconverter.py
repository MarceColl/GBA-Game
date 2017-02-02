import csv

with open('res/map_BG0.csv', 'r+') as bg0file,  open('res/map_BG1.csv', 'r+') as bg1file, open('res/map_BG2.csv', 'r+') as bg2file, open('source/map.h', 'w') as mapheader, open('source/map.c', 'w') as mapsource:
    mapheader.write('// Map header\n')

    mapreader = csv.reader(bg0file, delimiter=',')

    mapsource.write('// Map source\n')
    mapsource.write('#include <tonc.h> \n')
    mapsource.write('#include "map.h" \n')
    
    
    mapsource.write('SCR_ENTRY bg0[32][32] = {')
    for row in mapreader:
        mapsource.write('{')

        for column in row:
            mapsource.write(column + ', ') 

        mapsource.write('},')

    mapsource.write('};\n')
    mapsource.write('unsigned int bg0Len = 32*32;\n')


    mapreader = csv.reader(bg1file, delimiter=',')

    mapsource.write('SCR_ENTRY bg1[32][32] = {')
    for row in mapreader:
        mapsource.write('{')

        for column in row:
            mapsource.write(column + ', ') 

        mapsource.write('},')

    mapsource.write('};\n')
    mapsource.write('unsigned int bg1Len = 32*32;\n')


    
    mapreader = csv.reader(bg2file, delimiter=',')

    mapsource.write('SCR_ENTRY bg2[32][32] = {')
    for row in mapreader:
        mapsource.write('{')

        for column in row:
            mapsource.write(column + ', ') 

        mapsource.write('},')

    mapsource.write('};\n')
    mapsource.write('unsigned int bg2Len = 32*32;\n')
    
    

    mapheader.write('extern SCR_ENTRY bg0[32][32]; \n')
    mapheader.write('extern SCR_ENTRY bg1[32][32]; \n')
    mapheader.write('extern SCR_ENTRY bg2[32][32]; \n')
    mapheader.write('extern unsigned int bg0Len; \n')
    mapheader.write('extern unsigned int bg1Len; \n')
    mapheader.write('extern unsigned int bg2Len; \n')
