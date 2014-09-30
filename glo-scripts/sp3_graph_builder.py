# -*- coding: utf-8 -*-
"""SP3 File visualizer

author: cwiz
date: 21.11.2010
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

o_obs = {}
c_obs = {}

class Observation:
    def __init__(self, satId):
        self.sateliteId = satId
        self.vals = {}
    
    def push_data(self, date, value):
        self.vals[date] = value
        
    def pop_all_data(self):    
        keys = []
        values = []
        for key in sorted(self.vals.keys()):
            keys.append(key)
            values.append(self.vals[key])
            
        return keys, values

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
    
    parse_file(fileName)

    counter = 0 
    for sv in c_obs.keys():
        plt.figure(counter)
        date, clock = c_obs[sv].pop_all_data()
        build_graph(date, clock, counter, "", "Calculated clock satelite" + str(sv))
        savefile()
        counter += 1
    
    for sv in o_obs.keys():    
        plt.figure(counter)
        date, clock = o_obs[sv].pop_all_data()
        build_graph(date, clock, counter, "", "SP3 clock satelite" + str(sv))  
        savefile()
        counter += 1
        
    for sv in c_obs.keys():
        plt.figure(counter)
        date1, clock1 = c_obs[sv].pop_all_data()
        date2, clock2 = o_obs[sv].pop_all_data()
        build_2graph(date1, clock1, counter, "Calculated clock", "satelite" + str(sv), date2, clock2, counter, "SP3 clock", "satelite" + str(sv))        
        savefile()
        counter += 1
        
    for sv1 in c_obs.keys():
        for sv2 in c_obs.keys():
           if sv1 != sv2:
               date1, clock1 = c_obs[sv1].pop_all_data()
               date2, clock2 = c_obs[sv2].pop_all_data()
               
               date3, clock3 = o_obs[sv1].pop_all_data()
               date4, clock4 = o_obs[sv2].pop_all_data()
               
               if len(date1) == len(date2):
                   plt.figure(counter)
                   build_graph(date1, array(clock1) - array(clock2), counter, "","Calculated difference satelites " + str(sv1) + " and " + str(sv2))
                   counter += 1
                   savefile()
                   
                   plt.figure(counter)
                   build_graph(date1, array(clock3) - array(clock4), counter, "","SP3 difference satelites " + str(sv1) + " and " + str(sv2))
                   counter += 1
                   savefile()
    

def parse_file(fileName):
    file = open(fileName)    
    toggle = True    
    date = None
    while 1:
        line = file.readline()        
        if not line:
            break
        if line[0] == "*":
            date = process_date(line)
            toggle = True
        else:
            if date == None:
                continue
            
            satnum, x, y, z, clock = process_observation(line)
            if(toggle):
                add_observation(o_obs, satnum, date, clock)
            else:
                add_observation(c_obs, satnum, date, clock)
                
            toggle = not toggle  

def add_observation(collection, sv, date, value):
    if sv not in collection:
        collection[sv] = Observation(sv)
    collection[sv].push_data(date, value)       
            
def process_observation(line):
    elements = line.split(" ")    
    satnum = 0
    x = 0.0
    y = 0.0
    z = 0.0
    clock = 0.0
    
    counter = 0    
    for element in elements:
        newVal = element.strip()
        if len(newVal) == 0:            
            continue
            
        counter += 1
        
        if( counter == 1 ):
            satTypeExtraOffset = 0;
            if newVal[1] == "G":
                satTypeExtraOffset = 100
            satnum = int(newVal[2:]) + satTypeExtraOffset       
        if( counter == 2 ):
            x = float(newVal)
        if( counter == 3 ):
            y = float(newVal)
        if( counter == 4 ):
            z = float(newVal)
        if( counter == 5 ):
            clock = float(newVal)
      
    return satnum, x, y, z, clock
        
def process_date(line):
    line = line.replace("*", "").strip()
    elements = line.split(" ")
    
    year = 0
    month = 0
    day = 0
    hour = 0
    minute = 0
    second = 0
    
    counter = 0
    
    for element in elements:
        newVal = element.strip()
        if len(newVal) == 0:            
            continue
            
        counter += 1
        
        if( counter == 1 ):
            year = int(newVal)        
        if( counter == 2 ):
            month = int(newVal)
        if( counter == 3 ):
            day = int(newVal)
        if( counter == 4 ):
            hour = int(newVal)
        if( counter == 5 ):
            minute = int(newVal)
        if( counter == 6 ):
            second = int(newVal)
    
    return datetime(year, month, day, hour, minute, second)        

gtitle = ""

def build_graph(date, vals, figure, legend, title):  

    global gtitle  
    gtitle = title  
    #plt.title(title)
    plt.xticks(rotation=90)
    plt.grid()
    
    ax = plt.axes()
    y_formatter = FuncFormatter(lambda x,pos: ("%.3f"%x).replace(".",","))
    x_formatter = FuncFormatter(lambda x,pos: num2date(x).strftime('%H:%M'))
    ax.yaxis.set_major_formatter(y_formatter)
    ax.xaxis.set_major_formatter(x_formatter)
    
    plt.plot(date, vals, label=legend)    
    plt.ylabel("Clock [microseconds]")
    plt.xlabel("Time(UTC)")    

def build_2graph(date, vals, figure, legend, title, date1, vals1, figure1, legend1, title1):  
    global gtitle  
    gtitle = title    
    #plt.title(title)
    plt.xticks(rotation=90)
    plt.grid()  

    ax = plt.axes()
    y_formatter = FuncFormatter(lambda x,pos: ("%.3f"%x).replace(".",","))
    x_formatter = FuncFormatter(lambda x,pos: num2date(x).strftime('%H:%M'))
    ax.yaxis.set_major_formatter(y_formatter)
    ax.xaxis.set_major_formatter(x_formatter)

    p1 = plt.plot(date, vals, label=legend) 
    p2 = plt.plot(date1, vals1, label=legend1)  
    plt.legend([p1, p2], [legend, legend1])  
    plt.ylabel("Clock [microseconds]")
    plt.xlabel("Time(UTC)")   
  
def savefile():
  plt.savefig(gtitle + ".png")  

if __name__ == "__main__":
    main()
