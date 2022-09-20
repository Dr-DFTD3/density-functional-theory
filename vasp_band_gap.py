#!/usr/bin/python

import sys
import argparse
import numpy as np

# homo=`awk '/NELECT/ {print $3/2}' $1`
# lumo=`awk '/NELECT/ {print $3/2+1}' $1`
# nkpt=`awk '/NKPTS/ {print $4}' $1`

# e1=`grep "     $homo     " $1 | head -$nkpt | sort -n -k 2 | tail -1 | awk '{print $2}'`
# e2=`grep "     $lumo     " $1 | head -$nkpt | sort -n -k 2 | head -1 | awk '{print $2}'`

# echo "HOMO: band:" $homo " E=" $e1
# echo "LUMO: band:" $lumo " E=" $e2


class Point:
	def __init__(self):
		x = 0
		y = 0
		z = 0
		xs = 0
		ys = 0
		zs = 0
		globID = 0
		typeID = 0 
		vx = 0
		vy = 0
		vz = 0

def get_arguments(argv):
	cli = argparse.ArgumentParser(description='Utility to convert between various file types')
	# cli.add_argument("FILE",help="input file containig data to process",type=str)
	cli.add_argument("FILE",help="input file containig data to process",type=str)
	cli.add_argument("-f","--flat",dest="flatten",help="force all atoms into a 2D plane; z = (zhi+zlo)/2",action="store_true")
	cli.add_argument("-g","--debug", dest="verb",help="define level ofw debuging messages",default=0,type=int,choices = [1,2,3])
	cli.add_argument("-i","--ids",dest="ids",help="preserve global ID tags from the dump file",action="store_true")
	cli.add_argument("-m","--mod", dest="mods",help="modify the data",type=str,default=None)
	cli.add_argument("-t","--types", dest="nt",help="indicate how many unique types are in the file",default=1,type=int)
	cli.add_argument("-v","--velocities",dest="velocities",help="preserve global ID tags from the dump file",action="store_true")
	cli.add_argument('--version', action='version', version='%(prog)s 0.0.1')
	args = cli.parse_args()
	if args.verb>0: print "verbosity level: ", args.verb
	if args.verb>1: print "verbosity level: ", args.verb

	return args


def get_simulation_context(fname):

	outcar = file(fname,"r")
	natoms = 0
	nkpts = 0
	nbands = 0
	nelect = 0
	context = []

	for line in outcar:
		# print line
		# find the line indicating the start of bbox
		if "NKPTS" in line:
			# print line
			dLine = line.split()
			data = [str(word) for word in dLine]
			nkpts = data[3]
			nbands = data[len(data)-1]
		if "NIONS" in line:
			dLine = line.split()
			data = [str(word) for word in dLine]
			natoms = data[len(data)-1]
		if "NELECT" in line:
			# print line
			dLine = line.split()
			data = [str(word) for word in dLine]
			nelect = data[2]
			break

	outcar.close()
	return natoms,nelect,nkpts,nbands



def main(argv):

	args = get_arguments(argv)

	natoms,nelect,nkpts,nbands = get_simulation_context(args.FILE)

	natoms = int(natoms)
	nelect = float(nelect)
	nkpts = int(nkpts)
	nbands = int(nbands)

	print natoms,nelect,nkpts,nbands

	# outcar.readlines()


# run the routine
if __name__ == "__main__": main(sys.argv[1:])



