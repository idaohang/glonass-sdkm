# -*- coding: utf-8 -*-
"""
Created on Sat Dec  4 03:31:08 2010

@author: cwiz
"""

import sys
import getopt
from datetime import datetime
import matplotlib.pyplot as plt
from numpy import array
import matplotlib as mpl
from numpy import arange
from matplotlib.ticker import FuncFormatter

from time import strftime
from matplotlib.dates import num2date

def main():    
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
    except getopt.error, msg:
        print msg
        print "use --help for help"
        sys.exit(2)
    # process options
    for o, a in opts:
        if o in ("-h", "--help"):
            print __doc__
            sys.exit(0)

    fileName = args[0]
    
    clock, a, b, c = parse_file(fileName)
    sat = fileName.replace(".txt", "")
    graph(clock, a, 1, "", sat + "a", "a [sec]") 
    graph(clock, b, 2, "", sat + "b", "b [sec]")
    graph(clock, c, 3, "", sat + "c", "c [sec]")
    

def graph(date, vals, figure, legend, title, ylabel): 
    plt.figure(figure)
    plt.grid()    
    ax = plt.axes()
    y_formatter = FuncFormatter(lambda x,pos: ("%.1e"%x).replace(".",","))
    x_formatter = FuncFormatter(lambda x,pos: num2date(x).strftime('%H:%M'))
    ax.yaxis.set_major_formatter(y_formatter)
    #ax.xaxis.set_major_formatter(x_formatter)
    plt.plot(date, vals, label=legend)    
    plt.ylabel(ylabel)
    plt.xlabel("Time of day(UTC) [msec]") 
    plt.savefig("component-" + title + ".png")  
 

def parse_file(fileName):
    file = open(fileName)    
    toggle = True    
    date = None
    
    aA = []
    bA = []
    cA = []
    clockA = []
    
    while 1:
        line = file.readline()
        
        if not line:
            break
        
        vals = line.split(' ')
        
        clock = int(vals[0])
        a = float(vals[1])/299792458*1e6
        b = float(vals[2])/299792458*1e6
        c = float(vals[3])/299792458*1e6
       
        if clock >= 35527937:
            break
        
        aA.append(a)
        bA.append(b)
        cA.append(c)
        clockA.append(clock)
    
    return clockA, aA, bA, cA
        
if __name__ == "__main__":
    main()
